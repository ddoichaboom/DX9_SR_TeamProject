#include "pch.h"
#include "CFontUI.h"
#include "CFontMgr.h"

#include "CBaseUI.h"

CFontUI::CFontUI()
	: m_eFontType(FONT_DEFAULT), m_pParentUI(nullptr), m_bProject(false)
{
}

CFontUI::CFontUI(FONT_TYPE eType, const _vec3& vPos, const _vec3& vScale)
	: m_eFontType(eType), m_vPosition(vPos), m_vScale(vScale), m_pParentUI(nullptr), m_bProject(false)
{
}

CFontUI::CFontUI(const CFontUI& rhs)
	: m_eFontType(rhs.m_eFontType), m_vPosition(rhs.m_vPosition), m_vScale(rhs.m_vScale), m_pParentUI(nullptr), m_bProject(false)
{
}

CFontUI::~CFontUI()
{
}

CFontUI* CFontUI::Create()
{
	CFontUI* pFont = new CFontUI();
	if (!pFont) return nullptr;

	if (FAILED(pFont->Ready_Font()))
	{
		Safe_Release(pFont);
		MSG_BOX("FontUI Create Failed");
		return nullptr;
	}
	return pFont;
}

CFontUI* CFontUI::Create(FONT_TYPE eType, const _vec3& vPos, const _vec3& vScale)
{
	CFontUI* pFont = new CFontUI(eType, vPos, vScale);
	if (!pFont) return nullptr;

	if (FAILED(pFont->Ready_Font()))
	{
		Safe_Release(pFont);
		MSG_BOX("FontUI Create Failed");
		return nullptr;
	}
	return pFont;
}

void CFontUI::Free()
{

}

HRESULT CFontUI::Ready_Font()
{
	switch (m_eFontType)
	{
	case Engine::FONT_DEFAULT:
		m_tData.pFontTag = L"Font_Default";
		m_tData.Color = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
		break;
	case Engine::FONT_NUMBER:
		m_tData.pFontTag = L"Font_Number";
		m_tData.Color = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
		break;
	case Engine::FONT_WORD:
		m_tData.pFontTag = L"Font_Word";
		m_tData.Color = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
		break;
	case Engine::FONT_LARGEWORD:
		m_tData.pFontTag = L"Font_LargeWord";
		m_tData.Color = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
		break;
	case Engine::FONT_LARGENUMBER:
		m_tData.pFontTag = L"Font_LargeNumber";
		m_tData.Color = D3DXCOLOR(0.6f, 0.9f, 0.2f, 1.f);		
		break;
	case Engine::FONT_SMALLNUMBER:
		m_tData.pFontTag = L"Font_SmallNumber";
		m_tData.Color = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
		break;
	}
	m_tData.pSize = m_vScale;

    return S_OK;
}

_int CFontUI::Update_GameObject(const _float& fTimeDelta)
{
    return 0;
}

void CFontUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (nullptr == m_pParentUI)
		return;

	if (false == m_bProject)
	{
		_vec3 vPos = m_pParentUI->Get_Pos();
		vPos += m_vPosition;
		m_tData.pPos = vPos;
	}
	else
	{
		_vec3 vPos = m_pParentUI->Get_ScreenPos();
		vPos += m_vPosition;
		m_tData.pPos = vPos;
	}
	
	CFontMgr::GetInstance()->Add_RenderFont(&m_tData);
}

void CFontUI::Set_Parent(CBaseUI* pParent)
{
	m_pParentUI = pParent;
}

void CFontUI::Set_FontType(FONT_TYPE eType)
{
	m_eFontType = eType;
	Ready_Font();
}
