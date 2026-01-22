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
	//생성 + 컴포넌트가 보관CreateCollider
	CCollider*			CreateCollider(CGameObject* _obj, const _tchar* _name);
	CCollider*			GetCollider()
	{
		if (m_mapCollider.empty()) return nullptr;
		else if (m_pMainCollider) return m_pMainCollider;
		else return m_mapCollider.begin()->second;
	}
	CCollider*			GetCollider(const _tchar* _name) 
	{	
		auto iter = m_mapCollider.find(_name);
		if (iter == m_mapCollider.end()) return nullptr;
		else return iter->second;
	}
	map<const _tchar*, CCollider*>& GetColliderMap()
	{
		return m_mapCollider;
	}
	//Main Collider 설정. 여러 콜라이더가 있을 때 GetCollider()에서 메인만 꺼내오기 위함
	void				SetMainCollider(const _tchar* _name);

	void				SetCollision(CollisionInfo info, const _tchar* _name);
	
public:
	//충돌 여부만 판단 후 충돌 처리 
	static void			Collision_Base(CCollider* _aCol, CCollider * _bCol);
	
	//충돌 처리 후 미는 범위를 전달 함수
	static void			Collision_Diff(CCollider* _obj, CCollider* _terrain, COLLIDER_TAG eTag = TAG_NONE);

	//충돌 여부만 판단하는 AABB 함수 
	static bool			CheckCollision(CCollider* _aCol, CCollider* _bCol );

	//충돌 여부 + 충돌 범위를 반환하는 함수 ( 충돌에 따라 밀어내기 위한 정보)
	static bool			CheckCollision_Diff(CCollider* _obj, CCollider* _terrain, _vec3* diff);

	//매개변수로 들어온 객체가 마우스에 Picked 됐는지 반환
	static bool			Collision_Mouse(HWND hWnd, LPDIRECT3DDEVICE9 _pGraphicDev, CCollider* _col);

	//콜라이더와 선 충돌 탐지 함수
	static bool			Collision_Ray(CCollider* _col, _vec3 _RayPos, _vec3 _RayDir);

	//마우스 좌표값에 대한 Ray를 반환
	static void			GetRay(HWND hWnd,LPDIRECT3DDEVICE9 _pGraphicDev, _vec3* pOutRayWorldPos, _vec3* pOutRayWorldDir);


public:
	static CCollision*	Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();
	void				Reset() override; 
private:
	virtual void		Free();

private:
	map<const _tchar*, CCollider*> m_mapCollider;
	CCollider* m_pMainCollider;
};

END