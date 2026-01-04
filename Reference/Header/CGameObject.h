#pragma once
#include "CBase.h"
#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject : public CBase
{
protected:
	explicit CGameObject(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject();

public:
	virtual	HRESULT						Ready_GameObject();
	virtual	_int						Update_GameObject(const _float& fTimeDelta);
	virtual	void						LateUpdate_GameObject(const _float& fTimeDelta);
	virtual	void						Render_GameObject() PURE;

public:
	CComponent* Get_Component(COMPONENTID eID, const _tchar* pComponentTag);
	_float								Get_ViewZ() { return m_fViewZ; }
	void								Compute_ViewZ(const _vec3* pPos);

	bool								IsDead() { return m_bDead; }
	void								SetDead() { m_bDead = true; }

	//ObjectPool
	bool								IsActivate() { return m_bActivate; }
	virtual void						Activate();
	virtual void						Deactivate();

protected:
	virtual HRESULT						Add_Component() { return S_OK; }
	CComponent* Find_Component(COMPONENTID eID, const _tchar* pComponentTag);
	virtual		void					Free();

protected:
	map<const _tchar*, CComponent*>		m_mapComponent[ID_END];
	LPDIRECT3DDEVICE9					m_pGraphicDev;

	_float								m_fViewZ;
	bool								m_bDead;
	bool								m_bActivate;
};

END