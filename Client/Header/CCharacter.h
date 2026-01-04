#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CStateComponent;
	class CCollision;
}

class CCharacter :
    public CGameObject
{
protected:
	explicit			CCharacter(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit			CCharacter(const CCharacter& rhs);
	virtual				~CCharacter();

public:
	HRESULT				Ready_GameObject() override;
	_int				Update_GameObject(const _float& fTimeDelta) override;
	void				LateUpdate_GameObject(const _float& fTimeDelta) override;
	void				Render_GameObject() PURE;



protected:
	HRESULT				Add_Component() override;
	//void				Set_OnTerrain();

protected:
	void				Free() override;
	virtual void		ChangeState(_uint nextStateID) {};

protected:
	Engine::CRcTex*		m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture*	m_pTextureCom;
	Engine::CStateComponent* m_pStateCom;
	Engine::CCollision* m_pCollisionCom;

protected:
	float m_fTime;

};

