#include "pch.h"
#include "CEditorWall.h"
#include "CRenderer.h"

#include "CProtoMgr.h"
#include "CTransform.h"
#include "CRcTex.h"
#include "CTexture.h"

vector<TextureSource> CEditorWall::m_vTextureSource =
{
    {STATIC_WALL_1, L"../Bin/Resource/Texture/Terrain/Wall/STATIC_WALL/WALL_1.dds", true, 0, 2, 2, {0.f, 0.f}},
    {STATIC_WALL_2, L"../Bin/Resource/Texture/Terrain/Wall/STATIC_WALL/WALL_2.dds", true, 0, 2, 2, {0.f, 0.f}},
    {STATIC_WALL_3, L"../Bin/Resource/Texture/Terrain/Wall/STATIC_WALL/WALL_3.dds", true, 0, 2, 2, {0.f, 0.f}},
    {STATIC_WALL_4, L"../Bin/Resource/Texture/Terrain/Wall/STATIC_WALL/WALL_4.dds", true, 0, 2, 2, {0.f, 0.f}},
    {STATIC_WALL_5, L"../Bin/Resource/Texture/Terrain/Wall/STATIC_WALL/WALL_5.dds", true, 0, 2, 2, {0.f, 0.f}},
    {STATIC_WALL_6, L"../Bin/Resource/Texture/Terrain/Wall/STATIC_WALL/WALL_6.dds", true, 0, 2, 2, {0.f, 0.f}},
    {STATIC_WALL_7, L"../Bin/Resource/Texture/Terrain/Wall/STATIC_WALL/WALL_7.dds", true, 0, 2, 2, {0.f, 0.f}},
    {STATIC_WALL_8, L"../Bin/Resource/Texture/Terrain/Wall/STATIC_WALL/WALL_8.dds", true, 0, 2, 2, {0.f, 0.f}},
    {STATIC_WALL_9, L"../Bin/Resource/Texture/Terrain/Wall/STATIC_WALL/WALL_9.dds", true, 0, 2, 2, {0.f, 0.f}},
    {STATIC_WALL_10, L"../Bin/Resource/Texture/Terrain/Wall/STATIC_WALL/WALL_10.dds", true, 0, 2, 2, {0.f, 0.f}},
    {STATIC_WALL_WATER, L"../Bin/Resource/Texture/Terrain/Wall/STATIC_WALL/WALL_WATER.dds", true, 0, 1, 1, {0.f, 0.f}},
    {STATIC_WALL_LAVA, L"../Bin/Resource/Texture/Terrain/Wall/STATIC_WALL/WALL_LAVA.dds", true, 0, 1, 1, {0.f, 0.f}},
    {STATIC_WALL_ACID, L"../Bin/Resource/Texture/Terrain/Wall/STATIC_WALL/WALL_ACID.dds", true, 0, 1, 1, {0.f, 0.f}},
    {STATIC_WALL_FENCE, L"../Bin/Resource/Texture/Terrain/Wall/STATIC_WALL/WALL_FENCE.dds", false, 0, 0, 0, {0.f, 0.f}}
};

CEditorWall::CEditorWall(LPDIRECT3DDEVICE9 pGraphicDev)
    : CEditorObject(pGraphicDev)
    , m_eWallDir(WALL_XY_FRONT)
    , m_iTextureIdx(0)
    , m_iWallType(STATIC_WALL_1)
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
    // 텍스처 스테이트 저장 
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

    if (m_bSelected)
    {
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
        m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 255, 100));  // 노란색 tint
    }

    m_pBufferCom->Render_Buffer();

    // 텍스처 스테이트 복원
    m_pGraphicDev->SetTexture(0, nullptr);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, dwOldColorOp);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, dwOldColorArg1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, dwOldColorArg2);
    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwOldTextureFactor);

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
}

void CEditorWall::Set_WallDirection(WALL_DIR eDir)
{
    m_eWallDir = eDir;

    // 방향에 따라 회전 자동 설정
    if (m_eWallDir == WALL_XY_FRONT)
    {
        // XY 평면: 정면 벽 (회전 없음)
        Set_Rotation(_vec3(0.f, 0.f, 0.f));
    }
    else if (m_eWallDir == WALL_XY_BACK)
    {
        Set_Rotation(_vec3(180.f, 0.f, 180.f));
    }
    else if (m_eWallDir == WALL_YZ_LEFT)
    {
        // YZ 평면: 측면 벽 (Y축 90도 회전)
        Set_Rotation(_vec3(0.f, -90.f, 0.f));
    }
    else if (m_eWallDir == WALL_YZ_RIGHT)
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

    pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Static_WallTexture"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[Engine::ID_DYNAMIC].insert({ L"Com_Texture", pComponent });

    if (m_pTextureCom)
    {
        m_pTextureCom->Change_Texture(m_iWallType);
        m_pTextureCom->Set_Frame(_vec2(m_iTextureIdx, 0));
    }

    return S_OK;
}

void CEditorWall::Set_WallType(_uint eWallType)
{
    m_iWallType = eWallType;

    if (m_pTextureCom)
    {
        m_pTextureCom->Change_Texture(m_iWallType);
        m_pTextureCom->Set_Frame(_vec2(m_iTextureIdx, 0));
    }
}

void CEditorWall::Set_TextureIdx(_int iIdx)
{
    m_iTextureIdx = iIdx;

    if (m_pTextureCom)
    {
        m_pTextureCom->Set_Frame(_vec2(m_iTextureIdx, 0));
    }
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
    pInstance->Set_Scale(_vec3(16.f, 16.f, 1.f));

    // 기본 방향: XY 평면
    pInstance->Set_WallDirection(WALL_XY_FRONT);

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
    pInstance->Set_Scale(_vec3(16.f, 16.f, 1.f));
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

CEditorWall* CEditorWall::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot, _vec3 vScale, WALL_DIR eDir, _uint iType, _int iIdx)
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
    pInstance->Set_WallType(iType);
    pInstance->Set_TextureIdx(iIdx);

    return pInstance;
}

void CEditorWall::Free()
{
    CEditorObject::Free();
}