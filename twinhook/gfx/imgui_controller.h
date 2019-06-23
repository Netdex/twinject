#pragma once

#include <d3d9.h>

class imgui_controller
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

	imgui_controller(const imgui_controller& other) = delete;

	imgui_controller(imgui_controller&& other) noexcept
		: hWnd{other.hWnd},
		  pD3DDev{other.pD3DDev},
		  d3d9_state_block{other.d3d9_state_block},
		  d3d9_sb_preserve{other.d3d9_sb_preserve}
	{
	}

	imgui_controller& operator=(imgui_controller&& other) noexcept
	{
		using std::swap;
		swap(*this, other);
		return *this;
	}

	void init();
	void preframe();
	void render();
};
