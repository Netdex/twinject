#pragma once
#include "stdafx.h"
#define _USE_MATH_DEFINES

namespace cdraw
{
	struct D3DTLVERTEX
	{
		float x, y, z, rhw;
		DWORD color;
	};

	void init(IDirect3DDevice9 *d3dDev, RECT destRect);
	void begin();
	void end();

	void fillRect(float x, float y, float w, float h, D3DCOLOR Color);
	void rect(float x, float y, float w, float h, D3DCOLOR Color);

	void text(char *str, D3DCOLOR color, int x, int y);
	void text(char *str, int len, D3DCOLOR color, int x, int y);
	void text(char *str, D3DCOLOR color, int x, int y, LPD3DXSPRITE sprite);
	void text(char *str, int len, D3DCOLOR color, int x, int y, LPD3DXSPRITE sprite);

	void line(float x1, float y1, float x2, float y2, D3DCOLOR color);
	void circle(float x, float y, float radius, int sides, DWORD color);
}
