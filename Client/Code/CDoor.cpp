#include "pch.h"
#include "CDoor.h"
#include "CDoorLeft.h"
#include "CDoorRight.h"
#include "CProtoMgr.h"
#include "Engine_Enum.h"
#include "CPoolMgr.h"

CDoor::CDoor(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
    , m_pDoorLeft(nullptr)
    , m_pDoorRight(nullptr)
    , m_iDoorID(-1)
    , m_bIsOpen(false)
    , m_vPosition(0.f, 0.f, 0.f)
    , m_vRotation(0.f, 0.f, 0.f)
    , m_vScale(8.f, 16.f, 1.f)
    , m_fDoorGap(0.f)
    , m_eDoorType(DT_END)
    , m_pBufferCom(nullptr)
    , m_pCollisionCom(nullptr)
    , m_pCollider(nullptr)
    , m_pTransformCom(nullptr)
{
    m_eOBJ_ID = OBJ_DOOR;
}

CDoor::CDoor(const CDoor& rhs)
    : CGameObject(rhs)
    , m_pDoorLeft(rhs.m_pDoorLeft)
    , m_pDoorRight(rhs.m_pDoorRight)
    , m_iDoorID(rhs.m_iDoorID)
    , m_bIsOpen(rhs.m_bIsOpen)
    , m_vPosition(rhs.m_vPosition)
    , m_vRotation(rhs.m_vRotation)
    , m_vScale(rhs.m_vScale)
    , m_fDoorGap(rhs.m_fDoorGap)
    , m_eDoorType(rhs.m_eDoorType)
    , m_pBufferCom(rhs.m_pBufferCom)
    , m_pCollisionCom(rhs.m_pCollisionCom)
    , m_pCollider(rhs.m_pCollider)
    , m_pTransformCom(rhs.m_pTransformCom)
{
    m_eOBJ_ID = rhs.m_eOBJ_ID;
}

CDoor::~CDoor()
{
}

HRESULT CDoor::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    // 자식 문 객체 풀에서 꺼내오기
    m_pDoorLeft = CPoolMgr::GetInstance()->Get_Object<CDoorLeft>();

    if (nullptr == m_pDoorLeft)
        return E_FAIL;

    m_pDoorRight = CPoolMgr::GetInstance()->Get_Object<CDoorRight>();

    if (nullptr == m_pDoorRight)
        return E_FAIL;

    _vec3 vColliderScale = m_vScale;
    vColliderScale.x *= 1.0f;
    vColliderScale.z *= 4.0f;


    // Collider 생성 및 설정
    m_pCollider = m_pCollisionCom->CreateCollider(this, m_szColliderName);
    
    if (nullptr == m_pCollider)
        return E_FAIL;

    m_pCollider->Set_Scale(vColliderScale);


    // 부모의 회전을 Collider에 적용 
    //m_pCollider->Set_RotToPrt();


    m_pTransformCom->Update_Component(0.f);

    // 충돌 콜백 함수 바인딩
    m_pCollider->BindFuncToCollision([this](CollisionInfo info)
        {
            this->OnCollision(info);
        });

    Update_DoorPositions();

    return S_OK;
}

_int CDoor::Update_GameObject(const _float& fTimeDelta)
{
    if (IsDead())
    {
        // 자식 Door들 Pool로 반환
        if (m_pDoorLeft || m_pDoorRight)
        {
            Deactivate();
            return RET_DEAD;
        }
    }

    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    if (m_pDoorLeft)
        m_pDoorLeft->Update_GameObject(fTimeDelta);

    if (m_pDoorRight)
        m_pDoorRight->Update_GameObject(fTimeDelta);

    return iExit;
}

void CDoor::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);

    if (m_pDoorLeft)
        m_pDoorLeft->LateUpdate_GameObject(fTimeDelta);

    if (m_pDoorRight)
        m_pDoorRight->LateUpdate_GameObject(fTimeDelta);

    //m_pCollider->Get_Component(ID_DYNAMIC, L"Com_Transform");
    m_pCollisionCom->LateUpdate_Component();
}

void CDoor::Render_GameObject()
{
}

void CDoor::OnCollision(CollisionInfo info)
{
    if (m_bIsOpen)
        return;

    Open();
}

HRESULT CDoor::Add_Component()
{
    Engine::CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Buffer - RcTex ( 렌더용이 아님 ) 
    pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pCollisionCom = dynamic_cast<Engine::CCollision*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collision"));

    NULL_CHECK_RETURN(pComponent, E_FAIL);

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collision", pComponent });

    return S_OK;
}

void CDoor::Set_Position(const _vec3& vPos)
{
    m_vPosition = vPos;

    if (m_pTransformCom)
    {
        m_pTransformCom->Set_Pos(vPos);
    }

    Update_DoorPositions();
}

void  CDoor::Set_Rotation(const _vec3& vRot)
{
    m_vRotation = vRot;

    if (m_pTransformCom)
    {
        m_pTransformCom->Set_Angle(vRot);
    }

    if ((int)vRot.y != 0)
    {
        if (m_pCollider)
        {
            _vec3 vColliderScale = { m_vScale.z * 4.f, m_vScale.y, m_vScale.x };        // 각도가 0이 아닐 경우 x 와 z의 스케일을 스왑해서 AABB 어긋나는 것을 방지 ( 수정 최소화 )
            m_pCollider->Set_Scale(vColliderScale);
        }
    }

    Update_DoorPositions();
}

void  CDoor::Set_Scale(const _vec3& vScale)
{
    m_vScale = vScale;

    if (m_pTransformCom)
    {
        m_pTransformCom->Set_Scale(vScale);
    }

    // Collider 크기도 업데이트
    if (m_pCollider)
    {
        _vec3 vColliderScale = vScale;
        vColliderScale.x *= 1.0f;
        vColliderScale.z *= 4.0f;
        m_pCollider->Set_Scale(vColliderScale);
    }

    Update_DoorPositions();
}

void CDoor::Update_DoorPositions()
{
    if (nullptr == m_pDoorLeft || nullptr == m_pDoorRight)
        return;

    _float fRadianY = D3DXToRadian(m_vRotation.y);
    _vec3 vRight = { cosf(fRadianY), 0.f, -sinf(fRadianY) };

    // 각 문의 Scale.x (CDoor Scale의 절반)
    _float fDoorScaleX = m_vScale.x * 0.5f;

    // 실제 문 너비 = RcTexSide 기본 너비(2) * Scale.x
    _float fActualDoorWidth = 2.f * fDoorScaleX;  // 8

    _float fGapOffset = m_fDoorGap * 0.5f;

    // CDoorLeft: 피벗이 왼쪽(x=0), 오른쪽으로 확장
    // 위치를 중앙 - 실제너비에 두면 범위: 중앙-8 ~ 중앙
    _vec3 vLeftPos = m_vPosition - vRight * (fActualDoorWidth + fGapOffset);
    m_pDoorLeft->Set_Position(vLeftPos);
    m_pDoorLeft->Set_Rotation(m_vRotation);
    m_pDoorLeft->Set_Scale({ fDoorScaleX, m_vScale.y, m_vScale.z });

    // CDoorRight: 피벗이 Scale.x 음수로 오른쪽, 왼쪽으로 확장
    // 위치를 중앙 + 실제너비에 두면 범위: 중앙+8 ~ 중앙
    _vec3 vRightPos = m_vPosition + vRight * (fActualDoorWidth + fGapOffset);
    m_pDoorRight->Set_Position(vRightPos);
    m_pDoorRight->Set_Rotation(m_vRotation);
    m_pDoorRight->Set_Scale({ fDoorScaleX, m_vScale.y, m_vScale.z });
}

void CDoor::Open()
{
    if (m_bIsOpen)
        return;

    m_bIsOpen = true;

    if (m_pDoorLeft)
        m_pDoorLeft->Open();

    if (m_pDoorRight)
        m_pDoorRight->Open();

    // 문이 열리면 Collider 비활성화 (더 이상 충돌 체크 불필요)
    if (m_pCollider)
        m_pCollider->OffCollision();
}

void CDoor::Close()
{
    if (!m_bIsOpen)
        return;

    m_bIsOpen = false;

    if (m_pDoorLeft)
        m_pDoorLeft->Close();

    if (m_pDoorRight)
        m_pDoorRight->Close();

    // 문이 닫히면 Collider 다시 활성화
    if (m_pCollider)
        m_pCollider->OnCollision();
}

void  CDoor::Set_DoorType(DOOR_TYPE eType)
{
    m_eDoorType = eType;

    if (m_pDoorLeft)
        m_pDoorLeft->Set_DoorType(eType);

    if (m_pDoorRight)
        m_pDoorRight->Set_DoorType(eType);
}

void CDoor::Deactivate()
{
    CGameObject::Deactivate();

    // 자식 Door 정리 및 Pool 반환
    if (m_pDoorLeft)
    {
        m_pDoorLeft->ReturnToPool(); // Pool 반환
        m_pDoorLeft = nullptr;
    }

    if (m_pDoorRight)
    {
        m_pDoorRight->ReturnToPool();
        m_pDoorRight = nullptr;
    }

    // CDoor 자체 상태 초기화
    m_bIsOpen = false;
    if (m_pCollider)
        m_pCollider->OnCollision();
}

void CDoor::Activate()
{
    CGameObject::Activate();

    if (!m_pDoorLeft && !m_pDoorRight)
    {
        m_pDoorLeft = CPoolMgr::GetInstance()->Get_Object<CDoorLeft>();
        if (m_pDoorLeft)
            Update_DoorPositions();
        m_pDoorLeft->Set_DoorType(m_eDoorType);

        m_pDoorRight = CPoolMgr::GetInstance()->Get_Object<CDoorRight>();
        if (m_pDoorRight)
            Update_DoorPositions();
        m_pDoorRight->Set_DoorType(m_eDoorType);
    }

}

_bool CDoor::Is_Animating() const
{
    _bool bLeftAnimating = m_pDoorLeft ? m_pDoorLeft->Is_Animating() : false;
    _bool bRightAnimating = m_pDoorRight ? m_pDoorRight->Is_Animating() : false;

    return bLeftAnimating || bRightAnimating;
}

CDoor* CDoor::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CDoor* pInstance = new CDoor(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CDoor Create Failed");
        return nullptr;
    }
    return pInstance;
}

CDoor* CDoor::Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iDoorID,
                    const _vec3& vPos, const _vec3& vRot, const _vec3& vScale)
{
    CDoor* pInstance = new CDoor(pGraphicDev);

    // 초기 값 설정 (Ready_GameObject 전에)
    pInstance->m_vPosition = vPos;
    pInstance->m_vRotation = vRot;
    pInstance->m_vScale = vScale;

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CDoor Create Failed");
        return nullptr;
    }

    pInstance->Set_DoorID(iDoorID);
    pInstance->Set_Position(vPos);
    pInstance->Set_Rotation(vRot);
    pInstance->Set_Scale(vScale);

    return pInstance;
}

void CDoor::Free()
{
    CGameObject::Free();
}