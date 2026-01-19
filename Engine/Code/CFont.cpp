#include "CFont.h"

CFont::CFont(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev), m_pSprite(nullptr), m_pFont(nullptr), m_bKorean(false), m_bCenter(false)
{
	m_pGraphicDev->AddRef();
}

CFont::~CFont()
{
}

HRESULT CFont::Ready_Font(const _tchar* pFontType, const _uint& iWidth, const _uint& iHeight, const _uint& iWeight, _bool bKorean, _bool bCenter)
{
	m_bKorean = bKorean;
	m_bCenter = bCenter;
	D3DXFONT_DESC			tFont_Desc;
	ZeroMemory(&tFont_Desc, sizeof(D3DXFONT_DESC));

	if(m_bKorean)
		tFont_Desc.CharSet = HANGUL_CHARSET;
	else
		tFont_Desc.CharSet = DEFAULT_CHARSET;

	tFont_Desc.Width = iWidth;
	tFont_Desc.Height = iHeight;
	tFont_Desc.Weight = iWeight;

	if (m_bCenter)
		tFont_Desc.Italic = TRUE;
	else
		tFont_Desc.Italic = FALSE;
	lstrcpy(tFont_Desc.FaceName, pFontType);

	if (FAILED(D3DXCreateFontIndirect(m_pGraphicDev, &tFont_Desc, &m_pFont)))
	{
		MSG_BOX("Font Create Failed");
		return E_FAIL;
	}

	if (FAILED(D3DXCreateSprite(m_pGraphicDev, &m_pSprite)))
	{
		MSG_BOX("Sprite Create Failed");
		return E_FAIL;
	}
	

	return S_OK;
}

void CFont::Render_Font(const _tchar* pString, _vec3& vPos, _vec3& vSize, D3DXCOLOR Color)
{
	if (m_bCenter == false)
	{
		RECT rc{ (_long)vPos.x, (_long)vPos.y };

		m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);


		m_pFont->DrawTextW(m_pSprite, pString, lstrlen(pString), &rc, DT_NOCLIP, Color);
		m_pSprite->End();
	}
	else
	{
		RECT rc{ vPos.x - vSize.x * 0.5f, vPos.y - vSize.y*0.5f, vPos.x + vSize.x * 0.5f, vPos.y + vSize.y * 0.5f};

		m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);


		m_pFont->DrawTextW(m_pSprite, pString, lstrlen(pString), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE, Color);
		m_pSprite->End();
	}
	
}

CFont* CFont::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFontType, const _uint& iWidth, const _uint& iHeight, const _uint& iWeight, _bool bKorean, _bool bCenter)
{
	CFont* pFont = new CFont(pGraphicDev);

	if (FAILED(pFont->Ready_Font(pFontType, iWidth, iHeight, iWeight, bKorean, bCenter)))
	{
		Safe_Release(pFont);
		MSG_BOX("Font Create Failed");
		return nullptr;
	}

	return pFont;
}

void CFont::Free()
{
}
