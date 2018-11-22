#include "stdafx.h"
#include "cdraw.h"
#include <math.h>
#include "config/th_config.h"

namespace cdraw
{
	static IDirect3DDevice9 *CDrawDefaultD3DDevice = NULL;
	static IDirect3DTexture9* CDrawDefaultPrimitive = NULL;
	static LPD3DXFONT CDrawDefaultDxFont = NULL;
	static LPD3DXLINE CDrawDefaultDxLine = NULL;
	static LPD3DXSPRITE CDrawTextSprite = NULL;

	static bool CDrawFlagInit = false;
	static RECT CDrawDestRect;
	static float CDrawScaleX = 1.0f;
	static float CDrawScaleY = 1.0f;

	void init_solid_texture(LPDIRECT3DDEVICE9 m_pD3Ddev);
	void init_font(IDirect3DDevice9 *m_pD3Ddev, int sz, LPWSTR face);
	void init_line(IDirect3DDevice9 *m_pD3Ddev);
	
	void init(IDirect3DDevice9 *d3dDev, RECT destRect)
	{
		if (CDrawFlagInit) return;
		CDrawFlagInit = true;

		CDrawDefaultD3DDevice = d3dDev;
		init_solid_texture(d3dDev);
		init_font(d3dDev, 14, L"Consolas");
		init_line(d3dDev);

		CDrawDestRect = destRect;
		CDrawScaleX = destRect.right / th_param.EXPECTED_WINDOW_WIDTH;
		CDrawScaleY = destRect.bottom / th_param.EXPECTED_WINDOW_HEIGHT;

		LOG("CDraw: initialized graphics primitives");
	}

	void begin()
	{
		if (!CDrawFlagInit) return;
		CDrawTextSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
	}

	void end()
	{
		if (!CDrawFlagInit) return;
		CDrawTextSprite->End();
	}

	void fillRect(float x, float y, float w, float h, D3DCOLOR Color)
	{
		if (!CDrawFlagInit) return;

		x *= CDrawScaleX;
		y *= CDrawScaleY;
		w *= CDrawScaleX;
		h *= CDrawScaleY;

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
		x *= CDrawScaleX;
		y *= CDrawScaleY;
		w *= CDrawScaleX;
		h *= CDrawScaleY;
		D3DXVECTOR2 vert[] = { {x, y}, {x, y + h}, {x + w, y + h}, {x + w, y}, {x,y} };
		CDrawDefaultDxLine->Draw(vert, 5, c);
	}

	void text(char *str, D3DCOLOR color, int x, int y)
	{
		text(str, -1, color, x, y, CDrawTextSprite);
	}

	void text(char *str, int len, D3DCOLOR color, int x, int y)
	{
		text(str, len, color, x, y, CDrawTextSprite);
	}

	void text(char* str, D3DCOLOR color, int x, int y, LPD3DXSPRITE sprite)
	{
		text(str, -1, color, x, y, sprite);
	}

	void text(char* str, int len, D3DCOLOR color, int x, int y, LPD3DXSPRITE sprite)
	{
		if (!CDrawFlagInit) return;
		x = (int)(x * CDrawScaleX);
		y *= (int)(y * CDrawScaleY);
		static RECT textbox;
		SetRect(&textbox, x, y, 0, 0);
		if (CDrawDefaultDxFont) {
			CDrawDefaultDxFont->DrawTextA(sprite, str, len, &textbox, DT_LEFT | DT_TOP | DT_NOCLIP, color);
		}
	}

	void line(float x1, float y1, float x2, float y2, D3DCOLOR color)
	{
		if (!CDrawFlagInit) return;
		x1 *= CDrawScaleX;
		y1 *= CDrawScaleY;
		x2 *= CDrawScaleX;
		y2 *= CDrawScaleY;
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
		x *= CDrawScaleX;
		y *= CDrawScaleY;
		radius *= (CDrawScaleX + CDrawScaleY) / 2.0f; 

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
		CDrawDefaultDxLine->Draw(line, sides + 1, color);
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
