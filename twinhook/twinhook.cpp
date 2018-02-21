#include "stdafx.h"
#include "th_di8_hook.h"
#include "th08_player.h"
#include "th_d3d9_hook.h"
#include "th08_bullet_proc_hook.h"
#include "th07_player.h"
#include "th07_bullet_proc_hook.h"

void th07_init()
{
	th07_player *player = new th07_player;
	th_d3d9_hook::bind(player);
	th_di8_hook::bind(player);
	th07_bullet_proc_hook::bind(player);
}

void th08_init()
{
	th08_player *player = new th08_player;
	th_d3d9_hook::bind(player);
	th_di8_hook::bind(player);
	th08_bullet_proc_hook::bind(player);
}

typedef void(*th_loader_t)();
static std::unordered_map<std::string, th_loader_t> th_init{
	{"th07", th07_init},
	{"th08", th08_init}
};

BOOL WINAPI DllMain(HMODULE hModule, DWORD reasonForCall, LPVOID lpReserved)
{
	switch (reasonForCall)
	{
	case DLL_PROCESS_ATTACH:
	{
		CoInitialize(nullptr);
		DisableThreadLibraryCalls(hModule);
		LOG("DllMain: dll attached");

		size_t len;
		char buf[256];
		getenv_s(&len, buf, 256, "th");
		LOG("detected game: %s", buf);
		if (th_init.find(buf) != th_init.end())
		{
			th_init[buf]();
		}
		else
		{
			LOG("no implementation exists for this game");
		}
		break;
	}
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
