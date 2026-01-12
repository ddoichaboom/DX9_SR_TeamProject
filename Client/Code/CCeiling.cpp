#include "pch.h"
#include "CCeiling.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "Engine_Enum.h"

vector<TextureSource> CCeiling::m_vTextureSource =
{
    { STATIC_CEILING, L"../Bin/Resource/Texture/Terrain/Floor/STATIC_FLOOR/FLOORS.dds", true, 0, 7, 7, {2.f, 2.f} },
};

CCeiling::CCeiling(LPDIRECT3DDEVICE9 pGraphicDev)
    : CTerrain(pGraphicDev)
{
    m_eOBJ_ID = OBJ_CEILING;
    m_iID = Make_ID();
}

CCeiling::CCeiling(const CCeiling& rhs)
    : CTerrain(rhs)
{
    m_eOBJ_ID = OBJ_CEILING;
    m_iID = Make_ID();
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

    if (!m_bIsAnimated && m_pTextureCom)
        m_pTextureCom->Render_Texture();

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

    m_pGraphicDev->SetTexture(0, nullptr);
}

HRESULT CCeiling::Add_Component()
{
    if (FAILED(CTerrain::Add_Component()))
        return E_FAIL;

    Engine::CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Static_CeilingTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

void CCeiling::Set_CeilingType(_uint eCeilingType)
{
    if (m_pTextureCom)
    {
        m_pTextureCom->Change_Texture(eCeilingType);
        m_pTextureCom->Set_Frame(_vec2(m_iTextureIdx, 0));
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

void CCeiling::Free()
{
    CTerrain::Free();
}