#pragma once

#include <d3d9.h>

#include "pattern/nocopy.h"

class imgui_controller : nocopy
{
	HWND hWnd = NULL;
	IDirect3DDevice9 *pD3DDev = NULL;

	IDirect3DStateBlock9 *d3d9_state_block = NULL;
	IDirect3DStateBlock9 *d3d9_sb_preserve = NULL;
	
public:
	imgui_controller(HWND hWnd, IDirect3DDevice9 *pD3DDev) : hWnd(hWnd), pD3DDev(pD3DDev){}
	~imgui_controller()
	{
		if (d3d9_state_block)
			d3d9_state_block->Release();
		if (d3d9_sb_preserve)
			d3d9_sb_preserve->Release();
	}

	void init();
	void preframe();
	void render();
};
