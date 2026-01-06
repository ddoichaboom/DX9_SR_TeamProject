#include "pch.h"
#include "CEditorWall.h"
#include "CRenderer.h"

#include "CProtoMgr.h"
#include "CTransform.h"
#include "CRcTex.h"


CEditorWall::CEditorWall(LPDIRECT3DDEVICE9 pGraphicDev)
    : CEditorObject(pGraphicDev)
    , m_eWallDir(WALL_XY)
{
}

CEditorWall::~CEditorWall()
{
}

HRESULT CEditorWall::Ready_GameObject()
{
    FAILED_CHECK_RETURN(CEditorObject::Ready_GameObject(), E_FAIL);
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    // 기본 이름
    m_wstrName = L"Wall";

    return S_OK;
}

_int CEditorWall::Update_GameObject(const _float& fTimeDelta)
{
    CEditorObject::Update_GameObject(fTimeDelta);

    // Renderer에 추가
    Engine::CRenderer::GetInstance()->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

    return 0;
}

void CEditorWall::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CEditorObject::LateUpdate_GameObject(fTimeDelta);
}

void CEditorWall::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    // 텍스처 설정 (기본 흰색)
    if (nullptr == m_pTextureCom)
    {
        m_pGraphicDev->SetTexture(0, nullptr);
    }
    else
    {
        //m_pTextureCom->Set_Texture(0);
    }

    // 선택 상태 표시 (벽은 회색 계열)
    if (m_bSelected)
    {
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
        m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 200, 100));  // 주황색 (선택됨)
    }
    else
    {
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
        m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 180, 180, 180));  // 회색 (벽)
    }

    m_pBufferCom->Render_Buffer();

    // 렌더 상태 복원
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
}

void CEditorWall::Set_WallDirection(WALL_DIR eDir)
{
    m_eWallDir = eDir;

    // 방향에 따라 회전 자동 설정
    if (m_eWallDir == WALL_XY)
    {
        // XY 평면: 정면 벽 (회전 없음)
        Set_Rotation(_vec3(0.f, 0.f, 0.f));
    }
    else if (m_eWallDir == WALL_YZ)
    {
        // YZ 평면: 측면 벽 (Y축 90도 회전)
        Set_Rotation(_vec3(0.f, 90.f, 0.f));
    }
}

HRESULT CEditorWall::Add_Component()
{
    Engine::CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[Engine::ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Buffer ( RcTex - Floor/Ceiling과 동일 )
    pComponent = m_pBufferCom = dynamic_cast<Engine::CVIBuffer*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[Engine::ID_STATIC].insert({ L"Com_Buffer", pComponent });

    return S_OK;
}

CEditorWall* CEditorWall::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CEditorWall* pInstance = new CEditorWall(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorWall Create Failed");
        return nullptr;
    }

    // 위치 설정
    pInstance->Set_Position(vPos);

    // 기본 크기 (벽은 세로로 큼)
    pInstance->Set_Scale(_vec3(8.f, 24.f, 1.f));

    // 기본 방향: XY 평면
    pInstance->Set_WallDirection(WALL_XY);

    return pInstance;
}

CEditorWall* CEditorWall::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, WALL_DIR eDir)
{
    CEditorWall* pInstance = new CEditorWall(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorWall Create Failed");
        return nullptr;
    }

    pInstance->Set_Position(vPos);
    pInstance->Set_Scale(_vec3(8.f, 24.f, 1.f));
    pInstance->Set_WallDirection(eDir);  // 방향 지정

    return pInstance;
}

CEditorWall* CEditorWall::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot, _vec3 vScale, WALL_DIR eDir)
{
    CEditorWall* pInstance = new CEditorWall(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorWall Create Failed");
        return nullptr;
    }

    // Transform 전체 지정 (맵 로드 시 사용)
    pInstance->Set_Scale(vScale);
    pInstance->Set_Rotation(vRot);
    pInstance->Set_Position(vPos);
    pInstance->m_eWallDir = eDir;  // 방향만 저장 (Set_WallDirection 호출 X, 회전은 이미 설정됨)

    return pInstance;
}

void CEditorWall::Free()
{
    CEditorObject::Free();
}