#pragma once
#include "CBase.h"
#include "CGameObject.h"
#define MapObjectType multimap<OBJ_ID, CGameObject*>
BEGIN(Engine)


class ENGINE_DLL CLayer : public CBase
{
private:
	explicit CLayer();
	virtual ~CLayer();

public:
	CComponent*		Get_Component(COMPONENTID eID, OBJ_ID _objID, const _tchar* pComponentTag);

	//키가 objID인 원소 중 첫번째 원소를 반환 
	CGameObject*	Get_Object(OBJ_ID _objID);

	//multimap에 키가 objID인 원소들을 범위로 반환하는 함수
	pair<MapObjectType::iterator, MapObjectType::iterator> Get_Objects(OBJ_ID _objID);

	HRESULT			Add_GameObject(CGameObject* pGameObject);

public:
	HRESULT			Ready_Layer();
	_int			Update_Layer(const _float& fTimeDelta);
	void			LateUpdate_Layer(const _float& fTimeDelta);

	//TEST
	bool			IsEmptyByOBJID(OBJ_ID _objID)
	{
		return m_mapObject.find(_objID) == m_mapObject.end();
	}
private:
	MapObjectType			m_mapObject;

public:
	static CLayer* Create();

private:
	virtual void	Free();
};

END