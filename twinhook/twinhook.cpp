#include "stdafx.h"

#include <unordered_map>

#include <spdlog/spdlog.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/msvc_sink.h>

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
#include "util/spdlog_msvc.h"

struct twinhook_ctx
{
	std::shared_ptr<th_player> th_player;
	std::shared_ptr<th_algorithm> th_algo;
	std::shared_ptr<spdlog_msvc> logger;
};

twinhook_ctx* context;

void th06_init()
{
	context->th_player = std::make_shared<th06_player>();
	context->th_algo = std::make_shared<th_vo_algo>(context->th_player.get());
	context->th_player->bindAlgorithm(context->th_algo.get());

	th_d3d9_hook::bind(context->th_player.get(), true);
	th_di8_hook::bind(context->th_player.get());
}
void th07_init()
{
	context->th_player = std::make_shared<th07_player>();
	context->th_algo = std::make_shared<th_vo_algo>(context->th_player.get());
	context->th_player->bindAlgorithm(context->th_algo.get());

	th_d3d9_hook::bind(context->th_player.get(), true);
	th_di8_hook::bind(context->th_player.get());
	th07_bullet_proc_hook::bind(std::dynamic_pointer_cast<th07_player>(context->th_player).get());
}

void th08_init()
{
	context->th_player = std::make_shared<th08_player>();
	context->th_algo = std::make_shared<th_vo_algo>(context->th_player.get());
	context->th_player->bindAlgorithm(context->th_algo.get());

	th_d3d9_hook::bind(context->th_player.get(), true);
	th_di8_hook::bind(context->th_player.get());
	th08_bullet_proc_hook::bind(std::dynamic_pointer_cast<th08_player>(context->th_player).get());
}

void th10_init()
{
	context->th_player = std::make_shared<th10_player>();
	context->th_algo = std::make_shared<th_vo_algo>(context->th_player.get());
	context->th_player->bindAlgorithm(context->th_algo.get());

	th_d3d9_hook::bind(context->th_player.get(), false);
	th_di8_hook::bind(context->th_player.get());
	// no bullet proc hook due to polling
}

void th11_init()
{
	context->th_player = std::make_shared<th11_player>();
	context->th_algo = std::make_shared<th_vo_algo>(context->th_player.get());
	context->th_player->bindAlgorithm(context->th_algo.get());

	th_d3d9_hook::bind(context->th_player.get(), false);
	th_di8_hook::bind(context->th_player.get());
	th_registry::patch("th11_dinput_fix");
}

void th15_init()
{
	context->th_player = std::make_shared<th15_player>();
	context->th_algo = std::make_shared<th_vo_algo>(context->th_player.get());
	context->th_player->bindAlgorithm(context->th_algo.get());

	th_d3d9_hook::bind(context->th_player.get(), false);
	th_di8_hook::bind(context->th_player.get());
	th15_bullet_proc_hook::bind(std::dynamic_pointer_cast<th15_player>(context->th_player).get());
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

__declspec(dllexport) BOOL WINAPI DllMain(HMODULE hModule, DWORD reasonForCall, LPVOID lpReserved)
{
	switch (reasonForCall)
	{
	case DLL_PROCESS_ATTACH:
	{
		// disable COM warnings
		CoInitialize(nullptr);
		DisableThreadLibraryCalls(hModule);

		context = new twinhook_ctx;

		spdlog_msvc *logger = new spdlog_msvc{};
		spdlog::set_default_logger(logger->get_logger());
		spdlog::set_pattern("[%L:%!@%s:L%#] %v");
		SPDLOG_INFO("spdlog_msvc initialized");

		// get game name from environment variable passed from twinject
		// we should probably use IPC instead of envvars, but it works so hey
		size_t len;
		char buf[256];
		getenv_s(&len, buf, 256, "th");
		if (strcmp(buf, "") == 0)
		{
			SPDLOG_WARN("The game loader to use is not specified so no loader will "
				"be used, this is probably not what you want!");
		}
		else {
			SPDLOG_INFO("Detected game: '{}'", buf);

			// call respective game initialization routine based on envvar
			if (th_init.find(buf) != th_init.end())
			{
				th_init[buf]();
			}
			else
			{
				SPDLOG_WARN("No implementation exists for this game");
			}
		}
		break;
	}
	case DLL_PROCESS_DETACH:
		SPDLOG_INFO("Detaching from process");
		imgui_window_cleanup();
		delete context;
		break;
	}

	return TRUE;
}
