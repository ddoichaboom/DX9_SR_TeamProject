#pragma once
#include "CEditorObject.h"

namespace Engine
{
	class CCollision;
	class CCollider;
}

class CEditorMapCollider : public CEditorObject
{
private:
	explicit        CEditorMapCollider(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit        CEditorMapCollider(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale);
	virtual         ~CEditorMapCollider();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int    Update_GameObject(const _float& fTimeDelta) override;
	virtual void    LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual void    Render_GameObject() override;

private:
	HRESULT         Add_Component();

public:
	// Scale은 Collider에 적용 (Client 구조와 동일)
	void            Set_ColliderScale(_vec3 vScale);
	_vec3           Get_ColliderScale() const { return m_vColliderScale; }

public:
	// 기본 생성 (위치만)
	static CEditorMapCollider* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

	// 전체 파라미터 생성 (맵 로드용)
	static CEditorMapCollider* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale);

private:
	// Client와 동일한 구조
	Engine::CCollision* m_pCollisionCom;    // Collision 컴포넌트
	Engine::CCollider* m_pCollider;        // CreateCollider()로 생성된 Collider

	_vec3               m_vColliderScale;   // Collider 크기 (Transform과 별도)

	const _tchar* m_szColliderName = L"EditorMapCollider";

protected:
	virtual void    Free() override;
};

