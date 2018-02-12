#pragma once
#include "th_hook.h"
#include "th_player.h"

class th_di8_hook;

struct Direct3D9Hook;

IDirect3D9* WINAPI Direct3DCreate9_Hook(UINT sdkVers);
typedef IDirect3D9* (WINAPI *Direct3DCreate9_t)(UINT SDKVersion);
void Hook_D3D9_Direct3DCreate9(Direct3D9Hook hook);

struct Direct3D9Hook;

HMODULE WINAPI LoadLibraryA_Hook(LPCSTR lpFileName);
typedef HMODULE(WINAPI *LoadLibrary_t)(LPCSTR);
void Hook_Kernel32_LoadLibraryA(Direct3D9Hook hok);

/**
 * \brief		func hook for d3d9
 * \warning		only for d3d8 -> d3d9 patch
 */
class th_d3d9_hook : th_hook
{
public:
	explicit th_d3d9_hook(th_player *player) : th_hook(player) {}
	virtual ~th_d3d9_hook() = default;

	void hook() override;

	static void d3d9_init_hook(IDirect3DDevice9 *d3dDev);
	static void d3d9_begin_hook(IDirect3DDevice9 *d3dDev);
	static void d3d9_end_hook(IDirect3DDevice9 *d3dDev);
};
