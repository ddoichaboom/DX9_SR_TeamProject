#include "CStateComponent.h"
#include "CState.h"
#include "CGameObject.h"

CStateComponent::CStateComponent(LPDIRECT3DDEVICE9 pGraphicDev)
	:CComponent(pGraphicDev), m_pCurState(nullptr), m_pOwner(nullptr)
{
}

CStateComponent::CStateComponent(const CStateComponent& rhs)
	:CComponent(rhs), m_pCurState(nullptr), m_pOwner(nullptr)
{
}

CStateComponent::~CStateComponent()
{
}

void CStateComponent::SetOnwer(CGameObject* _owner)
{
	if (m_pOwner) return;
	m_pOwner = _owner;
	m_pOwner->AddRef();
}

void CStateComponent::ChangeState(CBaseState* _nextState)
{
	if (!m_pOwner || !_nextState || m_pCurState == _nextState) return;
	if (m_pCurState)
	{
		m_pCurState->End(m_pOwner);
	}
	m_pCurState = _nextState;
	m_pCurState->Begin(m_pOwner);
}


_int CStateComponent::Update_Component(const _float& fTimeDelta)
{
	if (!m_pCurState || !m_pOwner) return 0;
	m_pCurState->Update(m_pOwner);
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
	if(m_pOwner) m_pOwner->Release();
	CComponent::Free();
}