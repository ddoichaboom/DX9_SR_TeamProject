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
	
	//충돌 가능 / 불가능 설정 
	bool CanCollision() { return m_bCanCollision; }
	void OnCollision() { m_bCanCollision = true; }
	void OffCollision() { m_bCanCollision = false; }

	//직접 충돌 함수 호출시키기
	void SetCollision(CollisionInfo info);
	
	//충돌 여부만 판단 후 충돌 처리 
	void Collision_Base(CCollider *_other);
	
	//충돌 여부만 판단하는 AABB함수 
	bool CheckCollision(CCollider* _other);

	//충돌 여부 + 충돌 범위를 반환하는 함수 ( 충돌에 따라 밀어내기 위한 정보)
	//TODO : 지형생기면 테스트하기 
	bool CheckCollision_Diff(CCollider* _other, _vec3* diff);

public:
	void	GetRay(HWND hWnd, _vec3* pOutRayWorldPos, _vec3* pOutRayWorldDir);
	//내가 Picked 됐는지 체크
	bool	Collision_Mouse(HWND hWnd);
	//매개변수로 들어온 객체가 Picked 됐는지 체크
	bool	Collision_Mouse_Other(HWND hWnd, CCollision* _pCollision);
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