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

void th08_player::on_init()
{
	LOG("th08 player initialized");

	if (th_d3d9_hook::inst()->d3ddev9_wrapper) {
		D3DDEVICE_CREATION_PARAMETERS cparams;
		RECT rect;
		th_d3d9_hook::inst()->d3ddev9_wrapper->GetCreationParameters(&cparams);
		GetClientRect(cparams.hFocusWindow, &rect);
		th_param.WINDOW_WIDTH = (float)rect.right;
		th_param.WINDOW_HEIGHT = (float)rect.bottom;
		LOG("detected window dimensions %ld %ld", rect.right, rect.bottom);
	}
	else
	{
		ASSERT("fatal, d3ddev9_wrapper inaccessible");
	}
}

void th08_player::on_tick()
{
	th_di8_hook* di8 = th_di8_hook::inst();
	BYTE diKeys[256];
	if (di8->DirectInput8) {
		if (di8->DirectInput8->DirectInputDevice8->GetDeviceState(256, diKeys) == DI_OK)
		{
			this->handle_input(diKeys, NULL);
		}
	}
	if (algorithm)
		algorithm->on_tick();
}

void th08_player::on_begin_tick()
{

}

void th08_player::on_after_tick()
{
	bullets.clear();
	powerups.clear();
}

#define D BotOverlayRenderer_DisplayDebugString
#define B BotOverlayRenderer_BeginDebugString

static char BotOverlayRenderer_StringBuffer[256];
static int BotOverlayRenderer_DebugLineOffset = 0;

static void BotOverlayRenderer_BeginDebugString()
{
	cdraw::fill_rect(445, 250, 640, 480, D3DCOLOR_ARGB(200, 0, 0, 0));
	BotOverlayRenderer_DebugLineOffset = 0;
}

// BUG this code actually uses a ridiculous amount of computing power
// render text to a double-buffer then blit onto screen instead
// or use ID3DXSprite
static void BotOverlayRenderer_DisplayDebugString(D3DCOLOR color, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vsprintf_s(BotOverlayRenderer_StringBuffer, 256, fmt, args);
	cdraw::text(BotOverlayRenderer_StringBuffer, color,
		450, 255 + 15 * BotOverlayRenderer_DebugLineOffset, 
		(int) th_param.WINDOW_WIDTH, (int) th_param.WINDOW_HEIGHT);
	va_end(args);
	BotOverlayRenderer_DebugLineOffset++;
}

void th08_player::draw(IDirect3DDevice9* d3dDev)
{
	B();
	D(D3DCOLOR_ARGB(255, 0, 255, 255), "TWINJECT [netdex]");
	D(D3DCOLOR_ARGB(255, 255, 255, 255), "BULLET_COUNT: %d", bullets.size());
	D(D3DCOLOR_ARGB(255, 255, 255, 255), "   BOT_STATE: %s", enabled ? "ENABLED" : "DISABLED");
	D(D3DCOLOR_ARGB(255, 255, 255, 255), "         VIZ: %s", render ? "DETAILED" : "NONE");

	if (algorithm)
		algorithm->visualize(d3dDev);

	DI8_Overlay_RenderInput(d3dDev, this->get_kbd_state());
}

void th08_player::handle_input(const BYTE diKeys[256], const BYTE press[256])
{
	if (diKeys[DIK_G])
		set_enable(true);
	if (diKeys[DIK_B])
		set_enable(false);
	if (diKeys[DIK_H])
		render = true;
	if (diKeys[DIK_N])
		render = false;
}

void th08_player::on_enable_changed(bool enable)
{
	th08_patch_autobomb ptch;
	if (enable)
	{
		algorithm->on_begin();
		ptch.patch();
	}
	else
	{
		ptch.unpatch();
	}
}

