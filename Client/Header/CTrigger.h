#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CTransform;
	class CCollision;
	class CCollider;
}

class CTrigger : public CGameObject
{
protected:
	explicit			CTrigger(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit			CTrigger(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale);
	virtual				~CTrigger();

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

	virtual void		OnBeginCollision();
	virtual void		OnCollision(CGameObject * _other);
	virtual void		OnEndCollision();

	void				Bind_OnBegin(function<void()> _func);
	void				Bind_OnEnd(function<void()> _func);

public:
	void				Set_ColliderScale(_vec3 _scale);

public:
	static CTrigger*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CTrigger*	Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale);

protected:
	virtual		void	Free();

protected:
	//이벤트에 바인딩 할 함수 
	function<void()>	m_FuncBind_Begin;
	function<void()>	m_FuncBind_End;

	Engine::CTransform* m_pTransformCom;
	Engine::CCollision* m_pCollisionCom;

	Engine::CCollider*	m_pCollider;
	const	_tchar*		m_szColliderName = L"Collider";

	_vec3				m_vPos;
	_vec3				m_vScale;

	//현재 프레임에 충돌했는지 확인 요용
	_bool				m_bCollision;
	vector<CGameObject*> m_CollisionList;

};

