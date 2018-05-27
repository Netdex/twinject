#include "stdafx.h"
#include <unordered_map>

#include "hook/th_di8_hook.h"
#include "control/th08_player.h"
#include "hook/th_d3d9_hook.h"
#include "hook/th08_bullet_proc_hook.h"
#include "control/th07_player.h"
#include "hook/th07_bullet_proc_hook.h"
#include "algo/th_vo_algo.h"
#include "control/th15_player.h"
#include "hook/th15_bullet_proc_hook.h"
#include "algo/th_ann_algo.h"
#include "control/th10_player.h"
#include "hook/th10_bullet_proc_hook.h"

void th07_init()
{
	th07_player *player = new th07_player();
	//th_vector_algo *algo = new th_vector_algo(player);
	th_vo_algo *algo = new th_vo_algo(player);
	player->bind_algorithm(algo);

	th_d3d9_hook::bind(player, true);
	th_di8_hook::bind(player);
	th07_bullet_proc_hook::bind(player);
}

void th08_init()
{
	th08_player *player = new th08_player();
	//th_vector_algo *algo = new th_vector_algo(player);
	th_vo_algo *algo = new th_vo_algo(player);
	player->bind_algorithm(algo);

	th_d3d9_hook::bind(player, true);
	th_di8_hook::bind(player);
	th08_bullet_proc_hook::bind(player);
}

void th10_init()
{
	th10_player *player = new th10_player();
	th_vo_algo *algo = new th_vo_algo(player);
	player->bind_algorithm(algo);
	th_d3d9_hook::bind(player, false);
	th_di8_hook::bind(player);
	th10_bullet_proc_hook::bind(player);
}

void th15_init()
{
	th15_player *player = new th15_player();
	//th_vector_algo *algo = new th_vector_algo(player);
	th_vo_algo *algo = new th_vo_algo(player, true);
	//th_ann_algo *algo = new th_ann_algo(player, true);
	player->bind_algorithm(algo);
	th_d3d9_hook::bind(player, false);
	th_di8_hook::bind(player);
	th15_bullet_proc_hook::bind(player);
}
typedef void(*th_loader_t)();
static std::unordered_map<std::string, th_loader_t> th_init{
	{"th07", th07_init},
	{"th08", th08_init},
	{"th10", th10_init},
	{"th15", th15_init}
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
