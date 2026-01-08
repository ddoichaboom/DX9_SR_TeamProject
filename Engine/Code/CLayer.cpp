#include "CLayer.h"
#include "CObjectPool.h"

CLayer::CLayer()
{
}

CLayer::~CLayer()
{
}

CComponent* CLayer::Get_Component(COMPONENTID eID, OBJ_ID _objID, const _tchar* pComponentTag)
{
	auto iter = m_mapObject.find(_objID);

	if (iter == m_mapObject.end())
		return nullptr;
	//multimap 이니까 여러 원소들 중 첫 원소만 반환 
	return iter->second->Get_Component(eID, pComponentTag);
}

CGameObject* CLayer::Get_Object(OBJ_ID _objID)
{
	auto iter = m_mapObject.find(_objID);
	if (iter == m_mapObject.end()) return nullptr;
	return iter->second;
}

pair<MapObjectType::iterator, MapObjectType::iterator> CLayer::Get_Objects(OBJ_ID _objID)
{
	return m_mapObject.equal_range(_objID);
}


HRESULT CLayer::Add_GameObject(CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_mapObject.insert({ pGameObject->GetOBJID(), pGameObject});

	return S_OK;
}

HRESULT CLayer::Ready_Layer()
{
	return S_OK;
}

_int CLayer::Update_Layer(const _float& fTimeDelta)
{ 
	_int	iResult(0);
	for (auto iter = m_mapObject.begin(); iter != m_mapObject.end();)
	{
		_int iRet = iter->second->Update_GameObject(fTimeDelta);

		if (iRet == RET_DEAD)
		{
			//Pool에 있던 객체라면 되돌려줌
			IBasePool* pool = iter->second->GetPool();
			if (pool == nullptr) Safe_Release(iter->second);
			else iter->second->ReturnToPool();
			iter = m_mapObject.erase(iter);
		}
		else iter++;
	}

	return iResult;
}

void CLayer::LateUpdate_Layer(const _float& fTimeDelta)
{
	for (auto& pObj : m_mapObject)
		pObj.second->LateUpdate_GameObject(fTimeDelta);
}




CLayer* CLayer::Create()
{
	CLayer* pLayer = new CLayer;

	if (FAILED(pLayer->Ready_Layer()))
	{
		MSG_BOX("Layer Create Failed");
		Safe_Release(pLayer);
		return nullptr;
	}

	return pLayer;
}

void CLayer::Free()
{
	for (auto iter = m_mapObject.begin(); iter != m_mapObject.end(); iter++)
	{
		IBasePool* pool = iter->second->GetPool();
		//Pool 오브젝트들은 Pool에서 해제함 
		if (pool) iter->second->ReturnToPool();
		else Safe_Release(iter->second);
	}
	m_mapObject.clear();
}
