#include "pch.h"
#include "CTrigger.h"
#include "CManagement.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CTrigger::CTrigger(LPDIRECT3DDEVICE9 pGraphicDev)
    :CGameObject(pGraphicDev), m_pTransformCom(nullptr), m_pCollisionCom(nullptr)
    , m_pCollider(nullptr),m_vPos{}, m_vScale{1.f,1.f,1.f }, m_bCollision(false)
{
    m_eOBJ_ID = OBJ_TRIGGER;
}

CTrigger::CTrigger(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale)
    :CGameObject(pGraphicDev), m_pTransformCom(nullptr), m_pCollisionCom(nullptr), m_pCollider(nullptr),
    m_vPos(vPos), m_vScale(vScale), m_bCollision(false)
{
    m_eOBJ_ID = OBJ_TRIGGER;
}

CTrigger::~CTrigger()
{
}

HRESULT CTrigger::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pTransformCom->m_vScale = m_vScale;
    m_pTransformCom->Set_Pos(m_vPos);

    m_pCollider = m_pCollisionCom->CreateCollider(this, m_szColliderName);
    m_pCollider->Set_Scale(m_vScale);
    m_pCollider->BindFuncToCollision([&](CollisionInfo info)
        {
            OnCollision(info.pTarget);
        });
    //Transform -Static 
    m_pTransformCom->Update_Component(0.f);

    return S_OK;
}

_int CTrigger::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);
    if (!m_CollisionList.empty() && !m_bCollision)
    {
        m_CollisionList.clear();
        OnEndCollision();
    }

    //다음 프레임에서 충돌 처리 결과를 받기 위해 끝에서 초기화 
    m_bCollision = false;
    return iExit;
}

void CTrigger::Render_GameObject()
{
    m_pCollider->Render_GameObject();
}

HRESULT CTrigger::Add_Component()
{
    Engine::CComponent* pComponent = nullptr;
    pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Transform", pComponent });

    pComponent = m_pCollisionCom = dynamic_cast<Engine::CCollision*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collision"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collision", pComponent });

    return S_OK;
}

void CTrigger::SetPos(_vec3 _pos)
{
    m_pTransformCom->Set_Pos(_pos);
    m_pTransformCom->Update_Component(1.f);
}
void CTrigger::SetScale(_vec3 _scale)
{
    m_pTransformCom->Set_Scale(_scale);
    m_pTransformCom->Update_Component(1.f);
}

void CTrigger::Activate()
{
    CGameObject::Activate();
    if (m_pCollider) m_pCollider->OnCollision();
}

void CTrigger::Deactivate()
{
    CGameObject::Deactivate();
    if (m_pCollider) m_pCollider->OffCollision();
    m_CollisionList.clear();
    m_bCollision = false;
}

void CTrigger::OnBeginCollision()
{
    if (m_FuncBind_Begin) m_FuncBind_Begin();
}

void CTrigger::OnCollision(CGameObject* _other)
{
    //벡터 중복과는 상관없이 충돌 유무만 판단
    m_bCollision = true;

    if (m_CollisionList.empty())
    {
        OnBeginCollision();
    }
    else if (_other != nullptr)
    {
        //이미 벡터에 있으면 종료
        auto iter = find(m_CollisionList.begin(), m_CollisionList.end(), _other);
        if (iter != m_CollisionList.end()) return;
    }
    m_CollisionList.push_back(_other);
}


void CTrigger::OnEndCollision()
{
    if (m_FuncBind_End) m_FuncBind_End();
}

void CTrigger::Bind_OnBegin(function<void()> _func)
{
    m_FuncBind_Begin = _func;
}

void CTrigger::Bind_OnEnd(function<void()> _func)
{
    m_FuncBind_End = _func;
}


CTrigger* CTrigger::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CTrigger* pTrigger = new CTrigger(pGraphicDev);
    if (!pTrigger) return nullptr;
    if (FAILED(pTrigger->Ready_GameObject()))
    {
        Safe_Release(pTrigger);
        MSG_BOX("Trigger Man Create Failed");
        return nullptr;
    }
    return pTrigger;
}

CTrigger* CTrigger::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale)
{
    CTrigger* pTrigger = new CTrigger(pGraphicDev, vPos, vScale);
    if (!pTrigger) return nullptr;
    if (FAILED(pTrigger->Ready_GameObject()))
    {
        Safe_Release(pTrigger);
        MSG_BOX("Trigger Man Create Failed");
        return nullptr;
    }
    return pTrigger;
}

void CTrigger::Free()
{
    CGameObject::Free();
}


