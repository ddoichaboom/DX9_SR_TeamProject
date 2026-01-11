#include "pch.h"
#include "CCeiling.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "Engine_Enum.h"

vector<TextureSource> CCeiling::m_vTextureSource =
{
    { STATIC_CEILING, L"../Bin/Resource/Texture/Terrain/Floor/STATIC_FLOOR/FLOORS.dds", true, 0, 7, 7, {2.f, 2.f} },
};

vector<AnimationSource> CCeiling::m_vAnimSource =
{
};

CCeiling::CCeiling(LPDIRECT3DDEVICE9 pGraphicDev)
    : CTerrain(pGraphicDev)
{
    m_eTerrainType = TERRAIN_CEILING;
}

CCeiling::CCeiling(const CCeiling& rhs)
    : CTerrain(rhs)
{
    m_eTerrainType = TERRAIN_CEILING;
}

CCeiling::~CCeiling()
{
}

HRESULT CCeiling::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

_int CCeiling::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    return iExit;
}

void CCeiling::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CCeiling::Render_GameObject()
{
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    if (FAILED(Ready_Material(D3DXCOLOR(0.3f, 0.5f, 0.7f, 1.f))))
        return;

    if (m_bIsAnimated && m_pAnimationCom)
        m_pAnimationCom->Render_Animation();
    else
        m_pTextureCom->Render_Texture();

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

    m_pGraphicDev->SetTexture(0, nullptr);
}

HRESULT CCeiling::Add_Component()
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
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CeilingTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture", pComponent });

    // Animation
    pComponent = m_pAnimationCom = dynamic_cast<Engine::CAnimation*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CeilingAnimation"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });

    return S_OK;
}

void CCeiling::Set_CeilingType(_uint eCeilingType)
{
    if (eCeilingType >= (int)DC_START)
    {
        m_bIsAnimated = true;
        if (m_pAnimationCom)
            m_pAnimationCom->Change_Animation(eCeilingType);
    }
    else
    {
        m_bIsAnimated = false;
        if (m_pTextureCom)
        {
            m_pTextureCom->Change_Texture(eCeilingType);
            m_pTextureCom->Set_Frame(_vec2(m_iTextureIdx, 0));
        }
    }
}

CCeiling* CCeiling::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CCeiling* pInstance = new CCeiling(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CCeiling Create Failed");
        return nullptr;
    }

    return pInstance;
}

CCeiling* CCeiling::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CCeiling* pCeiling = new CCeiling(pGraphicDev);

    if (FAILED(pCeiling->Ready_GameObject()))
    {
        Safe_Release(pCeiling);
        MSG_BOX("CCeiling Create Failed");
        return nullptr;
    }


    // Transform 설정 
    pCeiling->m_pTransformCom->Set_Pos(vPos);
    pCeiling->m_pTransformCom->Set_Angle(90.f, 0.f, 0.f);

    pCeiling->m_pTransformCom->Update_Component(0.f);

    return pCeiling;
}

CCeiling* CCeiling::Create(LPDIRECT3DDEVICE9 pGraphicDev,
    _vec3 vPos, _vec3 vRot, _vec3 vScale)
{
    CCeiling* pCeiling = new CCeiling(pGraphicDev);

    if (FAILED(pCeiling->Ready_GameObject()))
    {
        Safe_Release(pCeiling);
        MSG_BOX("CCeiling Create Failed");
        return nullptr;
    }


    // Transform 설정 
    pCeiling->m_pTransformCom->Set_Pos(vPos);
    pCeiling->m_pTransformCom->Set_Angle(vRot.x, vRot.y, vRot.z);
    pCeiling->m_pTransformCom->Set_Scale(vScale.x, vScale.y, vScale.z);

    pCeiling->m_pTransformCom->Update_Component(0.f);

    return pCeiling;
}

void CCeiling::Free()
{
    CTerrain::Free();
}