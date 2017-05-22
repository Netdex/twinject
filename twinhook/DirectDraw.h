#pragma once
#include <d3d9.h>
#include <d3dx9core.h>

class CDirectDraw
{
private:
	IDirect3DDevice9* m_pDevice;
	ID3DXFont* m_pFont;
	ID3DXSprite* m_pSprite;

public:
	CDirectDraw(IDirect3DDevice9* pDevice);
	~CDirectDraw(void);

public:
	HRESULT DrawText(LPCWSTR lpszText, int nSize, int x, int y, D3DCOLOR color);
};
