#include "pch.h"
#include "CEditorTriggerBox.h"
#include "CRenderer.h"
#include "CProtoMgr.h"
#include "CTransform.h"
#include "CCollision.h"
#include "CCollider.h"

CEditorTriggerBox::CEditorTriggerBox(LPDIRECT3DDEVICE9 pGraphicDev)
    : CEditorObject(pGraphicDev)
    , m_pCollisionCom(nullptr)
    , m_pCollider(nullptr)
    , m_vColliderScale(8.f, 8.f, 8.f)
    , m_eTriggerType(TRIGGER_ROOM_CHANGE)
{
}

CEditorTriggerBox::CEditorTriggerBox(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale)
    : CEditorObject(pGraphicDev)
    , m_pCollisionCom(nullptr)
    , m_pCollider(nullptr)
    , m_vColliderScale(vScale)
    , m_eTriggerType(TRIGGER_ROOM_CHANGE)
{
}

CEditorTriggerBox::~CEditorTriggerBox()
{
}

HRESULT CEditorTriggerBox::Ready_GameObject()
{
    FAILED_CHECK_RETURN(CEditorObject::Ready_GameObject(), E_FAIL);
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_wstrName = L"TriggerBox";

    // Transform 초기화 (Client와 동일)
    m_pTransformCom->m_vScale = { 1.f, 1.f, 1.f };

    // CCollision을 통해 CCollider 생성
    m_pCollider = m_pCollisionCom->CreateCollider(this, m_szColliderName);
    if (m_pCollider)
    {
        m_pCollider->Set_Scale(m_vColliderScale);
    }

    m_pTransformCom->Update_Component(0.f);

    return S_OK;
}

_int CEditorTriggerBox::Update_GameObject(const _float& fTimeDelta)
{
    CEditorObject::Update_GameObject(fTimeDelta);

    if (m_pCollisionCom)
    {
        m_pCollisionCom->Update_Component(fTimeDelta);
    }

    Engine::CRenderer::GetInstance()->Add_RenderGroup(Engine::RENDER_DEBUG, this);

    if (m_pCollider)
    {
        m_pCollider->Set_Selected(m_bSelected);
    }

    return 0;
}

void CEditorTriggerBox::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CEditorObject::LateUpdate_GameObject(fTimeDelta);

    if (m_pCollisionCom)
    {
        m_pCollisionCom->LateUpdate_Component();
    }
}

void CEditorTriggerBox::Render_GameObject()
{
}

HRESULT CEditorTriggerBox::Add_Component()
{
    Engine::CComponent* pComponent = nullptr;
    
    // Transform
    pComponent = m_pTransformCom = static_cast<Engine::CTransform*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[Engine::ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Collision 컴포넌트 추가 (ID_DYNAMIC)
    pComponent = m_pCollisionCom = static_cast<Engine::CCollision*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collision"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[Engine::ID_DYNAMIC].insert({ L"Com_Collision", pComponent });

    m_pBufferCom = nullptr;
    m_pTextureCom = nullptr;

    return S_OK;
}

void CEditorTriggerBox::Set_ColliderScale(_vec3 vScale)
{
    m_vColliderScale = vScale;
    if (m_pCollider)
    {
        m_pCollider->Set_Scale(vScale);
    }

}

CEditorTriggerBox* CEditorTriggerBox::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CEditorTriggerBox* pInstance = new CEditorTriggerBox(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorTriggerBox Create Failed");
        return nullptr;
    }

    pInstance->Set_Position(vPos);
    pInstance->Set_ColliderScale(_vec3(8.f, 8.f, 8.f));
    pInstance->Set_TriggerType(TRIGGER_ROOM_CHANGE);

    return pInstance;
}

CEditorTriggerBox* CEditorTriggerBox::Create(LPDIRECT3DDEVICE9 pGraphicDev,
                                                _vec3 vPos,
                                                _vec3 vScale,
                                                TRIGGER_TYPE eType)
{
    CEditorTriggerBox* pInstance = new CEditorTriggerBox(pGraphicDev, vPos, vScale);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorTriggerBox Create Failed");
        return nullptr;
    }

    pInstance->Set_Position(vPos);
    pInstance->Set_ColliderScale(vScale);
    pInstance->Set_TriggerType(eType);

    return pInstance;
}

void CEditorTriggerBox::Free()
{
    CEditorObject::Free();
}