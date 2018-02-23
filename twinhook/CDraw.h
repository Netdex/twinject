#pragma once
#include "stdafx.h"
#define _USE_MATH_DEFINES

struct D3DTLVERTEX
{
	float x, y, z, rhw;
	DWORD color;
};

void CDraw_Init(IDirect3DDevice9 *d3dDev);


void CDraw_FillRect(float x, float y, float w, float h, D3DCOLOR Color);
void CDraw_Rect(float x, float y, float w, float h, D3DCOLOR Color);
void CDraw_Text(char *str, D3DCOLOR color, int x, int y, int w, int h);
void CDraw_Line(float x1, float y1, float x2, float y2, D3DCOLOR color);
void CDraw_Circle(float x, float y, float radius, int sides, DWORD color);

