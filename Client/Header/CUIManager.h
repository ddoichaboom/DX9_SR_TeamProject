#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "CEventMgr.h"

#include "CBaseUI.h"

class CBaseUI;
class CEffectUI;
class CCursor;
class CDashUI;
class CSlotUI;


class CUIManager : public CBase, public IListener
{
	DECLARE_SINGLETON(CUIManager)
private :
	explicit CUIManager();
	virtual ~CUIManager();

private :
	virtual void Free() override;


public :	
	void		OnEvent(EVENT_TYPE _type, EventData* _pData) override;
	void		Change_UIState(UI_STATE eState);
	void		Create_TextUI(LPDIRECT3DDEVICE9 pGraphicDev, COLLIDER_TAG eTag, _int iTimes);
	void		Create_TextEffect(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _int iTimes);
public :
	HRESULT Ready_GameObject(LPDIRECT3DDEVICE9 pGraphicDev);
	void	Update_GameObject(const _float& fTimeDelta);
	void	LateUpdate_GameObject(const _float& fTimeDelta);
	

private :
	HRESULT			Add_ProtoType(LPDIRECT3DDEVICE9 pGraphicDev);
	HRESULT			Add_UI(LPDIRECT3DDEVICE9 pGraphicDev);
	void			Sort_UI(UI_STATE eState);


	
public :
	void			Set_RenderEffect(_bool bRender) { m_bRenderEffectUI = bRender; }
	void			Set_OnEffectUI(_bool  bDrink);
	void			Set_OnDashUI(_bool bDash);
	void			Set_OnSlotUI(_bool bSlot);
	
private :	
	UI_STATE	m_eNowState;
	map<UI_STATE, list<CBaseUI*>> m_mapUI;

	

	CEffectUI*	m_pEffectUI;
	_bool		m_bRenderEffectUI;

	CDashUI*	m_pDashUI;
	_bool		m_bDash;

	CSlotUI*	m_pSlotUI;
	_bool		m_bSlot;
};

