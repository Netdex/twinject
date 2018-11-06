#include "stdafx.h"
#include <unordered_map>

#include "hook/th_di8_hook.h"
#include "hook/th_d3d9_hook.h"
#include "hook/th07_bullet_proc_hook.h"
#include "hook/th08_bullet_proc_hook.h"
#include "hook/th15_bullet_proc_hook.h"

#include "control/th06_player.h"
#include "control/th07_player.h"
#include "control/th08_player.h"
#include "control/th10_player.h"
#include "control/th11_player.h"
#include "control/th15_player.h"

#include "algo/th_vo_algo.h"

#include "patch/th_patch_registry.h"
#include "gfx/imgui_window.h"

void th06_init()
{
	th06_player *player = new th06_player();
	th_vo_algo *algo = new th_vo_algo(player);
	player->bindAlgorithm(algo);

	th_d3d9_hook::bind(player, true);
	th_di8_hook::bind(player);
}
void th07_init()
{
	th07_player *player = new th07_player();
	th_vo_algo *algo = new th_vo_algo(player);
	player->bindAlgorithm(algo);

	th_d3d9_hook::bind(player, true);
	th_di8_hook::bind(player);
	th07_bullet_proc_hook::bind(player);
}

void th08_init()
{
	th08_player *player = new th08_player();
	th_vo_algo *algo = new th_vo_algo(player);
	player->bindAlgorithm(algo);

	th_d3d9_hook::bind(player, true);
	th_di8_hook::bind(player);
	th08_bullet_proc_hook::bind(player);
}

void th10_init()
{
	th10_player *player = new th10_player();
	th_vo_algo *algo = new th_vo_algo(player);
	player->bindAlgorithm(algo);
	th_d3d9_hook::bind(player, false);
	th_di8_hook::bind(player);
	// no bullet proc hook due to polling
}

void th11_init()
{
	th11_player *player = new th11_player();
	th_vo_algo *algo = new th_vo_algo(player);
	player->bindAlgorithm(algo);
	th_d3d9_hook::bind(player, false);
	th_di8_hook::bind(player);
	th_registry::patch("th11_dinput_fix");
}

void th15_init()
{
	th15_player *player = new th15_player();
	th_vo_algo *algo = new th_vo_algo(player, true);
	player->bindAlgorithm(algo);
	th_d3d9_hook::bind(player, false);
	th_di8_hook::bind(player);
	th15_bullet_proc_hook::bind(player);
}

typedef void(*th_loader_t)();

static std::unordered_map<std::string, th_loader_t> th_init{
	{"th06", th06_init},
	{"th07", th07_init},
	{"th08", th08_init},
	{"th10", th10_init},
	{"th11", th11_init},
	{"th15", th15_init}
};

BOOL WINAPI DllMain(HMODULE hModule, DWORD reasonForCall, LPVOID lpReserved)
{
	switch (reasonForCall)
	{
	case DLL_PROCESS_ATTACH:
	{
		// disable COM warnings
		CoInitialize(nullptr);
		DisableThreadLibraryCalls(hModule);
		LOG("DllMain: dll attached");

		// get game name from environment variable passed from twinject
		// we should probably use IPC instead of envvars, but it works so hey
		size_t len;
		char buf[256];
		getenv_s(&len, buf, 256, "th");
		if (strcmp(buf, "") == 0)
		{
			LOG("WARNING: The game loader to use is not specified so no loader will "
				"be used, this is probably not what you want!");
		}
		else {
			LOG("detected game: '%s'", buf);

			// call respective game initialization routine based on envvar
			if (th_init.find(buf) != th_init.end())
			{
				th_init[buf]();
			}
			else
			{
				LOG("WARNING: no implementation exists for this game");
			}
		}
		break;
	}
	case DLL_PROCESS_DETACH:
		imgui_window_cleanup();
		break;
	}
	return TRUE;
}
