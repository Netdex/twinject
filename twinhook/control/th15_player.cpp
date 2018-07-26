#include "../stdafx.h"
#include "th15_player.h"
#include "../directx/IDI8ADevice_Wrapper.h"
#include "../config/th_config.h"
#include "../hook/th_di8_hook.h"
#include "../util/cdraw.h"
#include "../gfx/di8_input_overlay.h"
#include "../algo/th_algorithm.h"
#include "../patch/th15_patch_autobomb.h"
#include "../hook/th_d3d9_hook.h"
#include "gfx/th_info_overlay.h"


void th15_player::onInit()
{
	th_player::onInit();
}

void th15_player::onTick()
{
	th_player::onTick();
}

void th15_player::onBeginTick()
{
	th_player::onBeginTick();
	//tle.expand(lasers, bullets);
}

void th15_player::onAfterTick()
{
	th_player::onAfterTick();
}

void th15_player::draw(IDirect3DDevice9* d3dDev)
{
	th_player::draw(d3dDev);
}

void th15_player::handleInput(const BYTE diKeys[256], const BYTE press[256])
{
	th_player::handleInput(diKeys, press);
}

void th15_player::onEnableChanged(bool enable)
{
	th_player::onEnableChanged(enable);

	// this is cheating
	th15_patch_autobomb ptch;
	if (enable)
	{
		ptch.patch();
	}
	else
	{
		ptch.unpatch();
	}
}

entity th15_player::getPlayerEntity()
{
	PBYTE *PlayerPtrAddr = (PBYTE*)gs_ptr.plyr_pos; // this should be illegal
	if (*PlayerPtrAddr) {
		PBYTE plyrAddr = *PlayerPtrAddr;

		// we must check if size is readable, because it doesn't initialize immediately
		vec2 size;
		if (*(DWORD*)(plyrAddr + 0x2C008))
		{
			size = vec2(
				*(float*)(*(DWORD*)(plyrAddr + 0x2C008) + 4),
				*(float*)(*(DWORD*)(plyrAddr + 0x2C008) + 4));
		}

		entity e = {
			vec2(
				*(float*)(plyrAddr + 0x618) + th_param.GAME_WIDTH / 2,
				*(float*)(plyrAddr + 0x61C)),
			vec2(),
			size * 2,
			0
		};
		return e;
	}
	return {};
}



