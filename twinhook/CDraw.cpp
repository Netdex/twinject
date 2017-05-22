#include "stdafx.h"
#include "CDraw.h"

IDirect3DTexture9* Primitive = NULL;
bool Init = false;

HRESULT GenerateTexture(IDirect3DDevice9 *pD3Ddev, IDirect3DTexture9 **ppD3Dtex, DWORD colour32)
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

void DrawRect(IDirect3DDevice9* m_pD3Ddev, float x, float y, float w, float h, D3DCOLOR Color)
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

void CBeginScene(LPDIRECT3DDEVICE9 m_pD3Ddev)
{
	if (!Init)
	{
		GenerateTexture(m_pD3Ddev, &Primitive,
			D3DCOLOR_ARGB(255, 255, 255, 255)
		);
		Init = true;
	}
}

void CEndScene(LPDIRECT3DDEVICE9 m_pD3Ddev)
{
	DrawRect(m_pD3Ddev, 10, 10, 100, 100, D3DCOLOR_ARGB(255, 255, 2, 200));
}