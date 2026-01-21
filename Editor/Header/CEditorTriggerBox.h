#pragma once
#include "CEditorObject.h"
#include "Engine_Enum.h"

namespace Engine
{
	class CCollision;
	class CCollider;
}

class CEditorTriggerBox : public CEditorObject
{
private:
	explicit        CEditorTriggerBox(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit        CEditorTriggerBox(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale);
	virtual         ~CEditorTriggerBox();

public:
	virtual HRESULT Ready_GameObject() override;
	virtual _int    Update_GameObject(const _float& fTimeDelta) override;
	virtual void    LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual void    Render_GameObject() override;

private:
	HRESULT         Add_Component();

public:
	// Trigger 속성
	void            Set_TriggerType(TRIGGER_TYPE eType) { m_eTriggerType = eType; }
	TRIGGER_TYPE    Get_TriggerType() const { return m_eTriggerType; }

	// Collider Scale
	void            Set_ColliderScale(_vec3 vScale);
	_vec3           Get_ColliderScale() const { return m_vColliderScale; }

public:
	// 기본 생성 (위치만)
	static CEditorTriggerBox* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

	// 전체 파라미터 생성 (맵 로드용)
	static CEditorTriggerBox* Create(LPDIRECT3DDEVICE9 pGraphicDev,
										_vec3 vPos,
										_vec3 vScale,
										TRIGGER_TYPE eType);

private:
	// Client와 동일한 구조
	Engine::CCollision* m_pCollisionCom;    // Collision 컴포넌트
	Engine::CCollider* m_pCollider;        // CreateCollider()로 생성된 Collider

	_vec3               m_vColliderScale;   // Collider 크기

	const _tchar* m_szColliderName = L"EditorTriggerBox";

	// TriggerBox 전용 속성 
	TRIGGER_TYPE        m_eTriggerType;         // 트리거 타입

protected:
	virtual void    Free() override;
};

