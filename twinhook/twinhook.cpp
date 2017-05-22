#include "stdafx.h"
#include <detours.h>
#include <fstream>
#include <cstdio>

#include "ID3D9Wrapper.h"
#include "CDraw.h"

HANDLE HookHandle = NULL;

bool DetourFunction(PVOID *ppPointer, PVOID pDetour)
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	if (DetourAttach(ppPointer, pDetour))
		return false;
	if (DetourTransactionCommit() != NO_ERROR)
		return false;
	return true;
}


void BeginScene_Hook(IDirect3DDevice9 *d3dDev)
{
	CBeginScene(d3dDev);
}
void EndScene_Hook(IDirect3DDevice9 *d3dDev)
{
	CEndScene(d3dDev);
}

IDirect3D9* WINAPI Direct3DCreate9_Hook(UINT sdkVers);
typedef IDirect3D9* (WINAPI *Direct3DCreate9_t)(UINT SDKVersion);
Direct3DCreate9_t Direct3DCreate9_Original;
PBYTE pDirect3DCreate9;

IDirect3D9* WINAPI Direct3DCreate9_Hook(UINT sdkVers)
{
	logfs() << "Feeding fake IDirect3D9" << std::endl;
	IDirect3D9 *legit = Direct3DCreate9_Original(sdkVers);
	Direct3D9Wrapper *wrapper = new Direct3D9Wrapper(legit, BeginScene_Hook, EndScene_Hook);
	return wrapper;
}

void HookDirect3DCreate9()
{
	if (DetourFunction(&(PVOID&)Direct3DCreate9_Original, Direct3DCreate9_Hook))
		logfs() << "Hooked Direct3DCreate9" << std::endl;
	else
		logfs() << "Failed to hook Direct3DCreate9" << std::endl;
}


HMODULE WINAPI LoadLibraryA_Hook(LPCSTR lpFileName);
typedef HMODULE(WINAPI *LoadLibrary_t)(LPCSTR);
LoadLibrary_t LoadLibraryA_Original;

HMODULE WINAPI LoadLibraryA_Hook(LPCSTR lpFileName)
{
	HMODULE hM = LoadLibraryA_Original(lpFileName);
	if (strcmp(lpFileName, "d3d9.dll") == 0)
	{
		logfs() << "Loaded d3d9.dll" << std::endl;
		pDirect3DCreate9 = (PBYTE)GetProcAddress(hM, "Direct3DCreate9");
		logfs() << "Found Direct3DCreate9 at addr " << (int)pDirect3DCreate9 << std::endl;
		Direct3DCreate9_Original = (Direct3DCreate9_t)(pDirect3DCreate9);
		HookDirect3DCreate9();
	}
	return hM;
}

void HookLoadLibraryA()
{
	LoadLibraryA_Original = LoadLibraryA;

	if (DetourFunction(&(PVOID&)LoadLibraryA_Original, LoadLibraryA_Hook))
		logfs() << "Hooked LoadLibraryA" << std::endl;
	else
		logfs() << "Failed to hook LoadLibraryA" << std::endl;
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD reasonForCall, LPVOID lpReserved)
{
	logfs_init("twinhook.log");
	logfs() << "DllMain injected" << std::endl;

	HANDLE hProcess = GetCurrentProcess();

	switch (reasonForCall)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		HookHandle = hModule;
		HookLoadLibraryA();
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}