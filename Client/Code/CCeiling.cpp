#include "pch.h"
#include "CCeiling.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CCeiling::CCeiling(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
    , m_pBufferCom(nullptr)
    , m_pTransformCom(nullptr)
    , m_pTextureCom(nullptr)
{
}

CCeiling::CCeiling(const CCeiling& rhs)
    : CGameObject(rhs)
    , m_pBufferCom(nullptr)
    , m_pTransformCom(nullptr)
    , m_pTextureCom(nullptr)
{
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
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    if (FAILED(Ready_Material()))
        return;

    // 텍스처가 있으면 렌더링 
    if (m_pTextureCom)
        m_pTextureCom->Set_Texture(0);

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
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

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Texture (선택사항 - Phase 7에서 추가)
    // pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
    //     (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CeilingTexture"));
    // m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

HRESULT CCeiling::Ready_Material()
{
    D3DMATERIAL9 tMtrl;
    ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

    //tMtrl.Diffuse = D3DXCOLOR(0.9f, 0.9f, 1.f, 1.f);
    //tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
    //tMtrl.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
    //tMtrl.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
    tMtrl.Diffuse = D3DXCOLOR(0.3f, 0.5f, 0.7f, 1.f);
    tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
    tMtrl.Ambient = D3DXCOLOR(0.1f, 0.2f, 0.3f, 1.f);
    tMtrl.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
    tMtrl.Power = 0.f;

    m_pGraphicDev->SetMaterial(&tMtrl);

    return S_OK;
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
    //Safe_Release(m_pBufferCom);
    //Safe_Release(m_pTransformCom);
    //Safe_Release(m_pTextureCom);

    CGameObject::Free();
}