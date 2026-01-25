#pragma once
#include "CGameObject.h"

namespace Engine
{
    class CRcTex;
    class CTransform;
    class CCollision;
    class CCollider;
}

class CDoorLeft;
class CDoorRight;

class CDoor : public CGameObject
{
private:
    explicit CDoor(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CDoor(const CDoor& rhs);
    virtual ~CDoor();

public:
    virtual HRESULT     Ready_GameObject() override;
    virtual _int        Update_GameObject(const _float& fTimeDelta) override;
    virtual void        LateUpdate_GameObject(const _float& fTimeDelta) override;
    virtual void        Render_GameObject() override;

    void        OnCollision(CollisionInfo info);

public:
    // Door ID 설정/조회 ( iRoomIndex하고 혼동 금지 )
    void        Set_DoorID(_int iID) { m_iDoorID = iID; }
    _int        Get_DoorID() const { return m_iDoorID; }

    // 문 열림/닫힘 제어
    void        Open();
    void        Close();
    _bool       Is_Open() const { return m_bIsOpen; }
    _bool       Is_Animating() const;

    // Transform 설정 (두 문에 동시 적용)
    void        Set_Position(const _vec3& vPos);
    void        Set_Rotation(const _vec3& vRot);
    void        Set_Scale(const _vec3& vScale);

    // 문 간격 설정 (기본값: 0.0f - 붙어있음)
    void        Set_DoorGap(_float fGap) { m_fDoorGap = fGap; }
    void        Set_DoorType(DOOR_TYPE eType);

    virtual void Deactivate() override;
    virtual void Activate() override;


private:
    virtual HRESULT     Add_Component() override;
    virtual void        Free() override;

    void        Update_DoorPositions();

public:
    static CDoor* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    static CDoor* Create(LPDIRECT3DDEVICE9 pGraphicDev,
                        _int iDoorID,
                        const _vec3& vPos,
                        const _vec3& vRot = { 0.f, 0.f, 0.f },
                        const _vec3& vScale = { 8.f, 16.f, 1.f });

private:
    Engine::CRcTex* m_pBufferCom;
    Engine::CTransform* m_pTransformCom;

    // Collider 관련 추가
    Engine::CCollision* m_pCollisionCom;
    Engine::CCollider* m_pCollider;
    const _tchar*       m_szColliderName = L"DoorCollider";

    CDoorLeft* m_pDoorLeft;
    CDoorRight* m_pDoorRight;

    _int        m_iDoorID;          // 문 식별자 (-1은 미지정)
    _bool       m_bIsOpen;          // 현재 열린 상태인지

    // Transform 정보
    _vec3       m_vPosition;
    _vec3       m_vRotation;
    _vec3       m_vScale;
    _float      m_fDoorGap;         // 두 문 사이 간격
    DOOR_TYPE   m_eDoorType;

public:
    static wstring szDoorOpen;
};

