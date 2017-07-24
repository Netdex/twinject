#include "stdafx.h"

#include "BulletProcessHook.h"
#include "LoadLibraryAHook.h"
#include "DirectInput8CreateHook.h"

HANDLE HookHandle = NULL;
HANDLE ProcessHandle = NULL;

BOOL WINAPI DllMain(HMODULE hModule, DWORD reasonForCall, LPVOID lpReserved)
{
	logfs("DllMain injected");	

	ProcessHandle = GetCurrentProcess();

	switch (reasonForCall)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		logfs("DllMain: Debugger attached");
		HookHandle = hModule;
		Hook_Kernel32_LoadLibraryA();
		Hook_TH08_sub_410A70();
		Hook_TH08_sub_447A37();
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}