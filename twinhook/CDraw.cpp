#include "stdafx.h"
#include "cdraw.h"

IDirect3DTexture9* Primitive = NULL;
LPD3DXFONT dxfont;
LPD3DXLINE dxLine;

bool InitPrimitive = false;
bool InitFont = false;
bool InitLine = false;

HRESULT CDraw_GenerateTexture(IDirect3DDevice9 *pD3Ddev, IDirect3DTexture9 **ppD3Dtex, DWORD colour32)
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

void CDraw_InitSolidTexture(LPDIRECT3DDEVICE9 m_pD3Ddev)
{
	if (!InitPrimitive)
	{
		CDraw_GenerateTexture(m_pD3Ddev, &Primitive,
			D3DCOLOR_ARGB(255, 255, 255, 255)
		);
		InitPrimitive = true;
	}
}
void CDraw_InitFont(IDirect3DDevice9 *m_pD3Ddev, int sz, LPWSTR face)
{
	if (!InitFont) {
		D3DXCreateFont(m_pD3Ddev, sz, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, face, &dxfont);
		InitFont = true;
	}
}
void CDraw_InitLine(IDirect3DDevice9 *m_pD3Ddev)
{
	if (!InitLine)
	{
		D3DXCreateLine(m_pD3Ddev, &dxLine);
		InitLine = true;
	}
}

void CDraw_FillRect(IDirect3DDevice9* m_pD3Ddev, float x, float y, float w, float h, D3DCOLOR Color)
{
	D3DTLVERTEX qV[4] = {
		{ (float)x , (float)(y + h), 0.0f, 1.0f, Color },
		{ (float)x , (float)y , 0.0f, 1.0f, Color },
		{ (float)(x + w), (float)(y + h), 0.0f, 1.0f, Color },
		{ (float)(x + w), (float)y , 0.0f, 1.0f, Color }
	};
	const DWORD D3DFVF_TL = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
	m_pD3Ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pD3Ddev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pD3Ddev->SetFVF(D3DFVF_TL);
	m_pD3Ddev->SetTexture(0, Primitive);
	m_pD3Ddev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, qV, sizeof(D3DTLVERTEX));
}

void CDraw_Rect(float x, float y, float w, float h, D3DCOLOR c)
{
	CDraw_Line(x, y, x + w, y, c);
	CDraw_Line(x, y + h, x + w, y + h, c);
	CDraw_Line(x, y, x, y + h, c);
	CDraw_Line(x + w, y, x + w, y + h, c);
}


void CDraw_Text(char *str, D3DCOLOR color, int x, int y, int w, int h)
{
	static RECT textbox;
	SetRect(&textbox, x, y, w, h);
	if (dxfont)
		dxfont->DrawTextA(NULL,
			str,
			-1,
			&textbox,
			DT_LEFT | DT_TOP,
			color);
}
void CDraw_Line(float x1, float y1, float x2, float y2, D3DCOLOR color)
{
	D3DXVECTOR2 vert[2];
	vert[0].x = x1;
	vert[0].y = y1;
	vert[1].x = x2;
	vert[1].y = y2;
	dxLine->Draw(vert, 2, color);
}
void CDraw_Circle(float x, float y, float radius, int sides, DWORD color)
{
	sides = min(127, sides);
	D3DXVECTOR2 line[128];
	float step = M_PI * 2.0 / sides;
	for (int s = 0; s <= sides; s++)
	{
		float a = step * s;
		float x1 = radius * cos(a) + x;
		float y1 = radius * sin(a) + y;
		line[s].x = x1;
		line[s].y = y1;
	}
	dxLine->Begin();
	dxLine->Draw(line, sides + 1, color);
	dxLine->End();
}