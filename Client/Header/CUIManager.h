#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "CEventMgr.h"

#include "CBaseUI.h"

class CBaseUI;


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
	void		Create_TextUI(LPDIRECT3DDEVICE9 pGraphicDev,const wstring& wText, _int iTimes);
public :
	HRESULT Ready_GameObject(LPDIRECT3DDEVICE9 pGraphicDev);
	void	Update_GameObject(const _float& fTimeDelta);
	void	LateUpdate_GameObject(const _float& fTimeDelta);
	

private :
	HRESULT			Add_ProtoType(LPDIRECT3DDEVICE9 pGraphicDev);
	HRESULT			Add_UI(LPDIRECT3DDEVICE9 pGraphicDev);
	void			Sort_UI(UI_STATE eState);


	

	
private :	
	UI_STATE	m_eNowState;
	map<UI_STATE, list<CBaseUI*>> m_mapUI;

};

