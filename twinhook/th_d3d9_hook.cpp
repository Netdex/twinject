#include "stdafx.h"
#include "th_d3d9_hook.h"
#include "LoadLibraryAHook.h"
#include "ID3D9_Wrapper.h"
#include "cdraw.h"
#include "IDI8A_Wrapper.h"
#include "IDI8ADevice_Wrapper.h"


extern DirectInput8Wrapper *DirectInput8;

static th_player *player_context = nullptr;

static void d3d9_init_hook(IDirect3DDevice9 *d3dDev)
{
	assert(("no th_d3d9_hook has been hooked", player_context));

	CDraw_Init(d3dDev);
	player_context->init();
}

static void d3d9_begin_hook(IDirect3DDevice9 *d3dDev)
{
	assert(("no th_d3d9_hook has been hooked", player_context));

	player_context->before_tick();
}

static void d3d9_end_hook(IDirect3DDevice9 *d3dDev)
{
	assert(("no th_d3d9_hook has been hooked", player_context));

	BYTE diKeys[256];

	player_context->draw(d3dDev);
	if (DirectInput8) {
		if (DirectInput8->DirectInputDevice8->GetDeviceState(256, diKeys) == DI_OK)
		{
			player_context->handle_input(diKeys);
		}
		player_context->tick();
	}
}

th_d3d9_hook::th_d3d9_hook(th_player& player) : player(player)
{
	
}

void th_d3d9_hook::hook()
{
	player_context = &this->player;

	Direct3D9Hook hook;
	hook.CreateDeviceHook = d3d9_init_hook;
	hook.BeginSceneHook = d3d9_begin_hook;
	hook.EndSceneHook = d3d9_end_hook;
	Hook_Kernel32_LoadLibraryA(hook);
}

