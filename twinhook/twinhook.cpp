#include "stdafx.h"
#include "th_di8_hook.h"
#include "th08_player.h"
#include "th_d3d9_hook.h"
#include "th08_bullet_process_hook.h"


BOOL WINAPI DllMain(HMODULE hModule, DWORD reasonForCall, LPVOID lpReserved)
{
	switch (reasonForCall)
	{
	case DLL_PROCESS_ATTACH:
	{
		CoInitialize(nullptr);
		DisableThreadLibraryCalls(hModule);
		LOG("DllMain: dll attached");
		th08_player *player = new th08_player;
		th_d3d9_hook::bind(player);
		th_di8_hook::bind(player);
		th08_bullet_process_hook::bind(player);
		break;
	}
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
