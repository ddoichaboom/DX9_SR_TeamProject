#include "pch.h"
#include "CFloor.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "Engine_Enum.h"

vector<TextureSource> CFloor::m_vTextureSource =
{
    { STATIC_FLOOR, L"../Bin/Resource/Texture/Terrain/Floor/STATIC_FLOOR/FLOORS.dds", true, 0, 7, 7, {2.f, 2.f}},
    { STATIC_FLOOR_FLUID, L"../Bin/Resource/Texture/Terrain/Floor/STATIC_FLOOR/FLOOR_FLUID.dds", true, 0, 2, 2, {0.f, 0.f}},
    { STATIC_FLOOR_SLOPE, L"../Bin/Resource/Texture/Terrain/Floor/STATIC_FLOOR/FLOOR_SLOPE.dds", false, 0, 0, 0, {1.f, 1.f}},
    { STATIC_FLOOR_ROAD, L"../Bin/Resource/Texture/Terrain/Floor/STATIC_FLOOR/FLOOR_ROAD.dds", true, 0, 3, 3, {1.f, 1.f}},
    {STATIC_FLOOR_UNIQUE, L"../Bin/Resource/Texture/Terrain/Floor/STATIC_FLOOR/UNIQUE_FLOOR.dds"}
};

CFloor::CFloor(LPDIRECT3DDEVICE9 pGraphicDev)
    : CTerrain(pGraphicDev)
    , m_iFloorType(0)
{
    m_eOBJ_ID = OBJ_FLOOR;
    m_iID = Make_ID();
}

CFloor::CFloor(const CFloor& rhs)
    : CTerrain(rhs)
    , m_iFloorType(rhs.m_iFloorType)
{
    m_eOBJ_ID = OBJ_FLOOR;
    m_iID = Make_ID();
}

CFloor::~CFloor()
{
}

HRESULT CFloor::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

_int CFloor::Update_GameObject(const _float& fTimeDelta)
{
    if (IsDead()) return RET_DEAD;

    _int iExit = CTerrain::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    return iExit;
}

void CFloor::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CFloor::Render_GameObject()
{
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    if (FAILED(Ready_Material(D3DXCOLOR(0.6f, 0.4f, 0.2f, 1.f))))
        return;

    if (!m_bIsAnimated && m_pTextureCom)
        m_pTextureCom->Render_Texture();

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

    m_pGraphicDev->SetTexture(0, nullptr);
}

HRESULT CFloor::Add_Component()
{
    if (FAILED(CTerrain::Add_Component()))
        return E_FAIL;

    Engine::CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = static_cast<Engine::CTexture*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Static_FloorTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

void CFloor::Set_FloorType(_uint eFloorType)
{
    m_iFloorType = eFloorType;

    if (m_pTextureCom)
    {
        m_pTextureCom->Change_Texture(eFloorType);
        m_pTextureCom->Set_Frame(_vec2(m_iTextureIdx, 0));
    }
}

CFloor* CFloor::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CFloor* pInstance = new CFloor(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CFloor Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CFloor::Free()
{
    CTerrain::Free();
}