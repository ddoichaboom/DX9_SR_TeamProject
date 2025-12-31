#pragma once
#include "CComponent.h"
#include "CCollider.h"

BEGIN(Engine)

//캐릭터는 CreateCollider할 떄 반횐된 Collider의 포인터를 멤버변수로 소유해서 직접 다루기 
//Collision컴포넌트는 Collider를 보관 + Collider들의 Update,Late_Update를 담당 + static 충돌 체크 함수 제공 
class CTransform;
class ENGINE_DLL CCollision :
    public CComponent
{
protected:
	explicit			CCollision();
	explicit			CCollision(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit			CCollision(const CCollision& rhs);
	virtual				~CCollision();

public:
	virtual	_int		Update_Component(const _float& fTimeDelta);
	virtual	void		LateUpdate_Component();

public:
	//생성 + 컴포넌트가 보관
	CCollider*			CreateCollider(CTransform* _prtTransComp, const _tchar* _name = L"Base");
	CCollider*			GetCollider(const _tchar* _name = L"Base") 
	{	
		return m_mapCollider[_name]; 
	}

	void				SetCollision(CollisionInfo info, const _tchar* _name = L"Base");
	
public:
	//충돌 여부만 판단 후 충돌 처리 
	static void			Collision_Base(CCollider* _aCol, CCollider * _bCol);
	
	//충돌 여부만 판단하는 AABB 함수 
	static bool			CheckCollision(CCollider* _aCol, CCollider* _bCol );

	//충돌 여부 + 충돌 범위를 반환하는 함수 ( 충돌에 따라 밀어내기 위한 정보)
	//TODO : 지형생기면 테스트하기 
	static bool			CheckCollision_Diff(CCollider* _aCol, CCollider* _bCol, _vec3* diff);

	//매개변수로 들어온 객체가 마우스에 Picked 됐는지 반환
	static bool			Collision_Mouse(HWND hWnd, LPDIRECT3DDEVICE9 _pGraphicDev, CCollider* _col);

	static void			GetRay(HWND hWnd,LPDIRECT3DDEVICE9 _pGraphicDev, _vec3* pOutRayWorldPos, _vec3* pOutRayWorldDir);

public:
	static CCollision*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();

private:
	virtual void		Free();

private:
	map<const _tchar*, CCollider*> m_mapCollider;

};

END