#include "pch.h"
#include "CTerrain.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "Engine_Enum.h"


CTerrain::CTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
    , m_pBufferCom(nullptr)
    , m_pTransformCom(nullptr)
    , m_pTextureCom(nullptr)
    , m_pAnimationCom(nullptr)
    , m_eTerrainType(TERRAIN_END)
    , m_bIsAnimated(false)
    , m_iTextureIdx(0)
{
    m_eOBJ_ID = OBJ_TERRAIN;
    m_iID = Make_ID();
}

CTerrain::CTerrain(const CTerrain& rhs)
    : CGameObject(rhs)
    , m_pBufferCom(nullptr)
    , m_pTransformCom(nullptr)
    , m_pTextureCom(nullptr)
    , m_pAnimationCom(nullptr)
    , m_eTerrainType(rhs.m_eTerrainType)
    , m_bIsAnimated(false)
    , m_iTextureIdx(0)
{
    m_eOBJ_ID = rhs.m_eOBJ_ID;
    m_iID = Make_ID();
}

CTerrain::~CTerrain()
{
}

void CTerrain::SetPos(_vec3 _pos)
{
    if (nullptr == m_pTransformCom)
        return;

    m_pTransformCom->Set_Pos(_pos);
}

void CTerrain::SetAngle(_vec3 _rot)
{
    if (nullptr == m_pTransformCom)
        return;

    m_pTransformCom->Set_Angle(_rot);
}

void CTerrain::SetScale(_vec3 _scale)
{
    if (nullptr == m_pTransformCom)
        return;

    m_pTransformCom->Set_Scale(_scale);
}

HRESULT CTerrain::Ready_Material(const D3DXCOLOR& diffuse)
{
    D3DMATERIAL9 tMtrl;
    ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

    tMtrl.Diffuse = diffuse;

    tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

    tMtrl.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

    // 아직 조명 X 
    //tMtrl.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);

    tMtrl.Emissive = diffuse;
    tMtrl.Power = 0.f;

    m_pGraphicDev->SetMaterial(&tMtrl);

    return S_OK;
}

void CTerrain::Free()
{
    CGameObject::Free();
}