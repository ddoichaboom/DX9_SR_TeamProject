#pragma once

#include	"CBase.h"
#include	"CScene.h"

BEGIN(Engine)

class ENGINE_DLL CManagement : public CBase
{
	DECLARE_SINGLETON(CManagement)

private:
	explicit	CManagement();
	virtual		~CManagement();

public:
	CComponent* Get_Component(COMPONENTID eID,
		const _tchar* pLayerTag,
		OBJ_ID _objID,
		const _tchar* pComponentTag);
	CLayer* Get_Layer(const _tchar* pLayerTag);


	//TODO : 추후에 Scene - Layer에서 Set하도록 수정하기 
	_uint					GetPlayerID() { return m_iPlayerID; }

public:
	HRESULT					Set_Scene(CScene* pScene);
	_int					Update_Scene(const _float& fTimeDelta);
	void					LateUpdate_Scene(const _float& fTimeDelta);
	void					Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	CScene*					m_pScene;

public:
	virtual void			Free();

protected:
	//Player ID는 0으로 고정 
	_uint			m_iPlayerID = 0;
};

END