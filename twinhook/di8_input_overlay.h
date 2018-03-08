#pragma once

/**
 * \brief Information regarding key press visualization
 */
struct key_graphic
{
	BYTE vkCode;
	float x;
	float y;
	float w;
	float h;
	D3DCOLOR active_color;
	D3DCOLOR default_color;
};

/* key visualization positional information */
const float InputOverlayUnitSize = 15.f;

/* key visualization display information */
const key_graphic kg_MonitoredKeys[] = {
	{
		DIK_LEFT,
		0.f, 1.f, 1.f, 1.f,
		D3DCOLOR_ARGB(255, 0, 255, 0),
		D3DCOLOR_ARGB(40, 255, 255, 255)
	},
	{
		DIK_RIGHT,
		2.f, 1.f, 1.f, 1.f,
		D3DCOLOR_ARGB(255, 0, 255, 0),
		D3DCOLOR_ARGB(40, 255, 255, 255)
	},
	{
		DIK_UP,
		1.f, 0.f, 1.f, 1.f,
		D3DCOLOR_ARGB(255, 0, 255, 0),
		D3DCOLOR_ARGB(40, 255, 255, 255)
	},
	{
		DIK_DOWN,
		1.f, 1.f, 1.f, 1.f,
		D3DCOLOR_ARGB(255, 0, 255, 0),
		D3DCOLOR_ARGB(40, 255, 255, 255)
	},
	{
		DIK_Z,
		4.f, 0.f, 1.f, 1.f,
		D3DCOLOR_ARGB(255, 0, 255, 0),
		D3DCOLOR_ARGB(40, 255, 255, 255)
	},
	{
		DIK_X,
		4.f, 1.f, 1.f, 1.f,
		D3DCOLOR_ARGB(255, 0, 255, 0),
		D3DCOLOR_ARGB(40, 255, 255, 255)
	},
	{
		DIK_LSHIFT,
		5.f, 1.f, 1.f, 1.f,
		D3DCOLOR_ARGB(255, 0, 255, 0),
		D3DCOLOR_ARGB(40, 255, 255, 255)
	},
};

void DI8_Overlay_RenderInput(IDirect3DDevice9 *d3dDev);