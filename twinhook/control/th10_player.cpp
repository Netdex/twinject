#include "../stdafx.h"
#include "th10_player.h"
#include "../directx/IDI8ADevice_Wrapper.h"
#include "../config/th_config.h"
#include "../hook/th_di8_hook.h"
#include "../util/cdraw.h"
#include "../gfx/di8_input_overlay.h"
#include "../algo/th_algorithm.h"
#include "../hook/th_d3d9_hook.h"


void th10_player::on_init()
{
	LOG("th10 player initialized");


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
		ASSERT((false, "fatal, d3ddev9_wrapper inaccessible"));
	}
}

void th10_player::on_tick()
{
	// TODO this code should be generalized since it's the same in every bot
	th_di8_hook* di8 = th_di8_hook::inst();
	BYTE diKeys[256];
	if (di8->DirectInput8) {
		if (di8->DirectInput8->DirectInputDevice8->GetDeviceState(256, diKeys) == DI_OK)
		{
			BYTE press[256];
			kpd.tick(diKeys, press);
			this->handle_input(diKeys, press);
		}
	}
	if (algorithm)
		algorithm->on_tick();
}

void th10_player::on_begin_tick()
{

}

void th10_player::on_after_tick()
{
	bullets.clear();
	powerups.clear();
	lasers.clear();
}

// TODO this text rendering code should be generalized since it exists in every bot
#define D BotOverlayRenderer_DisplayDebugString
#define B BotOverlayRenderer_BeginDebugString

static char BotOverlayRenderer_StringBuffer[256];
static int BotOverlayRenderer_DebugLineOffset = 0;

static void BotOverlayRenderer_BeginDebugString()
{
	cdraw::fill_rect(th_param.WINDOW_WIDTH - (650 - 445), th_param.WINDOW_HEIGHT - (480 - 250),
		th_param.WINDOW_WIDTH, th_param.WINDOW_HEIGHT, D3DCOLOR_ARGB(200, 0, 0, 0));
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
		(int)th_param.WINDOW_WIDTH - (640 - 450), (int)th_param.WINDOW_HEIGHT - (480 - 255) + 15 * BotOverlayRenderer_DebugLineOffset,
		(int)th_param.WINDOW_WIDTH, (int)th_param.WINDOW_HEIGHT);
	va_end(args);
	BotOverlayRenderer_DebugLineOffset++;
}

void th10_player::draw(IDirect3DDevice9* d3dDev)
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

void th10_player::handle_input(const BYTE diKeys[256], const BYTE press[256])
{
	if (diKeys[DIK_G])
		set_enable(true);
	if (diKeys[DIK_B])
		set_enable(false);
	if (diKeys[DIK_H])
		render = true;
	if (diKeys[DIK_N])
		render = false;

	algorithm->handle_input(diKeys, press);
}

void th10_player::on_enable_changed(bool enable)
{
	// this is cheating
	/*th15_patch_autobomb ptch;
	if (enable)
	{
		algorithm->on_begin();
		ptch.patch();
	}
	else
	{
		ptch.unpatch();
	}*/
}

entity th10_player::get_plyr_ent()
{
	// use static pointer to game object to get player offset
	PBYTE *PlayerPtrAddr = (PBYTE*)gs_ptr.plyr_pos;
	if (*PlayerPtrAddr) {
		PBYTE plyrAddr = *PlayerPtrAddr;

		entity e = {
			vec2(
				*(float*)(plyrAddr + 0x3C0),
				*(float*)(plyrAddr + 0x3C4)),
			vec2(),
			vec2(5,5),		// TODO assume 5x5 size
			0
		};
		return e;
	}
	return {};
}



