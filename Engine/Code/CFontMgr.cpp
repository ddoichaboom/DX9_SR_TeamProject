#include "CFontMgr.h"

IMPLEMENT_SINGLETON(CFontMgr)

CFontMgr::CFontMgr()
{
}

CFontMgr::~CFontMgr()
{
	Free();
}

HRESULT CFontMgr::Ready_Font(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFontTag, const _tchar* pFontType, const _uint& iWidth, const _uint& iHeight, const _uint& iWeight, _bool	bKorean, _bool	bCenter)	
{
	CFont* pMyFont = Find_Font(pFontTag);

	if (nullptr != pMyFont)
		return E_FAIL;

	pMyFont = CFont::Create(pGraphicDev, pFontType, iWidth, iHeight, iWeight, bKorean, bCenter);

	if (nullptr == pMyFont)
		return E_FAIL;

	m_mapFont.insert({ pFontTag, pMyFont });

	return S_OK;
}


void CFontMgr::Render_Font(FontData* pData)
{
	CFont* pFont = Find_Font(pData->pFontTag);

	if (pFont != nullptr);
		pFont->Render_Font(pData->pString.c_str(), pData->pPos, pData->pSize, pData->Color);
}

void CFontMgr::Add_RenderFont(FontData* pFontData)
{
	m_RenderFont.push_back(pFontData);
}

void CFontMgr::Render_FontGroup()
{
	for (auto* pObj : m_RenderFont)
		Render_Font(pObj);

	Clear_RenderFont();
}

void CFontMgr::Clear_RenderFont()
{
	m_RenderFont.clear();
}

CFont* CFontMgr::Find_Font(const _tchar* pFontTag)
{
	auto	iter = find_if(m_mapFont.begin(), m_mapFont.end(), CTag_Finder(pFontTag));

	if (iter == m_mapFont.end())
		return nullptr;

	return iter->second;
}

void CFontMgr::Free()
{	
	Clear_RenderFont();
	for_each(m_mapFont.begin(), m_mapFont.end(), CDeleteMap());
	m_mapFont.clear();	
}
