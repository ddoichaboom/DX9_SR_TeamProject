#include "pch.h"
#include "CWall.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "Engine_Enum.h"

vector<TextureSource> CWall::m_vTextureSource =
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
    {STATIC_WALL_10, L"../Bin/Resource/Texture/Terrain/Wall/STATIC_WALL/WALL_10.dds", true, 0, 2, 2, {0.f, 0.f}}

};

vector<AnimationSource> CWall::m_vAnimSource =
{
 
};

CWall::CWall(LPDIRECT3DDEVICE9 pGraphicDev)
    : CTerrain(pGraphicDev)
{
    m_eTerrainType = TERRAIN_WALL;
}

CWall::CWall(const CWall& rhs)
    : CTerrain(rhs)
{
    m_eTerrainType = TERRAIN_WALL;
}

CWall::~CWall()
{
}

HRESULT CWall::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

_int CWall::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    return iExit;
}

void CWall::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CWall::Render_GameObject()
{
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    if (FAILED(Ready_Material(D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f))))
        return;

    if (m_bIsAnimated && m_pAnimationCom)
        m_pAnimationCom->Render_Animation();
    else
        m_pTextureCom->Render_Texture();

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

    m_pGraphicDev->SetTexture(0, nullptr);
}

HRESULT CWall::Add_Component()
{
    Engine::CComponent* pComponent = nullptr;

    // RcTex
    pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Transform", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_WallTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture", pComponent });

    // Animation
    pComponent = m_pAnimationCom = dynamic_cast<Engine::CAnimation*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_WallAnimation"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });


    return S_OK;
}

void CWall::Set_WallType(_uint eWallType)
{
    if (eWallType >= (int)DW_START)
    {
        m_bIsAnimated = true;
        if (m_pAnimationCom)
            m_pAnimationCom->Change_Animation(eWallType);
    }
    else
    {
        m_bIsAnimated = false;
        if (m_pTextureCom)
        {
            m_pTextureCom->Change_Texture(eWallType);
            m_pTextureCom->Set_Frame(_vec2(m_iTextureIdx, 0));
        }
    }
}


CWall* CWall::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CWall* pInstance = new CWall(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CWall Create Failed");
        return nullptr;
    }

    return pInstance;
}

CWall* CWall::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CWall* pWall = new CWall(pGraphicDev);

    if (FAILED(pWall->Ready_GameObject()))
    {
        Safe_Release(pWall);
        MSG_BOX("CWall Create Failed");
        return nullptr;
    }

    // Transform 설정 
    pWall->m_pTransformCom->Set_Pos(vPos);

    pWall->m_pTransformCom->Update_Component(0.f);

    return pWall;
}

CWall* CWall::Create(LPDIRECT3DDEVICE9 pGraphicDev,
    _vec3 vPos, _vec3 vRot, _vec3 vScale)
{
    CWall* pWall = new CWall(pGraphicDev);

    if (FAILED(pWall->Ready_GameObject()))
    {
        Safe_Release(pWall);
        MSG_BOX("CWall Create Failed");
        return nullptr;
    }

    // Transform 설정 
    pWall->m_pTransformCom->Set_Pos(vPos);
    pWall->m_pTransformCom->Set_Angle(vRot.x, vRot.y, vRot.z);
    pWall->m_pTransformCom->Set_Scale(vScale.x, vScale.y, vScale.z);

    pWall->m_pTransformCom->Update_Component(0.f);

    return pWall;
}

void CWall::Free()
{
    CTerrain::Free();
}