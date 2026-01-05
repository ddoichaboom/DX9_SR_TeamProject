#pragma once
#include "CBase.h"
#include "CGameObject.h"

BEGIN(Engine)


class ENGINE_DLL CLayer : public CBase
{
private:
	explicit CLayer();
	virtual ~CLayer();

public:
	CComponent*		Get_Component(COMPONENTID eID, OBJ_ID _objID, const _tchar* pComponentTag);
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
	multimap<OBJ_ID, CGameObject*>			m_mapObject;

public:
	static CLayer* Create();

private:
	virtual void	Free();
};

END