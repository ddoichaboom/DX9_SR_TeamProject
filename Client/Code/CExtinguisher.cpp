#include "pch.h"
#include "CExtinguisher.h"
#include "CProtoMgr.h"
#include "CPoolMgr.h"
#include "CRenderer.h"
#include "CEventMgr.h"


#include "CManagement.h"
#include "CBodyEmit.h"
#include "CExplosion.h"

TextureSource CExtinguisher::m_vTextureSource =
{
    0, L"../Bin/Resource/Texture/Object/InteractObject/Extinguisher.dds"
};


CExtinguisher::CExtinguisher(LPDIRECT3DDEVICE9 pGraphicDev)
    : CInteractObject(pGraphicDev), m_pMainCollider(nullptr), m_pExplosiveCollider(nullptr)
{
    m_eItemType = ITEM_EXTINGUISHER;

}

CExtinguisher::CExtinguisher(const CExtinguisher& rhs)
    : CInteractObject(rhs), m_pMainCollider(nullptr), m_pExplosiveCollider(nullptr)
{
    m_eItemType = ITEM_EXTINGUISHER;
}

CExtinguisher::~CExtinguisher()
{
}

CExtinguisher* CExtinguisher::Create(PDIRECT3DDEVICE9 pGraphicDev)
{
    CExtinguisher* pExtinguisher = new CExtinguisher(pGraphicDev);

    if (FAILED(pExtinguisher->Ready_GameObject()))
    {
        Safe_Release(pExtinguisher);
        MSG_BOX("Extinguisher Create Failed");
        return nullptr;
    }

    return pExtinguisher;
}

HRESULT CExtinguisher::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pTransformCom->m_vScale = { 2.f, 4.f, 1.f };

    //Collider 생성
    m_pMainCollider = m_pCollisionCom->CreateCollider(this, m_szMainColliderName);
    if (!m_pMainCollider) return E_FAIL;
    m_pMainCollider->Set_RelativePos(_vec3(0.f, 0.f, 0.f));
    m_pMainCollider->Set_Scale(_vec3(3.f, 5.f, 3.f));

    m_pMainCollider->BindFuncToCollision([&](CollisionInfo info)
        {
            OnCollision(info);
        });

    //Collider 생성
    m_pExplosiveCollider = m_pCollisionCom->CreateCollider(this, m_szExplosiveColliderName);
    if (!m_pExplosiveCollider) return E_FAIL;
    m_pExplosiveCollider->Set_RelativePos(_vec3(0.f, 0.f, 0.f));
    m_pExplosiveCollider->Set_Scale(_vec3(25.f, 25.f, 25.f));

    //m_pExplosiveCollider->BindFuncToCollision([&](CollisionInfo info)
    //    {
    //        OnCollision(info);
    //    });


    //m_pExplosiveCollider->OffCollision();
    m_pTextureCom->Change_Texture(m_iTextureID);
    return S_OK;
}

_int CExtinguisher::Update_GameObject(const _float& fTimeDelta)
{
    if (IsDead())
        return RET_DEAD;

    int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    _vec3 info;
    m_pTransformCom->Get_Info(INFO_POS, &info);
    Compute_ViewZ(&info);

    return iExit;
}

void CExtinguisher::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CInteractObject::LateUpdate_GameObject(fTimeDelta);
}

void CExtinguisher::Render_GameObject()
{
    CInteractObject::Render_GameObject();
}

HRESULT CExtinguisher::Add_Component()
{
    Engine::CComponent* pComponent = nullptr;

    //VIBuffer
    pComponent = m_pBufferCom = static_cast<Engine::CRcTex*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    if (nullptr == pComponent)
        return E_FAIL;

    //Transform
    pComponent = m_pTransformCom = static_cast<Engine::CTransform*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Transform", pComponent });

    //Texutre - 자식 클래스에서 생성
    pComponent = m_pTextureCom = static_cast<Engine::CTexture*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_ExtinguisherTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    // Collision
    pComponent = m_pCollisionCom = static_cast<Engine::CCollision*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collision"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collision", pComponent });

    return S_OK;
}

void CExtinguisher::Free()
{
    CInteractObject::Free();
}

void CExtinguisher::Activate()
{
    CInteractObject::Activate();
    m_pTextureCom->Change_Texture(0);
}

void CExtinguisher::Deactivate()
{
    CInteractObject::Deactivate();
}

void CExtinguisher::OnCollision(CollisionInfo info)
{
    if (info.fDamage > 0.f)
    {
        //m_pMainCollider->OffCollision();
        //m_pExplosiveCollider->OnCollision();
        CheckExplosiveCollision();
        DeadAction();
    }    
}

void CExtinguisher::DeadAction()
{
    CExplosion* exp = CPoolMgr::GetInstance()->Get_Object<CExplosion>();
    CBodyEmit* bodyEmit = CPoolMgr::GetInstance()->Get_Object<CBodyEmit>();
    if (exp)
    {
        CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer")->Add_GameObject(exp);
        exp->SetPos(*m_pTransformCom->Get_Info(INFO_POS));
        exp->SetSize({ 1.5f, 1.5f });
        exp->Reset();
    }

    if (bodyEmit)
    {
        CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer")->Add_GameObject(bodyEmit);
        bodyEmit->SetPos(*m_pTransformCom->Get_Info(INFO_POS));
        bodyEmit->Reset();
    }
    SetDead();
}

void CExtinguisher::CheckExplosiveCollision()
{
    list<pair<_float, CCollider*>> pickedList;
    CollisionInfo info = { this, {0,0,0}, 10.f, TAG_NONE };

    CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");
    if (!pLayer) return;

    auto pairIter = pLayer->Get_Objects(OBJ_MONSTER);
    for (auto iter = pairIter.first; iter != pairIter.second; iter++)
    {
        CCollision* pCollision = static_cast<CCollision*>(iter->second->Get_Component(ID_DYNAMIC, L"Com_Collision"));
        if (!pCollision) continue;

        CCollider* monCollider = pCollision->GetCollider();
        if (!monCollider) continue;

        bool bPicked = CCollision::CheckCollision(m_pExplosiveCollider, monCollider);
        if (bPicked)
        {
            pickedList.push_back({ iter->second->Get_ViewZ(),monCollider });
        }
    }    

    if (pickedList.empty())
        return;

    for (auto& obj : pickedList)
    {
        obj.second->Collision(info);
    }
}

