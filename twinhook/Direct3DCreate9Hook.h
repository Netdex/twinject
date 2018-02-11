#pragma once

struct Direct3D9Hook;

IDirect3D9* WINAPI Direct3DCreate9_Hook(UINT sdkVers);
typedef IDirect3D9* (WINAPI *Direct3DCreate9_t)(UINT SDKVersion);
void Hook_D3D9_Direct3DCreate9(Direct3D9Hook hook);