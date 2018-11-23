#include "stdafx.h"

#include <imgui/imgui.h>

#include "th_player.h"
#include "config/th_config.h"
#include "directx/IDI8ADevice_Wrapper.h"

#include "hook/th_di8_hook.h"
#include "hook/th_d3d9_hook.h"

#include "gfx/di8_input_overlay.h"
#include "gfx/imgui_window.h"

void th_player::onInit()
{
	ASSERT(("Could not initialize IMGUI window", imgui_window_init()));
}

void th_player::onBeginTick()
{
	imgui_window_preframe();
}

void th_player::onTick()
{
	th_di8_hook* di8 = th_di8_hook::inst();
	if (!di8->DirectInput8)
		return;

	BYTE diKeys[256];
	if (di8->DirectInput8->DirectInputDevice8->DirectInputDevice8->GetDeviceState(256, diKeys) == DI_OK)
	{
		BYTE press[256];
		kpd.tick(diKeys, press);
		this->handleInput(diKeys, press);
	}

	if (algorithm)
		algorithm->onTick();
}

void th_player::onAfterTick()
{
	bullets.clear();
	enemies.clear();
	powerups.clear();
	lasers.clear();

	imgui_window_render();
}

void th_player::draw(IDirect3DDevice9* d3dDev)
{
	if (algorithm)
		algorithm->visualize(d3dDev);
	cmdp.render(d3dDev);
	DI8_Overlay_RenderInput(d3dDev, this->getKeyboardState());

	/* IMGUI Integration*/

	using namespace ImGui;
	Begin("twinject (netdex)");
	Text("b e p l #: %d %d %d %d", bullets.size(), enemies.size(), powerups.size(), lasers.size());
	Text("bot state: %s", enabled ? "ENABLED" : "DISABLED");
	Text("viz state: %s", render ? "DETAILED" : "NONE");

	if (Button("Toggle Bot"))
		setEnable(!enabled);
	SameLine();
	if (Button("Toggle Debug"))
		render = !render;
	Checkbox("Show IMGUI demo", &imguiShowDemoWindow);
	End();

	if (imguiShowDemoWindow)	ShowDemoWindow();
}

void th_player::handleInput(const BYTE diKeys[256], const BYTE press[256])
{
	if (press[DIK_G])
		setEnable(!enabled);
	if (press[DIK_H])
		render = !render;

	algorithm->handleInput(diKeys, press);
}

void th_player::onEnableChanged(bool enable)
{
	if (enable) {
		algorithm->onBegin();
	}
}

void th_player::setEnable(bool enable)
{
	// debounce
	if (enable != enabled)
		onEnableChanged(enable);
	enabled = enable;
}

void th_player::bindAlgorithm(th_algorithm* algo)
{
	algorithm = algo;
}

//player th_player::getPlayerEntity()
//{
//	// TODO this must be overridden depending on the game's hit type!
//	PBYTE PlayerPtrAddr = (PBYTE)this->gs_ptr.plyr_pos;
//	
//	entity plyr = {
//		vec2(*(float*)PlayerPtrAddr - th_param.GAME_X_OFFSET,
//			 *(float*)(PlayerPtrAddr + 4) - th_param.GAME_Y_OFFSET),
//		vec2(),
//		vec2(5, 5), // hard-coded player size
//		0
//	};
//	return plyr;
//}

th_kbd_state th_player::getKeyboardState() const
{
	uint8_t* ptr_gkbd_st = gs_ptr.kbd_state;
	th_kbd_state s = {
		(bool)(ptr_gkbd_st[0] & 1),
		(bool)(ptr_gkbd_st[0] & 2),
		(bool)(ptr_gkbd_st[0] & 8),
		(bool)(ptr_gkbd_st[1] & 2),
		(bool)(ptr_gkbd_st[0] & 16),
		(bool)(ptr_gkbd_st[0] & 64),
		(bool)(ptr_gkbd_st[0] & 32),
		(bool)(ptr_gkbd_st[0] & 128),
	};
	return s;
}
