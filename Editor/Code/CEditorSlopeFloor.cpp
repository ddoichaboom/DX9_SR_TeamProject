#include "pch.h"
#include "CEditorSlopeFloor.h"

#include "CRenderer.h"
#include "CProtoMgr.h"
#include "CTransform.h"
#include "CRcTexUp.h"  // RcTexUp 버퍼 사용 (피벗이 하단)
#include "CTexture.h"

vector<TextureSource> CEditorSlopeFloor::m_vTextureSource =
{
	{ STATIC_FLOOR, L"../Bin/Resource/Texture/Terrain/Floor/STATIC_FLOOR/FLOORS.dds", true, 0, 7, 7, {2.f, 2.f}},
    { STATIC_FLOOR_FLUID, L"../Bin/Resource/Texture/Terrain/Floor/STATIC_FLOOR/FLOOR_FLUID.dds", true, 0, 2, 2, {0.f, 0.f}},
    { STATIC_FLOOR_SLOPE, L"../Bin/Resource/Texture/Terrain/Floor/STATIC_FLOOR/FLOOR_SLOPE.dds", false, 0, 0, 0, { 2.f, 2.f}}
};

CEditorSlopeFloor::CEditorSlopeFloor(LPDIRECT3DDEVICE9 pGraphicDev)
    : CEditorFloor(pGraphicDev)
    , m_fSlopeAngle(30.f)
    , m_eSlopeDir(SLOPE_POSITIVE_X)
{
    m_iFloorType = STATIC_FLOOR_SLOPE;
}

CEditorSlopeFloor::~CEditorSlopeFloor()
{
}

HRESULT CEditorSlopeFloor::Ready_GameObject()
{
    FAILED_CHECK_RETURN(CEditorObject::Ready_GameObject(), E_FAIL);
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    // 기본 이름
    m_wstrName = L"SlopeFloor";

    return S_OK;
}

_int CEditorSlopeFloor::Update_GameObject(const _float& fTimeDelta)
{
    CEditorObject::Update_GameObject(fTimeDelta);

    // Renderer에 추가
    Engine::CRenderer::GetInstance()->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

    return 0;
}

void CEditorSlopeFloor::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CEditorObject::LateUpdate_GameObject(fTimeDelta);
}

void CEditorSlopeFloor::Render_GameObject()
{
    // 렌더 스테이트 백업
    DWORD dwOldColorOp, dwOldColorArg1, dwOldColorArg2, dwOldTextureFactor;

    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwOldColorOp);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG1, &dwOldColorArg1);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG2, &dwOldColorArg2);
    m_pGraphicDev->GetRenderState(D3DRS_TEXTUREFACTOR, &dwOldTextureFactor);

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    if (m_pTextureCom)
    {
        m_pTextureCom->Render_Texture();
    }

    // 선택 시 색상 변경 (노란색 tint로 경사 바닥 구분)
    if (m_bSelected)
    {
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
        m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 200, 100));  // 주황색 tint
    }

    m_pBufferCom->Render_Buffer();

    // 렌더 스테이트 복원
    m_pGraphicDev->SetTexture(0, nullptr);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, dwOldColorOp);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, dwOldColorArg1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, dwOldColorArg2);
    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwOldTextureFactor);

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
}

HRESULT CEditorSlopeFloor::Add_Component()
{
    Engine::CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[Engine::ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // ========== Buffer (RcTexUp) - 피벗이 하단에 있어 경사 배치에 유리 ==========
    pComponent = m_pBufferCom = dynamic_cast<Engine::CVIBuffer*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTexUp"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[Engine::ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture - 경사 바닥용 (없으면 Static_FloorTexture 사용)
    pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Slope_FloorTexture"));

    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[Engine::ID_DYNAMIC].insert({ L"Com_Texture", pComponent });

    if (m_pTextureCom)
    {
        m_pTextureCom->Change_Texture(m_iFloorType);
        m_pTextureCom->Set_Frame(_vec2(m_iTextureIdx, 0));
    }

    return S_OK;
}

void    CEditorSlopeFloor::Set_FloorType(_uint iType)
{
    CEditorFloor::Set_FloorType(iType);
}

void CEditorSlopeFloor::Set_SlopeAngle(_float fAngle)
{
    m_fSlopeAngle = max(0.f, min(89.f, fAngle));
}

void    CEditorSlopeFloor::Set_SlopeDirection(SLOPE_DIR eDir)
{
    m_eSlopeDir = eDir;
}

void CEditorSlopeFloor::Calculate_Rotation()
{
    //  RcTexUp 기반 경사 회전 계산 
// RcTexUp 특성:
// - 피벗이 하단 중앙 (Y=0)
// - X축 90도 회전 시: 피벗이 -Z 방향 끝에 위치
// - 회전 시 피벗(하단)이 고정되고 상단(+Y)이 이동
//
// 경사 방향별 회전:
// - SLOPE_POSITIVE_X: 피벗이 뒤(-Z), 앞(+Z)이 올라감
// - SLOPE_NEGATIVE_X: 피벗이 앞(+Z), 뒤(-Z)가 올라감 (Y축 180도 회전 필요)
// - SLOPE_POSITIVE_Z: 피벗이 왼쪽(-X), 오른쪽(+X)이 올라감 (Y축 90도 회전)
// - SLOPE_NEGATIVE_Z: 피벗이 오른쪽(+X), 왼쪽(-X)이 올라감 (Y축 -90도 회전)

    _vec3 vRot(0.f, 0.f, 0.f);

    switch (m_eSlopeDir)
    {
    case SLOPE_POSITIVE_X:
        // +X 방향으로 올라감 (실제로는 +Z 방향, 피벗 기준)
        // X축 90도에서 경사 각도만큼 감소 → 피벗 고정, 반대쪽 상승
        vRot = _vec3(90.f - m_fSlopeAngle, 0.f, 0.f);
        break;

    case SLOPE_NEGATIVE_X:
        // -X 방향으로 올라감 (반대 방향)
        // Y축 180도 회전으로 방향 뒤집기 + X축 경사
        vRot = _vec3(90.f - m_fSlopeAngle, 180.f, 0.f);
        break;

    case SLOPE_POSITIVE_Z:
        // +Z 방향으로 올라감
        // Y축 -90도 회전으로 방향 전환 + X축 경사
        vRot = _vec3(90.f - m_fSlopeAngle, -90.f, 0.f);
        break;

    case SLOPE_NEGATIVE_Z:
        // -Z 방향으로 올라감
        // Y축 90도 회전으로 방향 전환 + X축 경사
        vRot = _vec3(90.f - m_fSlopeAngle, 90.f, 0.f);
        break;

    default:
        vRot = _vec3(90.f, 0.f, 0.f);  // 평평한 바닥
        break;
    }

    Set_Rotation(vRot);
}

CEditorSlopeFloor* CEditorSlopeFloor::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CEditorSlopeFloor* pInstance = new CEditorSlopeFloor(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorSlopeFloor Create Failed");
        return nullptr;
    }

    // 위치 설정
    pInstance->Set_Position(vPos);

    // 기본 크기
    pInstance->Set_Scale(_vec3(8.f, 8.f, 1.f));

    // 기본 경사 설정 (30도, +X 방향)
    pInstance->m_fSlopeAngle = 30.f;
    pInstance->m_eSlopeDir = SLOPE_POSITIVE_X;
    pInstance->Calculate_Rotation();

    return pInstance;
}

CEditorSlopeFloor* CEditorSlopeFloor::Create(LPDIRECT3DDEVICE9 pGraphicDev,
                                                _vec3 vPos, _vec3 vRot, _vec3 vScale)
{
    CEditorSlopeFloor* pInstance = new CEditorSlopeFloor(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorSlopeFloor Create Failed");
        return nullptr;
    }

    // Transform 설정
    pInstance->Set_Scale(vScale);
    pInstance->Set_Rotation(vRot);
    pInstance->Set_Position(vPos);
    return pInstance;
}

CEditorSlopeFloor* CEditorSlopeFloor::Create(LPDIRECT3DDEVICE9 pGraphicDev,
    _vec3 vPos, _vec3 vScale,
    _uint iFloorType,
    _float fSlopeAngle,
    SLOPE_DIR eSlopeDir)
{
    CEditorSlopeFloor* pInstance = new CEditorSlopeFloor(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorSlopeFloor Create Failed");
        return nullptr;
    }

    // Transform 설정
    pInstance->Set_Scale(vScale);
    pInstance->Set_Position(vPos);

    // 경사 설정
    pInstance->m_iFloorType = iFloorType;
    pInstance->m_fSlopeAngle = fSlopeAngle;
    pInstance->m_eSlopeDir = eSlopeDir;

    // 텍스처 설정
    pInstance->Set_FloorType(iFloorType);

    // 회전 계산 (경사 방향에 따라)
    pInstance->Calculate_Rotation();

    return pInstance;
}

void CEditorSlopeFloor::Free()
{
    CEditorFloor::Free();
}

_vec3 CEditorSlopeFloor::Get_OppositeEndPosition() 
{
    _vec3 vPivotPos = Get_Position();
    _vec3 vScale = Get_Scale();

    // RcTexUp 기본 길이: 2 단위 (Y: 0 ~ +2)
    _float fLength = 2.f * vScale.y;
    _float fRadian = D3DXToRadian(m_fSlopeAngle);

    // 경사 적용 후 변위
    _float fHorizontalDist = fLength * cosf(fRadian);
    _float fVerticalDist = fLength * sinf(fRadian);

    _vec3 vOffset(0.f, 0.f, 0.f);

    switch (m_eSlopeDir)
    {
    case SLOPE_POSITIVE_X:  // X축 회전 → Z+ 방향으로 올라감
        vOffset = _vec3(0.f, fVerticalDist, fHorizontalDist);
        break;
    case SLOPE_NEGATIVE_X:  // X축 회전 + Y 180도 → Z- 방향으로 올라감
        vOffset = _vec3(0.f, fVerticalDist, -fHorizontalDist);
        break;
    case SLOPE_POSITIVE_Z:  // X축 회전 + Y -90도 → X+ 방향으로 올라감
        vOffset = _vec3(-fHorizontalDist, fVerticalDist, 0.f);
        break;
    case SLOPE_NEGATIVE_Z:  // X축 회전 + Y 90도 → X- 방향으로 올라감
        vOffset = _vec3(fHorizontalDist, fVerticalDist, 0.f);
        break;
    }

    return vPivotPos + vOffset;
}

_vec3 CEditorSlopeFloor::Get_OppositeEndPosition(const _vec3& vDupDir)
{
    _vec3 vPivotPos = Get_Position();
    _vec3 vScale = Get_Scale();

    // RcTexUp 기본 길이: 2 단위 (Y: 0 ~ +2)
    _float fLength = 2.f * vScale.y;
    _float fRadian = D3DXToRadian(m_fSlopeAngle);

    // 경사 적용 후 변위
    _float fHorizontalDist = fLength * cosf(fRadian);
    _float fVerticalDist = fLength * sinf(fRadian);

    _vec3 vOffset(0.f, 0.f, 0.f);

    if (vDupDir.z > 0.f)
    {
        switch (m_eSlopeDir)
        {
        case SLOPE_POSITIVE_X:  // X축 회전 → Z+ 방향으로 올라감
            vOffset = _vec3(0.f, fVerticalDist, fHorizontalDist);
            break;
        case SLOPE_NEGATIVE_X:  // X축 회전 + Y 180도 → Z- 방향으로 올라감
            vOffset = _vec3(0.f, -fVerticalDist, fHorizontalDist);
            break;
        case SLOPE_POSITIVE_Z:  // X축 회전 + Y -90도 → X+ 방향으로 올라감
            vOffset = vDupDir * 32.f;
            break;
        case SLOPE_NEGATIVE_Z:  // X축 회전 + Y 90도 → X- 방향으로 올라감
            vOffset = vDupDir * 32.f;
            break;
        }
    }
    else if (vDupDir.z < 0.f)
    {
        switch (m_eSlopeDir)
        {
        case SLOPE_POSITIVE_X:  // X축 회전 → Z+ 방향으로 올라감
            vOffset = _vec3(0.f, -fVerticalDist, -fHorizontalDist);
            break;
        case SLOPE_NEGATIVE_X:  // X축 회전 + Y 180도 → Z- 방향으로 올라감
            vOffset = _vec3(0.f, fVerticalDist, -fHorizontalDist);
            break;
        case SLOPE_POSITIVE_Z:  // X축 회전 + Y -90도 → X+ 방향으로 올라감
            vOffset = vDupDir * 32.f;
            break;
        case SLOPE_NEGATIVE_Z:  // X축 회전 + Y 90도 → X- 방향으로 올라감
            vOffset = vDupDir * 32.f;
            break;
        }
    }
    
    if (vDupDir.x > 0.f)
    {
        switch (m_eSlopeDir)
        {
        case SLOPE_POSITIVE_X:  // X축 회전 → Z+ 방향으로 올라감
            vOffset = vDupDir * 32.f;;
            break;
        case SLOPE_NEGATIVE_X:  // X축 회전 + Y 180도 → Z- 방향으로 올라감
            vOffset = vDupDir * 32.f;
            break;
        case SLOPE_POSITIVE_Z:  // X축 회전 + Y -90도 → X+ 방향으로 올라감
            vOffset = _vec3(-fHorizontalDist, fVerticalDist, 0.f);
            break;
        case SLOPE_NEGATIVE_Z:  // X축 회전 + Y 90도 → X- 방향으로 올라감
            vOffset = _vec3(-fHorizontalDist, -fVerticalDist, 0.f);
            break;
        }
    }
    else if (vDupDir.x < 0.f)
    {
        switch (m_eSlopeDir)
        {
        case SLOPE_POSITIVE_X:  // X축 회전 → Z+ 방향으로 올라감
            vOffset = vDupDir * 32.f;
            break;
        case SLOPE_NEGATIVE_X:  // X축 회전 + Y 180도 → Z- 방향으로 올라감
            vOffset = vDupDir * 32.f;
            break;
        case SLOPE_POSITIVE_Z:  // X축 회전 + Y -90도 → X+ 방향으로 올라감
            vOffset = _vec3(fHorizontalDist, -fVerticalDist, 0.f);
            break;
        case SLOPE_NEGATIVE_Z:  // X축 회전 + Y 90도 → X- 방향으로 올라감
            vOffset = _vec3(fHorizontalDist, fVerticalDist, 0.f);
            break;
        }
    }

    return vPivotPos + vOffset;
}
