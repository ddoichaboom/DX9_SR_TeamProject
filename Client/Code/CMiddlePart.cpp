#include "pch.h"
#include "CMiddlePart.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"

#include "CPlayer.h"

CMiddlePart::CMiddlePart(LPDIRECT3DDEVICE9 pGraphicDev)
	: CPlayerPart(pGraphicDev), m_eNowState(MS_UNACTIVE)
{
}

CMiddlePart::CMiddlePart(const CMiddlePart& rhs)
	: CPlayerPart(rhs), m_eNowState(MS_UNACTIVE)
{
}

CMiddlePart::~CMiddlePart()
{
}

void CMiddlePart::Change_State(_uint iStateNum)
{
	m_eNowState = static_cast<MIDDLE_STATE>(iStateNum);
	if (m_eNowState == MS_UNACTIVE)
		return;

	m_pAnimationCom->Change_Animation(iStateNum);
}

HRESULT CMiddlePart::Ready_GameObject()
{
	if (FAILED(Add_Component())) 
		return E_FAIL;

	m_pTransformCom->m_vScale = { 256.f, 256.f, 1.f };

	m_vStartPos = { 0.f, WINCY * -0.5f + 200.f, 0.f };

	m_pTransformCom->Set_Pos(m_vStartPos);


	return S_OK;
}

_int CMiddlePart::Update_GameObject(const _float& fTimeDelta)
{
	if (m_eNowState == MS_UNACTIVE)
		return 0;

	int iExit = CCharacter::Update_GameObject(fTimeDelta);

	switch (m_eNowState)
	{
	case MS_KICK:
		Update_Kick(fTimeDelta);
		break;

	case MS_SODA:
		Update_Drink(fTimeDelta);
		break;
	default:
		break;
	}


	return iExit;
}

void CMiddlePart::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_eNowState == MS_UNACTIVE)
		return;

	CCharacter::LateUpdate_GameObject(fTimeDelta);
}

void CMiddlePart::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pAnimationCom->Render_Animation();
	m_pBufferCom->Render_Buffer();
	m_pAnimationCom->LateRender_Animation();
}

HRESULT CMiddlePart::Add_Component()
{
	if (FAILED(CCharacter::Add_Component()))
		return E_FAIL;

	Engine::CComponent* pComponent = nullptr;

	pComponent = m_pAnimationCom = dynamic_cast<Engine::CAnimation*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_MiddleAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });
	return S_OK;
}

CMiddlePart* CMiddlePart::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMiddlePart* pPart = new CMiddlePart(pGraphicDev);

	if (FAILED(pPart->Ready_GameObject()))
	{
		Safe_Release(pPart);
		MSG_BOX("MiddlePart Part Create Failed");
		return nullptr;
	}

	return pPart;
}

void CMiddlePart::Free()
{
	CCharacter::Free();
}

void CMiddlePart::Update_Kick(const _float& fTimeDelta)
{

	if (m_pAnimationCom->IsEnd())
	{
		Change_State(MS_UNACTIVE);
		return;
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
}

void CMiddlePart::Update_Drink(const _float& fTimeDelta)
{

	if (m_pAnimationCom->IsEnd())
	{
		Change_State(MS_UNACTIVE);
		return;
	}
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
}

void CMiddlePart::Update_Slide(const _float& fTimeDelta)
{
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
}
