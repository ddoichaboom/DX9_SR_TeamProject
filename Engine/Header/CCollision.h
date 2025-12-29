#pragma once
#include "CComponent.h"
#include "CCollider.h"

BEGIN(Engine)

class CTransform;

class ENGINE_DLL CCollision :
    public CComponent
{
private:
	explicit CCollision();
	explicit CCollision(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCollision(const CCollision& rhs);
	virtual ~CCollision();

public:
	virtual		_int	Update_Component(const _float& fTimeDelta);
	virtual		void	LateUpdate_Component();

public:
	void CreateCollider(CTransform* _prtTransComp);
	CCollider* GetCollider() { return m_pCollider; }
	
	void OnCollision() { m_bCanCollision = true; }
	void OffCollision() { m_bCanCollision = false; }

	void SetCollision(CollisionInfo info);
	void Collision_Base(CCollider *_other);
	bool CheckCollision(CCollider* _other);

	//void Collision_Diff(CCollider* _other); //밀어내기 필요할 때 구현
	bool CheckCollision_Diff(CCollider* _other, _vec3* diff);

public:
	void	GetRay(HWND hWnd, _vec3* pOutRayWorldPos, _vec3* pOutRayWorldDir);
	bool	Collision_Mouse(HWND hWnd);

public:
	static CCollision* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();

private:
	virtual void	Free();

private:
	CCollider* m_pCollider;
	bool m_bCanCollision;
};

END