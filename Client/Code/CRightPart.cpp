#include "pch.h"
#include "CRightPart.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CPlayer.h"

CRightPart::CRightPart(LPDIRECT3DDEVICE9 pGraphicDev)
	: CPlayerPart(pGraphicDev), m_eNowState(RS_UNACTIVE)
{
}

CRightPart::CRightPart(const CRightPart& rhs)
	: CPlayerPart(rhs), m_eNowState(RS_UNACTIVE)
{
}

CRightPart::~CRightPart()
{
}

void CRightPart::Change_State(_uint iStateNum)
{
	m_eNowState = static_cast<RIGHT_STATE>(iStateNum);
	if (m_eNowState == RS_UNACTIVE)
		return;

	m_pAnimationCom->Change_Animation(iStateNum);
}

HRESULT CRightPart::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->m_vScale = { 256.f, 256.f, 1.f };

	m_vStartPos = { WINCX * 0.5f - 200.f, WINCY * -0.5f + 200.f, 0.f };
	

	m_pTransformCom->Set_Pos(m_vStartPos);	

	return S_OK;
}

_int CRightPart::Update_GameObject(const _float& fTimeDelta)
{
	if (m_eNowState == RS_UNACTIVE)
		return 0;

	int iExit = CCharacter::Update_GameObject(fTimeDelta);

	switch (m_eNowState)
	{
	case CRightPart::RS_IDLE_PISTOL:
	case CRightPart::RS_IDLE_SHOTGUN:
		Update_Idle(fTimeDelta);
		break;
	case CRightPart::RS_ATTACK_PISTOL:
	case CRightPart::RS_ATTACK_SHOTGUN:
		Update_Attack(fTimeDelta);
		break;
	case CRightPart::RS_RELOAD_PISTOL:
	case CRightPart::RS_RELOAD_SHOTGUN:
		Update_Reload(fTimeDelta);
		break;						
	}

	//CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);


	return iExit;
}

void CRightPart::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_eNowState == RS_UNACTIVE)
		return;

	CCharacter::LateUpdate_GameObject(fTimeDelta);
}

void CRightPart::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pAnimationCom->Render_Animation();
	m_pBufferCom->Render_Buffer();
	m_pAnimationCom->LateRender_Animation();
}

HRESULT CRightPart::Add_Component()
{
	if (FAILED(CCharacter::Add_Component()))
		return E_FAIL;

	Engine::CComponent* pComponent = nullptr;

	pComponent = m_pAnimationCom = dynamic_cast<Engine::CAnimation*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RightAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });
	return S_OK;
}

CRightPart* CRightPart::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRightPart* pPart = new CRightPart(pGraphicDev);

	if (FAILED(pPart->Ready_GameObject()))
	{
		Safe_Release(pPart);
		MSG_BOX("Right Part Create Failed");
		return nullptr;
	}

	return pPart;
}

void CRightPart::Free()
{
	CCharacter::Free();
}

void CRightPart::Update_Idle(const float& fTimeDelta)
{	
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
}

void CRightPart::Update_Attack(const float& fTimeDelta)
{
	if (m_pAnimationCom->IsEnd())
	{
		Change_State(RS_IDLE_PISTOL);
		return;
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
}

void CRightPart::Update_Reload(const float& fTimeDelta)
{
	if (m_pAnimationCom->IsEnd())
	{
		Change_State(RS_IDLE_PISTOL);
		return;
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
}
