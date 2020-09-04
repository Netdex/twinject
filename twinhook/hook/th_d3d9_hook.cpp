#include "../stdafx.h"
#include "th_d3d9_hook.h"
#include "../directx/ID3D9_Wrapper.h"
#include "../util/cdraw.h"
#include "../util/detour.h"
#include "config/th_config.h"

th_d3d9_hook* th_d3d9_hook::instance = nullptr;
static Direct3D9Hook d3d9_hook;

void th_d3d9_hook::bind(th_player* player, bool twoStage)
{
	CHECK(!instance);
	instance = new th_d3d9_hook(player);

	Direct3D9Hook hook;
	hook.CreateDeviceHook = d3d9InitHook;
	hook.BeginSceneHook = d3d9BeginHook;
	hook.EndSceneHook = d3d9EndHook;
	d3d9_hook = hook;

	if (twoStage) {
		Hook_Kernel32_LoadLibraryA();
	}
	else
	{
		Hook_D3D9_Direct3DCreate9();
	}
}

th_d3d9_hook* th_d3d9_hook::inst()
{
	CHECK(instance);
	return instance;
}

void th_d3d9_hook::d3d9InitHook(IDirect3DDevice9* d3dDev)
{
	CHECK(th_d3d9_hook::inst()->d3ddev9_wrapper);
	D3DDEVICE_CREATION_PARAMETERS cparams;
	RECT rect;
	inst()->d3ddev9_wrapper->GetCreationParameters(&cparams);
	GetClientRect(cparams.hFocusWindow, &rect);
	th_param.WINDOW_WIDTH = static_cast<float>(rect.right);
	th_param.WINDOW_HEIGHT = static_cast<float>(rect.bottom);
	SPDLOG_INFO("Detected window dimensions {}x{}", rect.right, rect.bottom);
	cdraw::init(d3dDev, rect);
	inst()->player->onInit();
}

void th_d3d9_hook::d3d9BeginHook(IDirect3DDevice9* d3dDev)
{
	inst()->player->onBeginTick();
}

void th_d3d9_hook::d3d9EndHook(IDirect3DDevice9* d3dDev)
{
	inst()->player->onTick();
	cdraw::begin();
	inst()->player->draw(d3dDev);
	cdraw::end();
	inst()->player->onAfterTick();
}

static Direct3DCreate9_t Direct3DCreate9_Original = Direct3DCreate9;
static bool d3d9_created = false;

static IDirect3D9* WINAPI Direct3DCreate9_Hook(UINT sdkVers)
{
	IDirect3D9* legit = Direct3DCreate9_Original(sdkVers);
	if (!d3d9_created) {
		SPDLOG_INFO("D3D9: Feeding fake IDirect3D9");
		Direct3D9Wrapper* Direct3D9 = new Direct3D9Wrapper(legit, d3d9_hook);
		d3d9_created = true;
		return Direct3D9;
	}
	return legit;
}

static void Hook_D3D9_Direct3DCreate9()
{
	CHECK(!d3d9_created);

	if (DetourFunction(&(PVOID&)Direct3DCreate9_Original, Direct3DCreate9_Hook))
		SPDLOG_INFO("Detours: Hooked Direct3DCreate9");
	else
		SPDLOG_ERROR("Detours: Failed to hook Direct3DCreate9");
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
		SPDLOG_INFO("D3D9: Found Direct3DCreate9 at addr {}", (int)pDirect3DCreate9);
		Direct3DCreate9_Original = (Direct3DCreate9_t)pDirect3DCreate9;
		Hook_D3D9_Direct3DCreate9();
	}
	return hM;
}

static void Hook_Kernel32_LoadLibraryA()
{
	CHECK(("d3d9 already loaded", !HookedD3D));

	LoadLibraryA_Original = LoadLibraryA;

	if (DetourFunction(&(PVOID&)LoadLibraryA_Original, LoadLibraryA_Hook))
		SPDLOG_INFO("Detours: Hooked LoadLibraryA");
	else
		SPDLOG_INFO("Detours: Failed to hook LoadLibraryA");
}
