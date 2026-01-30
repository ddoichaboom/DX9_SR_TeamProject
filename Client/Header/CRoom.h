#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "CGameObject.h"
#include "CTransform.h"


class CRoom : public CBase
{
public:
	explicit CRoom();
	virtual ~CRoom();

public:
	_int Update_Room(const _float& fTimeDelta);
	void LateUpdate_Room(const _float& fTimeDelta);

public:
	void PushObject(CGameObject* obj);

	//현 방의 다음 위치를 반환
	_float GetNextPosZ();

	void SetSize(_vec3 _size) { m_vSize = _size; }
	_vec3 GetSize() { return m_vSize; }

	void SetOriginZPos(_float _pos) { m_fZPos = _pos; }
	_float GetOriginZPos() { return m_fZPos; }

	void Set_Pos_Room(_float _posZ);
	void Move_Room(_float _delta) { m_fMoveDelta += _delta; }
	_float Get_MoveDelta() { return m_fMoveDelta; }

protected:
	void Free() override;

private:
	//vector<CGameObject*> m_vObjects;
	//vector<CTransform*> m_vObjectTranforms;

	vector <pair< CGameObject*, CTransform*>> m_vObjectInfo;
	_float m_fZPos;
	_vec3 m_vSize;
	_float m_fMoveDelta;



};
