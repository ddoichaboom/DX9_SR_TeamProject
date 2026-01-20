#pragma once
#include "CGameObject.h"

BEGIN(Engine)

class CCubeCol;
class CTransform;

class ENGINE_DLL CCollider : public CGameObject
{
protected:
	explicit			CCollider(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit			CCollider(const CCollider& rhs) = delete;
	virtual				~CCollider();

public:
	HRESULT				Ready_Collider(CGameObject* _Owner);
	_int				Update_GameObject(const _float& fTimeDelta) override;
	void				LateUpdate_GameObject(const _float& fTimeDelta) override;
	void				Render_GameObject() override;

public:
	function<void(CollisionInfo)> m_BindFunc;
	void				BindFuncToCollision(function<void(CollisionInfo)> func)
	{
		m_BindFunc = func;
	}

	HRESULT				Add_Component();
	_vec3*				GetVtx();
	_matrix				GetWorldMatrix();
	void				Collision(CollisionInfo info);

public:
	CGameObject* Get_Owner()
	{
		return m_pOwner;
	}

	void				Set_Scale(_vec3 _scale);
	_vec3				Get_Scale();

	//상대적인 위치 
	//콜라이더의 월드 변환이 내 Transform * 부모 Transform 으로 진행되는데 
	//아래는 내 Transform을 변경하는 함수 
	_vec3				Get_WorldPos();
	void				Set_RelativePos(_vec3 _pos);
	_vec3				Get_RelativePos();

	void				Set_RotToPrt()
	{
			m_bRotToPrt = true;
	}
	_vec3				Get_ParentPos();
public:
	bool				CanCollision() { return m_bCanCollision; }
	void				OnCollision() { m_bCanCollision = true; }
	void				OffCollision() { m_bCanCollision = false; }

	static CCollider*	Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* _owner);

protected:
	virtual void Free();

protected:
	_matrix				m_matWorld;
	CCubeCol*			m_pBufferCom;
	CTransform*			m_pTransformCom;
	CTransform*			m_pPrtTransformCom;
	CGameObject*		m_pOwner;
	bool				m_bCanCollision;
	//부모 행렬에 의해 회전을 할건지 
	//주의. 콜라이더 대 콜라이더 충돌을 하는 콜라이더면 회전하면 안됨 (OBB 구현 x) 
	bool				m_bRotToPrt;
};

END