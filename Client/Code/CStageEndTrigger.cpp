#include "pch.h"
#include "CStageEndTrigger.h"
#include "CEventMgr.h"

CStageEndTrigger::CStageEndTrigger(LPDIRECT3DDEVICE9 pGraphicDev)
	:CTrigger(pGraphicDev)
{
}

CStageEndTrigger::CStageEndTrigger(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale)
	:CTrigger(pGraphicDev, vPos, vScale)
{
}

CStageEndTrigger::~CStageEndTrigger()
{
}

HRESULT CStageEndTrigger::Ready_GameObject()
{
	HRESULT hr = CTrigger::Ready_GameObject();
	return hr;
}

_int CStageEndTrigger::Update_GameObject(const _float& fTimeDelta)
{
	if (IsDead())
		return RET_DEAD;
	_int iExit = CTrigger::Update_GameObject(fTimeDelta);
	return iExit;
}

void CStageEndTrigger::Render_GameObject()
{
	CTrigger::Render_GameObject();
}


CStageEndTrigger* CStageEndTrigger::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStageEndTrigger* pTrigger = new CStageEndTrigger(pGraphicDev);
	if (!pTrigger) return nullptr;
	if (FAILED(pTrigger->Ready_GameObject()))
	{
		Safe_Release(pTrigger);
		MSG_BOX("Stage End Trigger  Create Failed");
		return nullptr;
	}
	return pTrigger;
}

CStageEndTrigger* CStageEndTrigger::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale)
{
	CStageEndTrigger* pTrigger = new CStageEndTrigger(pGraphicDev, vPos, vScale);
	if (!pTrigger) return nullptr;
	if (FAILED(pTrigger->Ready_GameObject()))
	{
		Safe_Release(pTrigger);
		MSG_BOX("Stage End Trigger  Create Failed");
		return nullptr;
	}
	return pTrigger;
}

void CStageEndTrigger::OnBeginCollision()
{
	CEventMgr::GetInstance()->Broadcast(EVENT_STAGE_END, nullptr);
	SetDead();
}

void CStageEndTrigger::Free()
{
	CTrigger::Free();
}
