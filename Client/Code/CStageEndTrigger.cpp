#include "pch.h"
#include "CStageEndTrigger.h"
#include "CEventMgr.h"
#include "CTransform.h"
#include "CManagement.h"

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
	MapEndData endData;
	endData.pos = *m_pTransformCom->Get_Info(INFO_POS);

	//문의 맵콜라이더위치로 잡을때 offset을 주는데 문의 방향이 모두 달라서 추가 연산함
	_matrix matView;
	_vec3 vLook{};
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);
	memcpy(&vLook, matView.m[INFO_LOOK], sizeof(_vec3));

	if (fabsf(vLook.x) < fabsf(vLook.z)) // 룩벡터가 z축에 가깝다면 = z축 방향 문이라면 
	{
		endData.pos.z += 12.f;
		endData.angleY = 0.f;
	}
	else
	{
		//endData.pos.x -= 5.f;
		endData.pos.x += 15.f;
		endData.angleY = 90.f;
	}

	CEventMgr::GetInstance()->Broadcast(EVENT_STAGE_END, &endData);
	SetDead();
}

void CStageEndTrigger::Free()
{
	CTrigger::Free();
}
