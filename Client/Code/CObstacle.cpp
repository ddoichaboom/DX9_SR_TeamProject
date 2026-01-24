#include "pch.h"
#include "CObstacle.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CObstacle::CObstacle(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
    , m_pBufferCom(nullptr)
    , m_pTransformCom(nullptr)
    , m_pTextureCom(nullptr)
{
    m_eOBJ_ID = OBJ_OBSTACLE;
    m_iID = Make_ID();
}

CObstacle::CObstacle(const CObstacle& rhs)
    : CGameObject(rhs)
    , m_pBufferCom(nullptr)
    , m_pTransformCom(nullptr)
    , m_pTextureCom(nullptr)
{
    m_eOBJ_ID = rhs.m_eOBJ_ID;
    m_iID = Make_ID();          // 복사생성자는 어떻게?
}

CObstacle::~CObstacle()
{
}

HRESULT CObstacle::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

_int CObstacle::Update_GameObject(const _float& fTimeDelta)
{
    if (IsDead()) 
        return RET_DEAD;

    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    return iExit;
}

void CObstacle::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CObstacle::Render_GameObject()
{
    //m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetTexture(0, nullptr);
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    if (FAILED(Ready_Material()))
        return;

    // 텍스처가 있으면 렌더링 
    //if (m_pTextureCom)
    //    m_pTextureCom->Set_Texture(0);    

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
}

void CObstacle::SetPos(_vec3 _pos)
{
    if (m_pTransformCom)
    {
        m_pTransformCom->Set_Pos(_pos);
        //m_pTransformCom->Update_Component(0.f);
    }
}

void CObstacle::SetAngle(_vec3 _rot)
{
    if (m_pTransformCom)
    {
        m_pTransformCom->Set_Angle(_rot);
        //m_pTransformCom->Update_Component(0.f);
    }
}

void CObstacle::SetScale(_vec3 _scale)
{
    if (m_pTransformCom)
    {
        m_pTransformCom->Set_Scale(_scale);
        //m_pTransformCom->Update_Component(0.f);
    }
}

HRESULT CObstacle::Add_Component()
{
    Engine::CComponent* pComponent = nullptr;

    // CubeTex
    pComponent = m_pBufferCom = dynamic_cast<Engine::CCubeTex*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CubeTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Transform", pComponent });

    // Texture (선택사항 - Phase 7에서 추가)
    // pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
    //     (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_ObstacleTexture"));
    // m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

HRESULT CObstacle::Ready_Material()
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
    tMtrl.Diffuse = D3DXCOLOR(0.8f, 0.3f, 0.3f, 1.f);

    // Specular: 반사광 (광택)
    tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

    // Ambient: 환경광 (어두운 부분 색상)
    tMtrl.Ambient = D3DXCOLOR(0.3f, 0.1f, 0.1f, 1.f);

    // Emissive: 발광 (자체 발광 없음)
    tMtrl.Emissive = D3DXCOLOR(0.8f, 0.3f, 0.3f, 1.f);

    // Power: 반사광 강도 (0 = 무광택)
    tMtrl.Power = 0.f;

    // DirectX 디바이스에 Material 설정
    m_pGraphicDev->SetMaterial(&tMtrl);

    return S_OK;
}

CObstacle* CObstacle::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CObstacle* pInstance = new CObstacle(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CObstacle Create Failed");
        return nullptr;
    }

    return pInstance;
}

CObstacle* CObstacle::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CObstacle* pObstacle = new CObstacle(pGraphicDev);

    if (FAILED(pObstacle->Ready_GameObject()))
    {
        Safe_Release(pObstacle);
        MSG_BOX("CObstacle Create Failed");
        return nullptr;
    }
    // Transform 설정 
    pObstacle->m_pTransformCom->Set_Pos(vPos);
    pObstacle->m_pTransformCom->Set_Angle(-90.f, 0.f, 0.f);

    pObstacle->m_pTransformCom->Update_Component(0.f);

    return pObstacle;
}

CObstacle* CObstacle::Create(LPDIRECT3DDEVICE9 pGraphicDev,
    _vec3 vPos, _vec3 vRot, _vec3 vScale)
{
    CObstacle* pObstacle = new CObstacle(pGraphicDev);

    if (FAILED(pObstacle->Ready_GameObject()))
    {
        Safe_Release(pObstacle);
        MSG_BOX("CObstacle Create Failed");
        return nullptr;
    }

    // Transform 설정 
    pObstacle->m_pTransformCom->Set_Pos(vPos);
    pObstacle->m_pTransformCom->Set_Angle(vRot.x, vRot.y, vRot.z);
    pObstacle->m_pTransformCom->Set_Scale(vScale.x, vScale.y, vScale.z);

    pObstacle->m_pTransformCom->Update_Component(0.f);

    return pObstacle;
}

void CObstacle::Free()
{
    //Safe_Release(m_pBufferCom);
    //Safe_Release(m_pTransformCom);
    //Safe_Release(m_pTextureCom);

    CGameObject::Free();
}