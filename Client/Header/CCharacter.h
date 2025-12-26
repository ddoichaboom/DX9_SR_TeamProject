#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCollider;
	class CStateComponent;
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
	virtual HRESULT		Add_Component();
	//void				Set_OnTerrain();
	//State 세팅 설정 
	virtual void		CreateStates() {};

protected:
	void				Free() override;

protected:
	Engine::CRcTex*		m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture*	m_pTextureCom;
	Engine::CCollider*	 m_pCollider;

	Engine::CStateComponent* m_pStateCom;
	
protected:
	float m_fTime;
};

