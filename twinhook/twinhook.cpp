#include "stdafx.h"


BOOL WINAPI DllMain(HMODULE hModule, DWORD reasonForCall, LPVOID lpReserved)
{
	switch (reasonForCall)
	{
	case DLL_PROCESS_ATTACH:
	{
		//CoInitialize(nullptr);
		DisableThreadLibraryCalls(hModule);
		LOG("DllMain: dll attached");
		/*th08_player *player = new th08_player;
		th_d3d9_hook hk(player);
		hk.hook();*/
		//Hook_TH08_sub_446A37(); // TODO remove
		break;
	}
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
