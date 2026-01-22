#include "pch.h"
#include "CDoorTrigger.h"
#include "CManagement.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CEventMgr.h"

CDoorTrigger::CDoorTrigger(LPDIRECT3DDEVICE9 pGraphicDev)
	:CTrigger(pGraphicDev)
{
}

CDoorTrigger::CDoorTrigger(LPDIRECT3DDEVICE9 pGraphicDev, _int _roomNum, _vec3 vPos, _vec3 vScale)
	:CTrigger(pGraphicDev, vPos, vScale)
{
	m_iRoomIndex = _roomNum;
}

CDoorTrigger::~CDoorTrigger()
{
}

HRESULT CDoorTrigger::Ready_GameObject()
{
	HRESULT  hr = CTrigger::Ready_GameObject();
	return hr;
}

_int CDoorTrigger::Update_GameObject(const _float& fTimeDelta)
{
	if (IsDead()) return RET_DEAD;
	_int iExit = CTrigger::Update_GameObject(fTimeDelta);
	return iExit;
}

void CDoorTrigger::Render_GameObject()
{
	CTrigger::Render_GameObject();
}

CDoorTrigger* CDoorTrigger::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CDoorTrigger* pTrigger = new CDoorTrigger(pGraphicDev);
	if (!pTrigger) return nullptr;
	if (FAILED(pTrigger->Ready_GameObject()))
	{
		Safe_Release(pTrigger);
		MSG_BOX("Door Trigger Create Failed");
		return nullptr;
	}
	return pTrigger;
}

CDoorTrigger* CDoorTrigger::Create(LPDIRECT3DDEVICE9 pGraphicDev, _int _roomNum, _vec3 vPos, _vec3 vScale)
{
	CDoorTrigger* pTrigger = new CDoorTrigger(pGraphicDev, _roomNum, vPos, vScale);
	if (!pTrigger) return nullptr;
	if (FAILED(pTrigger->Ready_GameObject()))
	{
		Safe_Release(pTrigger);
		MSG_BOX("Door Trigger  Create Failed");
		return nullptr;
	}
	return pTrigger;
}


void CDoorTrigger::OnBeginCollision()
{
	m_EventData.value = m_iRoomIndex;
	CEventMgr::GetInstance()->Broadcast(EVENT_DOOR_IN, &m_EventData);
	SetDead();
}

void CDoorTrigger::OnEndCollision()
{
	m_EventData.value = m_iRoomIndex;
	CEventMgr::GetInstance()->Broadcast(EVENT_DOOR_OUT, &m_EventData);
	SetDead();
}

void CDoorTrigger::Free()
{
	CTrigger::Free();
}