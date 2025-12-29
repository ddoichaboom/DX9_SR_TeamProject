#include "CStateComponent.h"
#include "CState.h"
#include "CGameObject.h"
#include "CDataMgr.h"

CStateComponent::CStateComponent(LPDIRECT3DDEVICE9 pGraphicDev)
	:CComponent(pGraphicDev), m_pCurState(nullptr), m_pOwner(nullptr), m_iCurStateID(0)
{
}

CStateComponent::CStateComponent(const CStateComponent& rhs)
	:CComponent(rhs), m_pCurState(nullptr), m_pOwner(nullptr), m_iCurStateID(0)
{
}

CStateComponent::~CStateComponent()
{
}

void CStateComponent::SetOnwer(CGameObject* _owner)
{
	if (m_pOwner) return;
	m_pOwner = _owner;
}


_int CStateComponent::Update_Component(const _float& fTimeDelta)
{
	if (!m_pCurState || !m_pOwner) return 0;
	m_pCurState->Update(m_pOwner);
	return 0;
}

CStateComponent* CStateComponent::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStateComponent* pStateComp = new CStateComponent(pGraphicDev);
	return pStateComp;
}

CComponent* CStateComponent::Clone()
{
	return new CStateComponent(*this);
}

void CStateComponent::Free()
{
	CComponent::Free();
}