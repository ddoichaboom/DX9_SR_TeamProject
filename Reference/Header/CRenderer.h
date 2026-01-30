#pragma once

#include "CBase.h"
#include "CGameObject.h"
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer : public CBase
{
	DECLARE_SINGLETON(CRenderer)

private:
	explicit CRenderer();
	virtual ~CRenderer();

public:
	void			Add_RenderGroup(RENDERID eType, CGameObject* pGameObject);
	void			Render_GameObject(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Clear_RenderGroup();
public:
	void			SetViewPortEvent(_ulong  _x, _ulong _y, _ulong _cx, _ulong _cy);
	void			SetClearViewPortEvent(LPDIRECT3DDEVICE9& pGraphicDev);

private:
	void			Render_Priority(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_NonAlpha(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_NonAlpha_WRAP(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_NonAlpha_Quality(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_Alpha(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_Alpha_UI(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_UI(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_DEBUG(LPDIRECT3DDEVICE9& pGraphicDev);


private:
	list<CGameObject*>			m_RenderGroup[RENDER_END];
	
private:
	virtual void		Free();

private:
	//기본 뷰포트 
	const D3DVIEWPORT9	m_OriginViewPort{0,0,WINCX,WINCY,0.f, 1.0f};
	// ViewPort 변환 행렬 변경 할 때 사용할 변수들
	D3DVIEWPORT9	m_EventViewPort{};
	D3DVIEWPORT9	m_EventUIViewPort{};

	pair<_ulong, _ulong> m_EventMinSize = { 1024,576 };
	bool			m_bViewPortEvent = false;

};

END
