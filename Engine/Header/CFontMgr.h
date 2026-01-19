#pragma once

#include "CBase.h"
#include "Engine_Define.h"
#include "CFont.h"

BEGIN(Engine)

class ENGINE_DLL CFontMgr : public CBase
{
	DECLARE_SINGLETON(CFontMgr)

private:
	explicit CFontMgr();
	virtual ~CFontMgr();

public:
	HRESULT				Ready_Font(LPDIRECT3DDEVICE9 pGraphicDev,
		const _tchar* pFontTag,
		const _tchar* pFontType,
		const _uint& iWidth,
		const _uint& iHeight,
		const _uint& iWeight,
		_bool	bKorean,
		_bool	bCenter
	);

	


	void				Add_RenderFont(FontData* pFontData);
	
	void				Render_FontGroup();
	void				Clear_RenderFont();

private:
	CFont* Find_Font(const _tchar* pFontTag);

	void				Render_Font(FontData* pData);

private:
	map<const _tchar*, CFont*>			m_mapFont;
	list<FontData*>						m_RenderFont;


private:
	virtual void	Free();
};

END