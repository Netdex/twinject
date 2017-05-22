#pragma once
#include "stdafx.h"

struct D3DTLVERTEX
{
	float x, y, z, rhw;
	DWORD color;
};

HRESULT GenerateTexture(IDirect3DDevice9 *pD3Ddev, IDirect3DTexture9 **ppD3Dtex, DWORD colour32);
void DrawRect(IDirect3DDevice9* m_pD3Ddev, float x, float y, float w, float h, D3DCOLOR Color);

void CBeginScene(LPDIRECT3DDEVICE9 m_pD3Ddev);
void CEndScene(LPDIRECT3DDEVICE9 m_pD3Ddev);