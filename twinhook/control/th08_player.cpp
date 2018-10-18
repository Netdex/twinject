#include "../stdafx.h"
#include "th08_player.h"
#include "../directx/IDI8ADevice_Wrapper.h"
#include "../patch/th08_patch_autobomb.h"
#include "../config/th_config.h"
#include "../hook/th_di8_hook.h"
#include "../util/cdraw.h"
#include "../gfx/di8_input_overlay.h"
#include "../algo/th_algorithm.h"
#include "../hook/th_d3d9_hook.h"
#include "gfx/th_info_overlay.h"
#include "patch/th_patch_registry.h"

/*
* TODO
*
* X - BUG: Bullets that come directly will not be dodged (fixed?)
* X - Bullet sizes contribute to action range (fixed?)
*
* - Handle focusing better
* - Lasers
* - Reimplement shrinking radius system
* - Implement simulated annealing tuning of parameters
* - Decrease focus dodge force (ie. focus more often)
* - Implement player velocity determination/calibration
* - Enemy detection and targeting (partially implemented through boss detection)
*/

void th08_player::onInit()
{
	th_player::onInit();
}

void th08_player::onTick()
{
	th_player::onTick();
}

void th08_player::onBeginTick()
{
	th_player::onBeginTick();
}

void th08_player::onAfterTick()
{
	th_player::onAfterTick();
}

void th08_player::draw(IDirect3DDevice9* d3dDev)
{
	th_player::draw(d3dDev);
}

void th08_player::handleInput(const BYTE diKeys[256], const BYTE press[256])
{
	th_player::handleInput(diKeys, press);
}

void th08_player::onEnableChanged(bool enable)
{
	th_player::onEnableChanged(enable);

	if (enable)
	{
		th_registry::patch("th08_autobomb");
	}
	else
	{
		th_registry::unpatch("th08_autobomb");
	}
}

