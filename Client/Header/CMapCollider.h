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
	_vec3				GetPos() override;
	void				SetPos(_vec3 _pos) override;
	_vec3				GetScale() override;
	void				SetScale(_vec3 _scale);

	void				Activate() override;
	void				Deactivate() override;	

	// CMapLoader에서 설정하며 쓸 함수
public:
	void				Set_ColliderScale(_vec3 _scale);

public:
	static CMapCollider* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CMapCollider* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale);

public:
	COLLIDER_TAG	Get_ColliderTag() const { return m_eColliderTag; }
	void			Set_ColliderTag(COLLIDER_TAG eColliderTag) { m_eColliderTag = eColliderTag; }

protected : 
	virtual		void	Free();

protected:	
	Engine::CTransform* m_pTransformCom;
	Engine::CCollision* m_pCollisionCom;

	Engine::CCollider* m_pCollider;
	const	_tchar* m_szColliderName = L"MapCollider";

	_vec3	m_vPos;
	_vec3	m_vScale;

private:
	COLLIDER_TAG	m_eColliderTag;
};

