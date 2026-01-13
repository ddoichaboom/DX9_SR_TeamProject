#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CTransform;
	class CCollision;
	class CCollider;
}


class CDebugObject : public CGameObject    
{

protected:
	explicit			CDebugObject(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit			CDebugObject(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale);

	explicit			CDebugObject(const CDebugObject& rhs);
	virtual				~CDebugObject();

public:
	HRESULT				Ready_GameObject() override;
	_int				Update_GameObject(const _float& fTimeDelta) override;
	void				LateUpdate_GameObject(const _float& fTimeDelta) override;
	void				Render_GameObject() override;


protected:
	HRESULT				Add_Component() override;

public:
	void				Activate() override;
	void				Deactivate() override;	

public:
	static CDebugObject* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale);

protected : 
	virtual		void	Free();
	void			OnCollision(CollisionInfo info);

protected:	
	Engine::CTransform* m_pTransformCom;
	Engine::CCollision* m_pCollisionCom;

	Engine::CCollider* m_pCollider;
	const	_tchar* m_szColliderName = L"TestDebug";

	_vec3	m_vPos;
	_vec3	m_vScale;
};

