#include "stdafx.h"
#include "th08_player.h"
#include "IDI8ADevice_Wrapper.h"
#include "th08_patch_autobomb.h"
#include "th_config.h"
#include "th_di8_hook.h"
#include "cdraw.h"
#include "di8_input_overlay.h"
#include "th_algorithm.h"

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
}

void th08_player::on_tick()
{
	th_di8_hook* di8 = th_di8_hook::inst();
	BYTE diKeys[256];
	if (di8->DirectInput8) {
		if (di8->DirectInput8->DirectInputDevice8->GetDeviceState(256, diKeys) == DI_OK)
		{
			this->handle_input(diKeys);
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
		(int) th_param::WINDOW_WIDTH, (int) th_param::WINDOW_HEIGHT);
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

	DI8_Overlay_RenderInput(d3dDev);
}

void th08_player::handle_input(BYTE diKeys[256])
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

static PBYTE PlayerPosAddr = (PBYTE)0x017D6110;
static PBYTE BossPosAddr = (PBYTE)0x004CE7EC;

vec2 th08_player::get_plyr_loc()
{
	return vec2(*(float*)PlayerPosAddr - th_param::GAME_X_OFFSET, *(float*)(PlayerPosAddr + 4) - th_param::GAME_Y_OFFSET);
}

vec2 th08_player::get_boss_loc()
{
	vec2 a(*(float*)BossPosAddr - th_param::GAME_X_OFFSET, *(float*)(BossPosAddr + 4) - th_param::GAME_Y_OFFSET);
	if (a.x < 0 || a.y < 0) return vec2(NAN, NAN);
	return a;
}
