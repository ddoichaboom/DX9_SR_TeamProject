#pragma once
#include "CBase.h"
#include "CComponent.h"

BEGIN(Engine)

class IBasePool;
class ENGINE_DLL CGameObject : public CBase
{
protected:
	explicit CGameObject(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject();

public:
	void Set_RoomIndex(_int iRoomIndex) { m_iRoomIndex = iRoomIndex; }
	_int Get_RoomIndex() const { return m_iRoomIndex; }

public:
	virtual	HRESULT						Ready_GameObject();
	virtual	_int						Update_GameObject(const _float& fTimeDelta);
	virtual	void						LateUpdate_GameObject(const _float& fTimeDelta);
	virtual	void						Render_GameObject() PURE;

public:
	CComponent*							Get_Component(COMPONENTID eID, const _tchar* pComponentTag);
	void								Compute_ViewZ(const _vec3* pPos);
	_float								Get_ViewZ()		{ return m_fViewZ; }

	bool								IsDead()		{ return m_bDead; }
	void								SetDead()		{ m_bDead = true; }

	//ObjectPool
	bool								IsActivate()	{ return m_bActivate; }
	virtual void						Activate();
	virtual void						Deactivate();

	_int								GetRoomIndex() { return m_iRoomIndex; }
	void								SetRoomIndex(_int _idx) { m_iRoomIndex = _idx; }

	OBJ_ID								GetOBJID()		{ return m_eOBJ_ID; }
	_uint								GetID()			{ return m_iID; }

	void								SetPool(IBasePool* _pPool) { m_pPool = _pPool; }
	IBasePool*							GetPool() { return m_pPool; }
	void								ReturnToPool();

	//TranformCom이 있는 하위 클래스에서 재정의하기 
	virtual void						SetPos(_vec3 _pos) {};
	virtual void						Rotate(ROTATION _Axis, _float _degree) {};

protected:
	virtual HRESULT						Add_Component() { return S_OK; }
	virtual _uint						Make_ID()
	{
		_uint ret = (((_uint)m_eOBJ_ID) << 16 | m_iCount);
		m_iCount++;
		return ret; 
	}

	virtual	 void						Free();
	CComponent* Find_Component(COMPONENTID eID, const _tchar* pComponentTag);

protected:
	map<const _tchar*, CComponent*>		m_mapComponent[ID_END];
	LPDIRECT3DDEVICE9					m_pGraphicDev;

	_float								m_fViewZ;
	bool								m_bDead;


	bool								m_bActivate;
	IBasePool*							m_pPool;
	OBJ_ID								m_eOBJ_ID;
	_uint								m_iID;
	static _uint						m_iCount;

	// 몇번 방에 속하는 객체인지 파악하기 위한 정보 
	_int								m_iRoomIndex;		// -1은 전역 오브젝트
};

END