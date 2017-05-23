#pragma once
#include "stdafx.h"

struct D3DTLVERTEX
{
	float x, y, z, rhw;
	DWORD color;
};

HRESULT CDraw_GenerateTexture(IDirect3DDevice9 *pD3Ddev, IDirect3DTexture9 **ppD3Dtex, DWORD colour32);
void CDraw_DrawRect(IDirect3DDevice9* m_pD3Ddev, float x, float y, float w, float h, D3DCOLOR Color);
void CDraw_Text(char *str, D3DCOLOR color, int x, int y, int w, int h);

void CDraw_InitSolidTexture(LPDIRECT3DDEVICE9 m_pD3Ddev);
void CDraw_InitFont(IDirect3DDevice9 *m_pD3Ddev, int sz, LPWSTR face);
