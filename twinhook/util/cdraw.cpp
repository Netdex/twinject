#include "stdafx.h"
#include "cdraw.h"
#include <math.h>

namespace cdraw
{
	static IDirect3DDevice9 *CDrawDefaultD3DDevice = NULL;
	static IDirect3DTexture9* CDrawDefaultPrimitive = NULL;
	static LPD3DXFONT CDrawDefaultDxFont;
	static LPD3DXLINE CDrawDefaultDxLine;
	static LPD3DXSPRITE CDrawTextSprite;

	static bool CDrawFlagInit = false;

	void init_solid_texture(LPDIRECT3DDEVICE9 m_pD3Ddev);
	void init_font(IDirect3DDevice9 *m_pD3Ddev, int sz, LPWSTR face);
	void init_line(IDirect3DDevice9 *m_pD3Ddev);

	void init(IDirect3DDevice9 *d3dDev)
	{
		if (CDrawFlagInit) return;
		CDrawFlagInit = true;

		CDrawDefaultD3DDevice = d3dDev;
		init_solid_texture(d3dDev);
		init_font(d3dDev, 14, L"Consolas");
		init_line(d3dDev);

		LOG("CDraw: initialized graphics primitives");
	}

	void fill_rect(float x, float y, float w, float h, D3DCOLOR Color)
	{
		if (!CDrawFlagInit) return;

		D3DTLVERTEX qV[4] = {
			{ (float)x , (float)(y + h), 0.0f, 1.0f, Color },
		{ (float)x , (float)y , 0.0f, 1.0f, Color },
		{ (float)(x + w), (float)(y + h), 0.0f, 1.0f, Color },
		{ (float)(x + w), (float)y , 0.0f, 1.0f, Color }
		};
		const DWORD D3DFVF_TL = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
		CDrawDefaultD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		CDrawDefaultD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		CDrawDefaultD3DDevice->SetFVF(D3DFVF_TL);
		CDrawDefaultD3DDevice->SetTexture(0, CDrawDefaultPrimitive);
		CDrawDefaultD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, qV, sizeof(D3DTLVERTEX));
	}

	void rect(float x, float y, float w, float h, D3DCOLOR c)
	{
		if (!CDrawFlagInit) return;

		line(x, y, x + w, y, c);
		line(x, y + h, x + w, y + h, c);
		line(x, y, x, y + h, c);
		line(x + w, y, x + w, y + h, c);
	}


	void text(char *str, D3DCOLOR color, int x, int y, int w, int h)
	{
		if (!CDrawFlagInit) return;

		static RECT textbox;
		SetRect(&textbox, x, y, w, h);
		if (CDrawDefaultDxFont) {
			CDrawTextSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
			CDrawDefaultDxFont->DrawTextA(CDrawTextSprite, str, -1, &textbox, DT_LEFT | DT_TOP, color);
			CDrawTextSprite->End();
		}
	}

	void line(float x1, float y1, float x2, float y2, D3DCOLOR color)
	{
		if (!CDrawFlagInit) return;

		D3DXVECTOR2 vert[2];
		vert[0].x = x1;
		vert[0].y = y1;
		vert[1].x = x2;
		vert[1].y = y2;
		CDrawDefaultDxLine->Draw(vert, 2, color);
	}

	void circle(float x, float y, float radius, int sides, DWORD color)
	{
		if (!CDrawFlagInit) return;

		sides = std::min(127, sides);
		D3DXVECTOR2 line[128];
		float step = (float)M_PI * 2.0f / sides;
		for (int s = 0; s <= sides; s++)
		{
			float a = step * s;
			float x1 = radius * cos(a) + x;
			float y1 = radius * sin(a) + y;
			line[s].x = x1;
			line[s].y = y1;
		}
		CDrawDefaultDxLine->Begin();
		CDrawDefaultDxLine->Draw(line, sides + 1, color);
		CDrawDefaultDxLine->End();
	}

	HRESULT gen_texture(IDirect3DDevice9 *pD3Ddev, IDirect3DTexture9 **ppD3Dtex, DWORD colour32)
	{
		if (FAILED(pD3Ddev->CreateTexture(8, 8, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, ppD3Dtex, NULL)))
			return E_FAIL;

		WORD colour16 = ((WORD)((colour32 >> 28) & 0xF) << 12)
			| (WORD)(((colour32 >> 20) & 0xF) << 8)
			| (WORD)(((colour32 >> 12) & 0xF) << 4)
			| (WORD)(((colour32 >> 4) & 0xF) << 0);
		D3DLOCKED_RECT d3dlr;
		(*ppD3Dtex)->LockRect(0, &d3dlr, 0, 0);
		WORD *pDst16 = (WORD*)d3dlr.pBits;
		for (int xy = 0; xy < 8 * 8; xy++)
			*pDst16++ = colour16;
		(*ppD3Dtex)->UnlockRect(0);
		return S_OK;
	}

	void init_solid_texture(LPDIRECT3DDEVICE9 m_pD3Ddev)
	{
		gen_texture(m_pD3Ddev, &CDrawDefaultPrimitive, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	void init_font(IDirect3DDevice9 *m_pD3Ddev, int sz, LPWSTR face)
	{
		D3DXCreateFont(m_pD3Ddev, sz, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, NONANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, face, &CDrawDefaultDxFont);
		D3DXCreateSprite(m_pD3Ddev, &CDrawTextSprite);
	}

	void init_line(IDirect3DDevice9 *m_pD3Ddev)
	{
		D3DXCreateLine(m_pD3Ddev, &CDrawDefaultDxLine);
	}
}
