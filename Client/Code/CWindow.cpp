#include "pch.h"
#include "CWindow.h"
#include "CProtoMgr.h"
#include "CPoolMgr.h"
#include "CRenderer.h"
#include "CEventMgr.h"
#include "CManagement.h"

vector<TextureSource> CWindow::m_vTextureSource =
{
    { GLASS_IDLE,  L"../Bin/Resource/Texture/Object/Window_Idle.dds" },
    { GLASS_DEAD,  L"../Bin/Resource/Texture/Object/Window_Dead.dds" },

};

vector<AnimationSource>  CWindow::m_vAnimSource =
{
    { GLASS_IDLE,1,1,1, true, 0.4f},
    { GLASS_DEAD,1,1,1, false, 0.8f, 1.f},

};



CWindow::CWindow(LPDIRECT3DDEVICE9 pGraphicDev)
    : CInteractObject(pGraphicDev)
    , m_pMainCollider(nullptr), m_pAnimationCom(nullptr), m_pStateCom(nullptr)
    , m_fTime(0.f)
{
    m_eItemType = ITEM_NONE;

}

CWindow::CWindow(const CWindow& rhs)
    : CInteractObject(rhs)
    , m_pMainCollider(nullptr), m_pAnimationCom(nullptr), m_pStateCom(nullptr)
    , m_fTime(0.f)
{
    m_eItemType = ITEM_NONE;
}

CWindow::~CWindow()
{
}

CWindow* CWindow::Create(PDIRECT3DDEVICE9 pGraphicDev)
{
    CWindow* pWindow = new CWindow(pGraphicDev);

    if (FAILED(pWindow->Ready_GameObject()))
    {
        Safe_Release(pWindow);
        MSG_BOX("Window Create Failed");
        return nullptr;
    }

    return pWindow;
}

void CWindow::CreateStateData()
{
    auto Mgr = CDataMgr<CWindow>::GetInstance();
    if (Mgr->IsStateEmpty() == false) return;

    CState<CWindow>* State = new CState<CWindow>(&CWindow::Begin_Idle, &CWindow::Idle, nullptr);
    Mgr->AddState(GLASS_IDLE, State);

    State = new CState<CWindow>(&CWindow::Begin_Dead, &CWindow::Dead, nullptr);
    Mgr->AddState(GLASS_DEAD, State);
}

HRESULT CWindow::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    CreateStateData();
    ChangeState(GLASS_IDLE);

    //Collider »ý¼º
    m_pMainCollider = m_pCollisionCom->CreateCollider(this, m_szMainColliderName);
    if (!m_pMainCollider) return E_FAIL;    
    m_pMainCollider->Set_Scale(_vec3(16.f, 16.f, 4.f));

    m_pMainCollider->BindFuncToCollision([&](CollisionInfo info)
        {
            OnCollision(info);
        });

    
    
    return S_OK;
}

_int CWindow::Update_GameObject(const _float& fTimeDelta)
{
    if (IsDead())
        return RET_DEAD;

    int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    _vec3 info;
    m_pTransformCom->Get_Info(INFO_POS, &info);
    Compute_ViewZ(&info);

    return iExit;
}

void CWindow::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
    m_pAnimationCom->Update_State(m_pStateCom->GetCurrentStateID());
}

void CWindow::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pAnimationCom->Render_Animation();
    m_pBufferCom->Render_Buffer();
}

HRESULT CWindow::Add_Component()
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

    // State Component
    pComponent = m_pStateCom = static_cast<Engine::CStateComponent*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_StateComponent"));

    m_pStateCom->SetOnwer(this);
    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_StateComponent", pComponent });

    //Animation
    pComponent = m_pAnimationCom = static_cast<Engine::CAnimation*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_WindowAnimation"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });

    // Collision
    pComponent = m_pCollisionCom = static_cast<Engine::CCollision*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collision"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collision", pComponent });

    return S_OK;
}

void CWindow::Free()
{
    CDataMgr<CWindow>::DestroyInstance();
    CInteractObject::Free();
}

void CWindow::Activate()
{
    CInteractObject::Activate();
}

void CWindow::Deactivate()
{
    CInteractObject::Deactivate();
}

void CWindow::ChangeState(_uint nextStateID)
{
    m_fTime = 0.f;
    m_pAnimationCom->Update_State(nextStateID);
    m_pStateCom->ChangeState<CWindow>(nextStateID);
}

void CWindow::OnCollision(CollisionInfo info)
{
    if (info.fDamage > 0.f && m_pAnimationCom->Get_State() == GLASS_IDLE)
    {
        //DeadAction();
        ChangeState(GLASS_DEAD);
    }
}

void CWindow::DeadAction()
{
    
    SetDead();
}

void CWindow::Begin_Idle()
{
}

void CWindow::Idle()
{
}

void CWindow::Begin_Dead()
{
}

void CWindow::Dead()
{
}

