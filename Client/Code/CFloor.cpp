#include "pch.h"
#include "CFloor.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CFloor::CFloor(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
    , m_pBufferCom(nullptr)
    , m_pTransformCom(nullptr)
    , m_pTextureCom(nullptr)
{
}

CFloor::CFloor(const CFloor& rhs)
    : CGameObject(rhs)
    , m_pBufferCom(nullptr)
    , m_pTransformCom(nullptr)
    , m_pTextureCom(nullptr)
{
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
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    return iExit;
}

void CFloor::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CFloor::Render_GameObject()
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

HRESULT CFloor::Add_Component()
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
    //     (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_FloorTexture"));
    // m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

HRESULT CFloor::Ready_Material()
{
    D3DMATERIAL9 tMtrl;
    ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

    //// Diffuse: 확산광 (기본 색상) - 회색 바닥
    //tMtrl.Diffuse = D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.f);

    //// Specular: 반사광 (광택)
    //tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

    //// Ambient: 환경광 (어두운 부분 색상)
    //tMtrl.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);

    //// Emissive: 발광 (자체 발광 없음)
    //tMtrl.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);

    // Diffuse: 확산광 (기본 색상) - 회색 바닥
    tMtrl.Diffuse = D3DXCOLOR(0.6f, 0.4f, 0.2f, 1.f);

    // Specular: 반사광 (광택)
    tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

    // Ambient: 환경광 (어두운 부분 색상)
    tMtrl.Ambient = D3DXCOLOR(0.3f, 0.2f, 0.1f, 1.f);

    // Emissive: 발광 
    tMtrl.Emissive = D3DXCOLOR(0.6f, 0.4f, 0.2f, 1.f);

    // Power: 반사광 강도 (0 = 무광택)
    tMtrl.Power = 0.f;

    // DirectX 디바이스에 Material 설정
    m_pGraphicDev->SetMaterial(&tMtrl);

    return S_OK;
}

CFloor* CFloor::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CFloor* pFloor = new CFloor(pGraphicDev);

    if (FAILED(pFloor->Ready_GameObject()))
    {
        Safe_Release(pFloor);
        MSG_BOX("CFloor Create Failed");
        return nullptr;
    }


    // Transform 설정 
    pFloor->m_pTransformCom->Set_Pos(vPos);
    pFloor->m_pTransformCom->Set_Angle(-90.f, 0.f, 0.f);  

    pFloor->m_pTransformCom->Update_Component(0.f);

    return pFloor;
}

CFloor* CFloor::Create(LPDIRECT3DDEVICE9 pGraphicDev,
                        _vec3 vPos, _vec3 vRot, _vec3 vScale)
{
    CFloor* pFloor = new CFloor(pGraphicDev);

    if (FAILED(pFloor->Ready_GameObject()))
    {
        Safe_Release(pFloor);
        MSG_BOX("CFloor Create Failed");
        return nullptr;
    }

    // Transform 설정 
    pFloor->m_pTransformCom->Set_Pos(vPos);
    pFloor->m_pTransformCom->Set_Angle(vRot.x, vRot.y, vRot.z);  
    pFloor->m_pTransformCom->Set_Scale(vScale.x, vScale.y, vScale.z);  

    pFloor->m_pTransformCom->Update_Component(0.f);


    return pFloor;
}

void CFloor::Free()
{
    //Safe_Release(m_pBufferCom);
    //Safe_Release(m_pTransformCom);
    //Safe_Release(m_pTextureCom);

    CGameObject::Free();
}