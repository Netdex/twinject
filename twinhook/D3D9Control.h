#pragma once

void D3D9_Init_Hook(IDirect3DDevice9 *d3dDev);
void D3D9_BeginScene_Hook(IDirect3DDevice9 *d3dDev);
void D3D9_EndScene_Hook(IDirect3DDevice9 *d3dDev);