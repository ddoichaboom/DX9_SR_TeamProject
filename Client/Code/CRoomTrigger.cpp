#include "pch.h"
#include "CRoomTrigger.h"
#include "CManagement.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CEventMgr.h"

CRoomTrigger::CRoomTrigger(LPDIRECT3DDEVICE9 pGraphicDev)
	:CTrigger(pGraphicDev)
{
}

CRoomTrigger::CRoomTrigger(LPDIRECT3DDEVICE9 pGraphicDev, _int _roomNum, _vec3 vPos, _vec3 vScale)
	:CTrigger(pGraphicDev, vPos, vScale)
{
	m_iRoomIndex = _roomNum;
}

CRoomTrigger::~CRoomTrigger()
{
}

HRESULT CRoomTrigger::Ready_GameObject()
{
	HRESULT  hr = CTrigger::Ready_GameObject();
	return hr;
}

_int CRoomTrigger::Update_GameObject(const _float& fTimeDelta)
{
	if (IsDead()) return RET_DEAD;
	_int iExit = CTrigger::Update_GameObject(fTimeDelta);
	return iExit;
}

void CRoomTrigger::Render_GameObject()
{
	CTrigger::Render_GameObject();
}

CRoomTrigger* CRoomTrigger::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRoomTrigger* pTrigger = new CRoomTrigger(pGraphicDev);
	if (!pTrigger) return nullptr;
	if (FAILED(pTrigger->Ready_GameObject()))
	{
		Safe_Release(pTrigger);
		MSG_BOX("Door Trigger Create Failed");
		return nullptr;
	}
	return pTrigger;
}

CRoomTrigger* CRoomTrigger::Create(LPDIRECT3DDEVICE9 pGraphicDev, _int _roomNum, _vec3 vPos, _vec3 vScale)
{
	CRoomTrigger* pTrigger = new CRoomTrigger(pGraphicDev, _roomNum, vPos, vScale);
	if (!pTrigger) return nullptr;
	if (FAILED(pTrigger->Ready_GameObject()))
	{
		Safe_Release(pTrigger);
		MSG_BOX("Door Trigger  Create Failed");
		return nullptr;
	}
	return pTrigger;
}


void CRoomTrigger::OnBeginCollision()
{
	m_EventData.value = m_iRoomIndex;
	CEventMgr::GetInstance()->Broadcast(EVENT_ROOM_CHANGE, &m_EventData);
	SetDead();
}

void CRoomTrigger::Free()
{
	CTrigger::Free();
}