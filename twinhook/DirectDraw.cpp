

#include "DirectDraw.h"


CDirectDraw::CDirectDraw(IDirect3DDevice9* pDevice) :
	m_pDevice(pDevice),
	m_pFont(NULL),
	m_pSprite(NULL)

{
	D3DXCreateFontW(pDevice, 25, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		L"Microsoft YaHei", &m_pFont);
	D3DXCreateSprite(pDevice, &m_pSprite);
}

CDirectDraw::~CDirectDraw(void)
{
	if (m_pFont)
		m_pFont->Release();
	if (m_pSprite)
		m_pSprite->Release();
}

HRESULT CDirectDraw::DrawText(LPCWSTR lpszText, int nSize, int x, int y, D3DCOLOR color)
{
	HRESULT hr;

	RECT rc;
	SetRect(&rc, x, y, 0, 0);

	if (m_pSprite)
		hr = m_pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);

	if (m_pFont)
	{
		hr = m_pFont->DrawTextW(m_pSprite, lpszText, nSize, &rc, DT_CALCRECT, color);
		hr = m_pFont->DrawTextW(m_pSprite, lpszText, nSize, &rc, DT_NOCLIP, color);
	}

	if (m_pSprite)
		hr = m_pSprite->End();

	return hr;
}
