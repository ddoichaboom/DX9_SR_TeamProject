#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CTransform;
	class CCollision;
	class CCollider;
}


class CMapCollider : public CGameObject    
{

protected:
	explicit			CMapCollider(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit			CMapCollider(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale);

	explicit			CMapCollider(const CMapCollider& rhs);
	virtual				~CMapCollider();

public:
	HRESULT				Ready_GameObject() override;
	_int				Update_GameObject(const _float& fTimeDelta) override;
	void				Render_GameObject() override;


protected:
	HRESULT				Add_Component() override;

public:
	void				SetPos(_vec3 _pos) override;
	void				SetScale(_vec3 _scale);

	void				Activate() override;
	void				Deactivate() override;	

public:
	static CMapCollider* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CMapCollider* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale);

protected : 
	virtual		void	Free();

protected:	
	Engine::CTransform* m_pTransformCom;
	Engine::CCollision* m_pCollisionCom;

	Engine::CCollider* m_pCollider;
	const	_tchar* m_szColliderName = L"MapCollider";

	_vec3	m_vPos;
	_vec3	m_vScale;
};

