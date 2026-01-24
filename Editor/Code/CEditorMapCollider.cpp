#include "pch.h"
#include "CEditorMapCollider.h"
#include "CRenderer.h"
#include "CProtoMgr.h"
#include "CTransform.h"
#include "CCollision.h"
#include "CCollider.h"

CEditorMapCollider::CEditorMapCollider(LPDIRECT3DDEVICE9 pGraphicDev)
    : CEditorObject(pGraphicDev)
    , m_pCollisionCom(nullptr)
    , m_pCollider(nullptr)
    , m_vColliderScale(8.f, 8.f, 8.f)  // 기본 크기
{
}

CEditorMapCollider::CEditorMapCollider(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale)
    : CEditorObject(pGraphicDev)
    , m_pCollisionCom(nullptr)
    , m_pCollider(nullptr)
    , m_vColliderScale(vScale)
{
}

CEditorMapCollider::~CEditorMapCollider()
{
}

HRESULT CEditorMapCollider::Ready_GameObject()
{
    // 부모 클래스 초기화 (m_pTransformCom 생성)
    FAILED_CHECK_RETURN(CEditorObject::Ready_GameObject(), E_FAIL);
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_wstrName = L"MapCollider";

    // Transform 초기화 (Client와 동일하게 Scale은 1로 설정)
    m_pTransformCom->m_vScale = { 1.f, 1.f, 1.f };

    // CCollision을 통해 CCollider 생성 (Client 구조와 동일)
    m_pCollider = m_pCollisionCom->CreateCollider(this, m_szColliderName);
    if (m_pCollider)
    {
        m_pCollider->Set_Scale(m_vColliderScale);
    }

    // Transform 업데이트
    m_pTransformCom->Update_Component(0.f);

    return S_OK;
}

_int CEditorMapCollider::Update_GameObject(const _float& fTimeDelta)
{
    // 부모 업데이트
    CEditorObject::Update_GameObject(fTimeDelta);

    // Collision 컴포넌트 업데이트 (Collider 위치 동기화)
    if (m_pCollisionCom)
    {
        m_pCollisionCom->Update_Component(fTimeDelta);
    }

    // Alpha 그룹에서 렌더링 (항상 보이도록)
    Engine::CRenderer::GetInstance()->Add_RenderGroup(Engine::RENDER_DEBUG, this);

    return 0;
}

void CEditorMapCollider::LateUpdate_GameObject(const _float& fTimeDelta)
{
    OutputDebugStringA("CEditorMapCollider::LateUpdate called\n");

    CEditorObject::LateUpdate_GameObject(fTimeDelta);

    if (m_pCollisionCom)
    {
        OutputDebugStringA("m_pCollisionCom->LateUpdate_Component() calling\n");
        m_pCollisionCom->LateUpdate_Component();
    }
    else
    {
        OutputDebugStringA("m_pCollisionCom is nullptr!\n");
    }
}

void CEditorMapCollider::Render_GameObject()
{
    // CCollider에 렌더링 위임 (Client 구조와 동일)
    // CCollider 내부에서 CCubeCol로 와이어프레임 렌더링
    if (m_pCollider)
    {
        m_pCollider->Render_GameObject();
    }
}

HRESULT CEditorMapCollider::Add_Component()
{
    Engine::CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[Engine::ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Collision 컴포넌트 추가 (ID_DYNAMIC)
    pComponent = m_pCollisionCom = dynamic_cast<Engine::CCollision*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collision"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[Engine::ID_DYNAMIC].insert({ L"Com_Collision", pComponent });

    // Buffer와 Texture는 사용하지 않음 (Collider 내부에서 처리)
    m_pBufferCom = nullptr;
    m_pTextureCom = nullptr;

    return S_OK;
}

void CEditorMapCollider::Set_ColliderScale(_vec3 vScale)
{
    m_vColliderScale = vScale;
    if (m_pCollider)
    {
        m_pCollider->Set_Scale(vScale);
    }
    
}

CEditorMapCollider* CEditorMapCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CEditorMapCollider* pInstance = new CEditorMapCollider(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorMapCollider Create Failed");
        return nullptr;
    }

    // 위치 설정
    pInstance->Set_Position(vPos);

    // 기본 Collider 크기
    pInstance->Set_ColliderScale(_vec3(8.f, 8.f, 8.f));

    return pInstance;
}

CEditorMapCollider* CEditorMapCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale)
{
    CEditorMapCollider* pInstance = new CEditorMapCollider(pGraphicDev, vPos, vScale);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorMapCollider Create Failed");
        return nullptr;
    }

    // 위치 설정
    pInstance->Set_Position(vPos);

    // Collider 크기 설정
    pInstance->Set_ColliderScale(vScale);

    return pInstance;
}

void CEditorMapCollider::Free()
{
    CEditorObject::Free();
}