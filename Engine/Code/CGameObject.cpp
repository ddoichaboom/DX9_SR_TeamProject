#include "CGameObject.h"
#include "CObjectPool.h"

_uint CGameObject::m_iCount = 0;

CGameObject::CGameObject(LPDIRECT3DDEVICE9 pGraphicDev)
    : m_pGraphicDev(pGraphicDev), m_fViewZ(0.f), m_bDead(false), m_bActivate(false)
    , m_eOBJ_ID(OBJ_END), m_iID(-1), m_pPool(nullptr)
{
    m_pGraphicDev->AddRef();
}

CGameObject::CGameObject(const CGameObject& rhs)
    : m_pGraphicDev(rhs.m_pGraphicDev), m_fViewZ(rhs.m_fViewZ), m_bDead(false), m_bActivate(false)
    , m_eOBJ_ID(OBJ_END), m_iID(-1), m_pPool(nullptr)
{
    m_pGraphicDev->AddRef();
}

CGameObject::~CGameObject()
{
}

CComponent* CGameObject::Get_Component(COMPONENTID eID, const _tchar* pComponentTag)
{
    CComponent* pComponent = Find_Component(eID, pComponentTag);

    if (nullptr == pComponent)
        return nullptr;

    return pComponent;
}

HRESULT CGameObject::Ready_GameObject()
{
    return S_OK;
}

_int CGameObject::Update_GameObject(const _float& fTimeDelta)
{
    for (auto& pComponent : m_mapComponent[ID_DYNAMIC])
        pComponent.second->Update_Component(fTimeDelta);

    return RET_NONE;
}

void CGameObject::LateUpdate_GameObject(const _float& fTimeDelta)
{
    for (auto& pComponent : m_mapComponent[ID_DYNAMIC])
        pComponent.second->LateUpdate_Component();
}

void CGameObject::Compute_ViewZ(const _vec3* pPos)
{
    _matrix     matCamWorld;
    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matCamWorld);
    D3DXMatrixInverse(&matCamWorld, 0, &matCamWorld);

    _vec3   vCamPos;
    memcpy(&vCamPos, &matCamWorld.m[3][0], sizeof(_vec3));

    _vec3      vDir = vCamPos - *pPos;

    m_fViewZ = D3DXVec3Length(&vDir);
}

void CGameObject::Activate()
{
    m_bActivate = true;
    m_bDead = false;
}

void CGameObject::Deactivate()
{
    m_bActivate = false;

    for (auto& pComponent : m_mapComponent[ID_DYNAMIC])
        pComponent.second->Reset();

    for (auto& pComponent : m_mapComponent[ID_STATIC])
        pComponent.second->Reset();
}

void CGameObject::ReturnToPool()
{
    if (!m_pPool) return;
    m_pPool->Return_Object(this);
}

CComponent* CGameObject::Find_Component(COMPONENTID eID, const _tchar* pComponentTag)
{
    auto        iter = find_if(m_mapComponent[eID].begin(),
                                m_mapComponent[eID].end(), 
                                 CTag_Finder(pComponentTag));

    if (iter == m_mapComponent[eID].end())
        return nullptr;

    return iter->second;
}

void CGameObject::Free()
{
    for (_uint i = 0; i < ID_END; ++i)
    {
        for_each(m_mapComponent[i].begin(), m_mapComponent[i].end(), CDeleteMap());
        m_mapComponent[i].clear();
    }

    Safe_Release(m_pGraphicDev);
}
