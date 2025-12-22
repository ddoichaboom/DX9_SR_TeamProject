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
	HRESULT				Ready_Collider(CTransform* _prtTransComp);
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
	void				Set_Scale(_vec3 _scale);
	_vec3				Get_Scale();

	//상대적인 위치 
	void				Set_RelativePos(_vec3 _pos);
	_vec3				Get_RelativePos();

	static CCollider* Create(LPDIRECT3DDEVICE9 pGraphicDev, CTransform* _prtTransComp);

protected:
	virtual void Free();

protected:
	_matrix		m_matWorld;
	CCubeCol* m_pBufferCom;
	CTransform* m_pTransformCom;
	CTransform* m_pPrtTransformCom;

	//function<void(CollisionInfo)> m_BindFunc;
};

END