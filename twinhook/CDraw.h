#pragma once
#include "stdafx.h"
#define _USE_MATH_DEFINES
#include <math.h>

struct D3DTLVERTEX
{
	float x, y, z, rhw;
	DWORD color;
};

HRESULT CDraw_GenerateTexture(IDirect3DDevice9 *pD3Ddev, IDirect3DTexture9 **ppD3Dtex, DWORD colour32);
void CDraw_FillRect(IDirect3DDevice9* m_pD3Ddev, float x, float y, float w, float h, D3DCOLOR Color);
void CDraw_Rect(float x, float y, float w, float h, D3DCOLOR Color);
void CDraw_Text(char *str, D3DCOLOR color, int x, int y, int w, int h);
void CDraw_Line(float x1, float y1, float x2, float y2, D3DCOLOR color);
void CDraw_Circle(float x, float y, float radius, int sides, DWORD color);

void CDraw_InitSolidTexture(LPDIRECT3DDEVICE9 m_pD3Ddev);
void CDraw_InitFont(IDirect3DDevice9 *m_pD3Ddev, int sz, LPWSTR face);
void CDraw_InitLine(IDirect3DDevice9 *m_pD3Ddev);
