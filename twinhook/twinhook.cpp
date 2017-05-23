#include "stdafx.h"
#include <detours.h>
#include <fstream>
#include <cstdio>

#include "ID3D9Wrapper.h"
#include "CDraw.h"
#include <vector>

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

struct vec2
{
	float x;
	float y;
	float unknown;
};

std::vector<vec2> bullets;


void BeginScene_Hook(IDirect3DDevice9 *d3dDev)
{
	CDraw_InitSolidTexture(d3dDev);
	CDraw_InitFont(d3dDev, 17, L"Calibri");
}
void EndScene_Hook(IDirect3DDevice9 *d3dDev)
{
	CDraw_Text("TWINJECT [ NETDEX ]", D3DCOLOR_ARGB(255, 255, 255, 255), 0, 0, 640, 480);
	for (auto iter = bullets.begin(); iter != bullets.end(); ++iter)
	{
		CDraw_DrawRect(d3dDev, (*iter).x - 5 + 33, (*iter).y - 5 + 17, 10, 10, D3DCOLOR_ARGB(255, 255, 2, 200));
	}
	bullets.clear();
}


typedef int(__fastcall *BulletProcess_t)(int a1, int a2, int a3);
PBYTE pBulletProcess = (PBYTE)0x00410A70;
BulletProcess_t BulletProcess_Original = (BulletProcess_t)pBulletProcess;
__declspec(naked) int __fastcall BulletProcess_Hook(int a1, int a2, int a3)
{
	// this is why you need MSVC to compile this
	__asm {
		// get return address, put it somewhere safe
		mov eax, [esp]
		push ebp
		mov ebp, esp
		sub esp, __LOCAL_SIZE

		// move __fastcall arguments into function parameters
		mov a1, ecx
		mov a2, edx
		mov ecx, [ebp + 8]
		mov a3, ecx
	}

	unsigned int addr;
	__asm mov addr, eax

	// routine from bullet update
	if(addr == 0x004314B3)
	{
		vec2 bulletPos;
		vec2 bulletVel;
		bulletPos.x = *(float*)(a1 + 0);
		bulletPos.y = *(float*)(a1 + 4);
		bulletVel.x = *(float*)(a3 + 0);
		bulletVel.y = *(float*)(a3 + 4);
		logfs() << bulletPos.x << " " << bulletPos.y << std::endl;
		bullets.push_back(bulletPos);
	}
	
	int retval;
	retval = BulletProcess_Original(a1, a2, a3);
	__asm {
		mov eax, retval

		mov esp, ebp
		pop ebp
		ret
	}
}
void HookBulletProcess()
{
	if (DetourFunction(&(PVOID&)BulletProcess_Original, BulletProcess_Hook))
		logfs() << "Hooked BulletProcess" << std::endl;
	else
		logfs() << "Failed to hook BulletProcess" << std::endl;
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
		HookBulletProcess();
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}