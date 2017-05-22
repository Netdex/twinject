#include "stdafx.h"
#include <detours.h>
#include <fstream>
#include <cstdio>
#include "ID3D9Wrapper.h"

std::ofstream logfs;

HANDLE HookHandle = NULL;

IDirect3D9*  (WINAPI *Real_Direct3DCreate9)(UINT SDKVersion) = Direct3DCreate9;

IDirect3D9* WINAPI Mine_Direct3DCreate9(UINT SDKVersion)
{
	logfs << "called alternate direct3d create" << std::endl;
	IDirect3D9* Direct3D = Real_Direct3DCreate9(SDKVersion);
	IDirect3D9* Mine_Direct3D = new Direct3D9Wrapper(Direct3D);
	return Mine_Direct3D;
}


void HookAPI()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	if (DetourAttach(&(PVOID&)Real_Direct3DCreate9, Mine_Direct3DCreate9))
	{
		logfs << "failed to attach" << std::endl;
	}
	else
	{
		logfs << "successfully attached" << std::endl;
	}
	if (DetourTransactionCommit() == NO_ERROR)
	{
		logfs << "set function detour" << std::endl;
	}
	else
	{
		logfs << "failed to set function detour" << std::endl;
	}
}



typedef HMODULE(WINAPI *LoadLibrary_t)(LPCSTR);
LoadLibrary_t orig_LoadLibrary; // holds address of original non-detoured function

								// Our hooked LoadLibrary

HMODULE WINAPI LoadLibrary_Hook(LPCSTR lpFileName)
{
	HMODULE hM = orig_LoadLibrary(lpFileName); // keep functionality
	logfs << lpFileName << std::endl;
	return hM;
}

void HookLoad()
{
	orig_LoadLibrary = LoadLibraryA;

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	if (DetourAttach(&(PVOID&)orig_LoadLibrary, LoadLibrary_Hook))
	{
		logfs << "failed to attach" << std::endl;
	}
	else
	{
		logfs << "successfully attached" << std::endl;
	}
	if (DetourTransactionCommit() == NO_ERROR)
	{
		logfs << "set function detour" << std::endl;
	}
	else
	{
		logfs << "failed to set function detour" << std::endl;
	}
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD reasonForCall, LPVOID lpReserved)
{
	logfs.open("twinhook.log");
	logfs << "dll executed" << std::endl;

	HANDLE hProcess = GetCurrentProcess();

	switch (reasonForCall)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		HookHandle = hModule;
		HookAPI();

		///HookLoad();
		break;

	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}