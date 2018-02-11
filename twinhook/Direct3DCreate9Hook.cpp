#include "stdafx.h"
#include "Direct3DCreate9Hook.h"
#include "detour.h"
#include "ID3D9_Wrapper.h"
#include "D3D9Control.h"

Direct3DCreate9_t Direct3DCreate9_Original = Direct3DCreate9;

static Direct3D9Hook d3d9_hook;

IDirect3D9* WINAPI Direct3DCreate9_Hook(UINT sdkVers)
{
	logfs("D3D9: Feeding fake IDirect3D9");
	IDirect3D9 *legit = Direct3DCreate9_Original(sdkVers);
	Direct3D9Wrapper *Direct3D9 = new Direct3D9Wrapper(legit, d3d9_hook);
	return Direct3D9;
}
void Hook_D3D9_Direct3DCreate9(Direct3D9Hook hook)
{
	d3d9_hook = hook;
	if (DetourFunction(&(PVOID&)Direct3DCreate9_Original, Direct3DCreate9_Hook))
		logfs("Detours: Hooked Direct3DCreate9");
	else
		logfs("Detours: Failed to hook Direct3DCreate9");
}