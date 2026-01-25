#include "pch.h"
#include "CDoorRight.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "Engine_Enum.h"

vector<TextureSource> CDoorRight::m_vTextureSource =
{
    {DOOR_1, L"../Bin/Resource/Texture/Terrain/Door/DOOR_1_RIGHT.dds", false, 0, 0, 0, {1.f, 1.f}},
    {DOOR_2, L"../Bin/Resource/Texture/Terrain/Door/DOOR_2_RIGHT.dds", false, 0, 0, 0, {1.f, 1.f}},
    {DOOR_3, L"../Bin/Resource/Texture/Terrain/Door/DOOR_3_RIGHT.dds", false, 0, 0, 0, {1.f, 1.f}},
    {DOOR_ELEVATOR, L"../Bin/Resource/Texture/Terrain/Door/DOOR_ELEVATOR_RIGHT.dds", false, 0, 0, 0, {1.f, 1.f}},
};

CDoorRight::CDoorRight(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
    , m_pBufferCom(nullptr)
    , m_pTransformCom(nullptr)
    , m_pTextureCom(nullptr)
    , m_bIsOpen(false)
    , m_bIsAnimating(false)
    , m_fCurrentAngle(0.f)
    , m_fTargetAngle(0.f)
    , m_fAnimSpeed(360.f)   // 180도/초 (0.5초에 90도)
    , m_vInitialPos(0.f, 0.f, 0.f)
    , m_vInitialRot(0.f, 0.f, 0.f)
    , m_vInitialScale(8.f, 16.f, 1.f)
    , m_eDoorType(DOOR_1)
{
}

CDoorRight::CDoorRight(const CDoorRight& rhs)
    : CGameObject(rhs)
    , m_pBufferCom(rhs.m_pBufferCom)
    , m_pTransformCom(rhs.m_pTransformCom)
    , m_pTextureCom(rhs.m_pTextureCom)
    , m_bIsOpen(rhs.m_bIsOpen)
    , m_bIsAnimating(rhs.m_bIsAnimating)
    , m_fCurrentAngle(rhs.m_fCurrentAngle)
    , m_fTargetAngle(rhs.m_fTargetAngle)
    , m_fAnimSpeed(rhs.m_fAnimSpeed)
    , m_vInitialPos(rhs.m_vInitialPos)
    , m_vInitialRot(rhs.m_vInitialRot)
    , m_vInitialScale(rhs.m_vInitialScale)
    , m_eDoorType(rhs.m_eDoorType)
{
}

CDoorRight::~CDoorRight()
{
}

HRESULT CDoorRight::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (m_pTextureCom)
    {
        m_pTextureCom->Change_Texture(m_eDoorType);
        m_pTextureCom->Set_Frame(_vec2(0, 0));
    }

    return S_OK;
}

_int CDoorRight::Update_GameObject(const _float& fTimeDelta)
{
    if (IsDead())
        return RET_DEAD;

    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    // 애니메이션 업데이트 
    Update_Animation(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    return iExit;
}

void CDoorRight::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CDoorRight::Render_GameObject()
{
    DWORD dOldCullMode;
    m_pGraphicDev->GetRenderState(D3DRS_CULLMODE, &dOldCullMode);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    if (m_pTextureCom)
        m_pTextureCom->Render_Texture();

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, dOldCullMode);

    m_pGraphicDev->SetTexture(0, nullptr);
}


HRESULT CDoorRight::Add_Component()
{
    Engine::CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Buffer - RcTexSide
    pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTexSide*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTexSide"));

    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture 
    pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_DoorRight_Texture_Door"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });


    return S_OK;
}

void    CDoorRight::Set_Position(const _vec3& vPos)
{
    m_vInitialPos = vPos;
    m_pTransformCom->Set_Pos(vPos);
}

void	CDoorRight::Set_Rotation(const _vec3& vRot)
{
    m_vInitialRot = vRot;
    m_pTransformCom->Set_Angle(vRot);
}

void	CDoorRight::Set_Scale(const _vec3& vScale)
{
    m_vInitialScale = vScale;
    
    // Scale.x를 음수로 설정하여 좌우 반전 (오른쪽 피벗 기준으로 변경)
    _vec3 vFlippedScale = { -vScale.x, vScale.y, vScale.z };
    m_pTransformCom->Set_Scale(vFlippedScale);
}

void CDoorRight::Set_DoorType(DOOR_TYPE eType)
{
    m_eDoorType = eType;

    if (m_pTextureCom)
    {
        m_pTextureCom->Change_Texture(m_eDoorType);
        m_pTextureCom->Set_Frame(_vec2(0, 0));
    }
}

void CDoorRight::Open()
{
    if (m_bIsOpen)
        return;

    m_bIsOpen = true;
    m_bIsAnimating = true;
    m_fTargetAngle = 80.f;      // 우측 피벗 기준 +Y 축 회전 (안쪽으로 열림) - 보고 수정
}

void CDoorRight::Close()
{
    if (!m_bIsOpen)
        return;

    m_bIsOpen = false;
    m_bIsAnimating = true;
    m_fTargetAngle = 0.f;
}

void CDoorRight::Deactivate()
{
    CGameObject::Deactivate();  // 부모 호출

    // 상태 변수 초기화
    m_bIsOpen = false;
    m_bIsAnimating = false;
    m_fCurrentAngle = 0.f;
    m_fTargetAngle = 0.f;
    m_eDoorType = DOOR_1;
    m_vInitialPos = { 0.f, 0.f, 0.f };
    m_vInitialRot = { 0.f, 0.f, 0.f };
    m_vInitialScale = { 8.f, 16.f, 1.f };
}

CDoorRight* CDoorRight::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CDoorRight* pInstance = new CDoorRight(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CDoorRight Create Failed");
        return nullptr;
    }

    return pInstance;
}

CDoorRight* CDoorRight::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, const _vec3& vRot, const _vec3& vScale)
{
    CDoorRight* pInstance = new CDoorRight(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CDoorRight Create Failed");
        return nullptr;
    }

    pInstance->Set_Position(vPos);
    pInstance->Set_Rotation(vRot);
    pInstance->Set_Scale(vScale); 

    return pInstance;
}


void CDoorRight::Free()
{
    CGameObject::Free();
}

void CDoorRight::Update_Animation(const _float& fTimeDelta)
{
    if (!m_bIsAnimating)
        return;

    _float fAngleDiff = m_fTargetAngle - m_fCurrentAngle;

    if (fabs(fAngleDiff) < 0.5f)
    {
        m_fCurrentAngle = m_fTargetAngle;
        m_bIsAnimating = false;
    }
    else
    {
        _float fSign = (fAngleDiff > 0.f) ? 1.f : -1.f;
        _float fDelta = fSign * m_fAnimSpeed * fTimeDelta;

        if (fabs(fDelta) > fabs(fAngleDiff))
            fDelta = fAngleDiff;

        m_fCurrentAngle += fDelta;
    }

    // Transform에 현재 회전 적용
    _vec3 vCurrentRot = m_vInitialRot;
    vCurrentRot.y += m_fCurrentAngle;  // Y축 회전에 애니메이션 각도 누적
    m_pTransformCom->Set_Angle(vCurrentRot);
}
