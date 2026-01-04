#include "pch.h"
#include "CLeftPart.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"

CLeftPart::CLeftPart(LPDIRECT3DDEVICE9 pGraphicDev)
	:	CPlayerPart(pGraphicDev), m_eNowState(LS_UNACTIVE)
{
}

CLeftPart::CLeftPart(const CLeftPart& rhs)
	:	CPlayerPart(rhs), m_eNowState(LS_UNACTIVE)
{
}

CLeftPart::~CLeftPart()
{
}

void CLeftPart::Change_State(_uint iStateNum)
{
	m_eNowState = static_cast<LEFT_STATE>(iStateNum);

	if (m_eNowState == LS_UNACTIVE)
		return;

	switch (m_eNowState)
	{
	case CLeftPart::LS_RELOAD_PISTOL:
		m_fTime = 0.f;
		break;
	case CLeftPart::LS_RELOAD_SHOTGUN:
		m_fTime = 0.f;
		break;
	default:
		break;
	}
	
	m_pAnimationCom->Change_Animation(iStateNum);
}

HRESULT CLeftPart::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->m_vScale = { 256.f, 256.f, 1.f };

	m_vStartPos = { WINCX * -0.5f + 200.f, WINCY * -0.5f + 150.f, 0.f };
	m_vEndPos = { WINCX * 0.5f - 200.f, WINCY * -0.5f + 150.f, 0.f };

	m_pTransformCom->Set_Pos(m_vStartPos);

	return S_OK;
}

_int CLeftPart::Update_GameObject(const _float& fTimeDelta)
{
	if (m_eNowState == LS_UNACTIVE)
		return 0;


	int iExit = CCharacter::Update_GameObject(fTimeDelta);
	
	switch (m_eNowState)
	{
	case LS_IDLE:
		
		Update_Idle(fTimeDelta);
		break;

	case LS_RELOAD_PISTOL:		
		Update_Reload(fTimeDelta);
		break;

	case LS_RELOAD_SHOTGUN:		
		Update_Reload(fTimeDelta);
		break;
	default :
		break;
	}


	return iExit;
}

void CLeftPart::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_eNowState == LS_UNACTIVE)
		return;

	CCharacter::LateUpdate_GameObject(fTimeDelta);
}

void CLeftPart::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pAnimationCom->Render_Animation();
	m_pBufferCom->Render_Buffer();
}

HRESULT CLeftPart::Add_Component()
{
	if (FAILED(CCharacter::Add_Component()))
		return E_FAIL;

	Engine::CComponent* pComponent = nullptr;

	pComponent = m_pAnimationCom = dynamic_cast<Engine::CAnimation*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_LeftAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });
	return S_OK;
}

CLeftPart* CLeftPart::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLeftPart* pPart = new CLeftPart(pGraphicDev);

	if (FAILED(pPart->Ready_GameObject()))
	{
		Safe_Release(pPart);
		MSG_BOX("Left Part Create Failed");
		return nullptr;
	}

	return pPart;
}

void CLeftPart::Free()
{
	CCharacter::Free();
}

void CLeftPart::Update_Idle(const float& fTimeDelta)
{
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
}

void CLeftPart::Update_Reload(const float& fTimeDelta)
{
	_vec3 vPos; 
	m_fTime += fTimeDelta + 0.05f;
	D3DXVec3Lerp(&vPos, &m_vStartPos, &m_vEndPos, m_fTime);
	m_pTransformCom->Set_Pos(vPos);
	if (m_fTime > 1.f)
	{
		m_pTransformCom->Set_Pos(m_vEndPos);
	}


	if (m_pAnimationCom->IsEnd())
	{
		Change_State(LS_IDLE);
		m_pTransformCom->Set_Pos(m_vStartPos);
		return;
	}
		
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
}