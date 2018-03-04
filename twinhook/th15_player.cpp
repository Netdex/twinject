#include "stdafx.h"
#include "th15_player.h"
#include "IDI8ADevice_Wrapper.h"
#include "th_config.h"
#include "th_di8_hook.h"
#include "cdraw.h"
#include "di8_input_overlay.h"
#include "th_algorithm.h"


void th15_player::on_init()
{
	LOG("th15 player initialized");
}

void th15_player::on_tick()
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

void th15_player::on_begin_tick()
{

}

void th15_player::on_after_tick()
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
		(int)th_param::WINDOW_WIDTH, (int)th_param::WINDOW_HEIGHT);
	va_end(args);
	BotOverlayRenderer_DebugLineOffset++;
}

void th15_player::draw(IDirect3DDevice9* d3dDev)
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

void th15_player::handle_input(BYTE diKeys[256])
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

void th15_player::on_enable_changed(bool enable)
{
	if (enable)
	{
		algorithm->on_begin();
	}
}

static PBYTE *PlayerPtrAddr = (PBYTE*)0x004E9BB8;

entity th15_player::get_plyr_cz()
{
	if (*PlayerPtrAddr) {
		PBYTE plyrAddr = *PlayerPtrAddr;

		// we must check if size is readable, because it doesn't initialize immediately
		vec2 size;
		if(*(DWORD*)(plyrAddr + 0x2C008))
		{
			size = vec2(
				*(float*)(*(DWORD*)(plyrAddr + 0x2C008) + 4),
				*(float*)(*(DWORD*)(plyrAddr + 0x2C008) + 4));
		}

		entity e = {
			vec2(
				*(float*)(plyrAddr + 0x618) + th_param::GAME_WIDTH / 2,
				*(float*)(plyrAddr + 0x61C)),
			vec2(),
			size,
			0
		};
		return e;
	}
	return {};
}



