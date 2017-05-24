#include "stdafx.h"
#include "LoadLibraryAHook.h"
#include "Direct3DCreate9Hook.h"
#include "detour.h"

extern PBYTE pDirect3DCreate9;
extern Direct3DCreate9_t Direct3DCreate9_Original;

bool HookedD3D = false;

LoadLibrary_t LoadLibraryA_Original;
HMODULE WINAPI LoadLibraryA_Hook(LPCSTR lpFileName)
{
	HMODULE hM = LoadLibraryA_Original(lpFileName);
	logfs("LoadLibrary: %s" , lpFileName);
	if (!HookedD3D && strcmp(lpFileName, "d3d9.dll") == 0)
	{
		pDirect3DCreate9 = (PBYTE)GetProcAddress(hM, "Direct3DCreate9");
		logfs("D3D9: Found Direct3DCreate9 at addr %d", (int)pDirect3DCreate9);
		Direct3DCreate9_Original = (Direct3DCreate9_t)(pDirect3DCreate9);
		Hook_D3D9_Direct3DCreate9();
		HookedD3D = true;
	}
	return hM;
}
void Hook_Kernel32_LoadLibraryA()
{
	LoadLibraryA_Original = LoadLibraryA;

	if (DetourFunction(&(PVOID&)LoadLibraryA_Original, LoadLibraryA_Hook))
		logfs("Detours: Hooked LoadLibraryA");
	else
		logfs("Detours: Failed to hook LoadLibraryA");
}
