#include "CScene.h"
#include "CSoundMgr.h"
#include "CEventMgr.h"
CScene::CScene(LPDIRECT3DDEVICE9 pGraphicDev)
    : m_pGraphicDev(pGraphicDev)
{
    m_pGraphicDev->AddRef();
}

CScene::~CScene()
{
}

CComponent* CScene::Get_Component(COMPONENTID eID, const _tchar* pLayerTag, OBJ_ID _objID, const _tchar* pComponentTag)
{
    auto    iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(pLayerTag));

    if (iter == m_mapLayer.end())
        return nullptr;

    return iter->second->Get_Component(eID, _objID, pComponentTag);
}

CLayer* CScene::Get_Layer(const _tchar* pLayerTag)
{
    auto iter = m_mapLayer.find(pLayerTag);
    if (iter == m_mapLayer.end()) return nullptr;
    else return iter->second;
}

HRESULT CScene::Ready_Scene()
{
    return S_OK;
}

_int CScene::Update_Scene(const _float& fTimeDelta)
{
    for (auto& pLayer : m_mapLayer)
        pLayer.second->Update_Layer(fTimeDelta);

    return 0;
}

void CScene::LateUpdate_Scene(const _float& fTimeDelta)
{
    for (auto& pLayer : m_mapLayer)
        pLayer.second->LateUpdate_Layer(fTimeDelta);
}



void CScene::Free()
{
    for_each(m_mapLayer.begin(), m_mapLayer.end(), CDeleteMap());
    m_mapLayer.clear();


    Safe_Release(m_pGraphicDev);
}
