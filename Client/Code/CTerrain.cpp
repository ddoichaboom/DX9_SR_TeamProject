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
    , m_bIsAnimated(false)
    , m_iTextureIdx(0)
    , m_bIsBlocked(true)
    , m_eColliderTag(TAG_NONE)
    , m_pBufferUpCom(nullptr)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
    : CGameObject(rhs)
    , m_pBufferCom(nullptr)
    , m_pTransformCom(nullptr)
    , m_pTextureCom(nullptr)
    , m_bIsAnimated(false)
    , m_iTextureIdx(0)
    , m_bIsBlocked(true)
    , m_eColliderTag(TAG_NONE)
    , m_pBufferUpCom(nullptr)
{
}

CTerrain::~CTerrain()
{
}

_vec3 CTerrain::GetPos()
{
    if (nullptr == m_pTransformCom) return _vec3();
    return *m_pTransformCom->Get_Info(INFO_POS);
}

void CTerrain::SetPos(_vec3 _pos)
{
    if (nullptr == m_pTransformCom)
        return;

    m_pTransformCom->Set_Pos(_pos);
}

_vec3 CTerrain::GetScale()
{
    if (nullptr == m_pTransformCom) return _vec3();
    return m_pTransformCom->Get_Scale();
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

_int    CTerrain::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);
    return iExit;
}

HRESULT CTerrain::Add_Component()
{
    Engine::CComponent* pComponent = nullptr;

    // RcTex
    pComponent = m_pBufferCom = static_cast<Engine::CRcTex*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Transform
    pComponent = m_pTransformCom = static_cast<Engine::CTransform*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Transform", pComponent });

    return S_OK;
}

void CTerrain::Free()
{
    CGameObject::Free();
}