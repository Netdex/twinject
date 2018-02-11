#include "stdafx.h"



BOOL WINAPI DllMain(HMODULE hModule, DWORD reasonForCall, LPVOID lpReserved)
{
	logfs("DllMain loaded");	

	switch (reasonForCall)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		logfs("DllMain: Debugger attached");

		
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}