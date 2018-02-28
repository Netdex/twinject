#include "stdafx.h"
#include "th_d3d9_hook.h"
#include "ID3D9_Wrapper.h"
#include "cdraw.h"
#include "detour.h"

th_d3d9_hook* th_d3d9_hook::instance = nullptr;

void th_d3d9_hook::bind(th_player* player)
{
	assert(("cannot multi-bind", !instance));
	instance = new th_d3d9_hook(player);

	Direct3D9Hook hook;
	hook.CreateDeviceHook = d3d9_init_hook;
	hook.BeginSceneHook = d3d9_begin_hook;
	hook.EndSceneHook = d3d9_end_hook;
	Hook_Kernel32_LoadLibraryA(hook);
}

th_d3d9_hook* th_d3d9_hook::inst()
{
	assert(("cannot obtain unbounded instance", instance));
	return instance;
}

void th_d3d9_hook::d3d9_init_hook(IDirect3DDevice9 *d3dDev)
{
	cdraw::init(d3dDev);
	inst()->player->on_init();
}

void th_d3d9_hook::d3d9_begin_hook(IDirect3DDevice9 *d3dDev)
{
	inst()->player->on_begin_tick();
}

void th_d3d9_hook::d3d9_end_hook(IDirect3DDevice9 *d3dDev)
{
	inst()->player->on_tick();
	inst()->player->draw(d3dDev);
	inst()->player->on_after_tick();
}

static Direct3DCreate9_t Direct3DCreate9_Original = Direct3DCreate9;

static Direct3D9Hook d3d9_hook;
static bool d3d9_created = false;

static IDirect3D9* WINAPI Direct3DCreate9_Hook(UINT sdkVers)
{
	LOG("D3D9: Feeding fake IDirect3D9");
	IDirect3D9 *legit = Direct3DCreate9_Original(sdkVers);
	Direct3D9Wrapper *Direct3D9 = new Direct3D9Wrapper(legit, d3d9_hook);
	d3d9_created = true;
	return Direct3D9;
}

static void Hook_D3D9_Direct3DCreate9(Direct3D9Hook hook)
{
	assert(("d3d9 already created", !d3d9_created));

	if (DetourFunction(&(PVOID&)Direct3DCreate9_Original, Direct3DCreate9_Hook))
		LOG("Detours: Hooked Direct3DCreate9");
	else
		LOG("Detours: Failed to hook Direct3DCreate9");
}

static bool HookedD3D = false;

static LoadLibrary_t LoadLibraryA_Original;
static HMODULE WINAPI LoadLibraryA_Hook(LPCSTR lpFileName)
{
	HMODULE hM = LoadLibraryA_Original(lpFileName);
	// logfs("LoadLibrary: %s" , lpFileName);
	if (!HookedD3D && strcmp(lpFileName, "d3d9.dll") == 0)
	{
		HookedD3D = true;
		PBYTE pDirect3DCreate9 = (PBYTE)GetProcAddress(hM, "Direct3DCreate9");
		LOG("D3D9: Found Direct3DCreate9 at addr %d", (int)pDirect3DCreate9);
		Direct3DCreate9_Original = (Direct3DCreate9_t)pDirect3DCreate9;
		Hook_D3D9_Direct3DCreate9(d3d9_hook);
	}
	return hM;
}

static void Hook_Kernel32_LoadLibraryA(Direct3D9Hook hook)
{
	assert(("d3d9 already loaded", !HookedD3D));

	d3d9_hook = hook;
	LoadLibraryA_Original = LoadLibraryA;

	if (DetourFunction(&(PVOID&)LoadLibraryA_Original, LoadLibraryA_Hook))
		LOG("Detours: Hooked LoadLibraryA");
	else
		LOG("Detours: Failed to hook LoadLibraryA");
}
