#include "pch.h"
#include "CLeftHand.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"


CLeftHand::CLeftHand(LPDIRECT3DDEVICE9 pGraphicDev)
	:CCharacter(pGraphicDev)
	,	m_eNowState(IDLE)
{
}

CLeftHand::CLeftHand(const CLeftHand& rhs)
	:CCharacter(rhs)
	, m_eNowState(IDLE)
{
}

CLeftHand::~CLeftHand()
{
}

void CLeftHand::Set_Animation()
{
	 m_pAnimationCom->ChangeNextAnimation(); 
	 m_pAnimationCom->PlayFromStart();
}

HRESULT CLeftHand::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->m_vScale = { 256.f, 256.f, 1.f };

	m_pAnimationCom->Change_Animation(0);

	m_vStartPos = { _float(-WINCX / 2) + 200.f, _float(-WINCY / 2) + 200.f, 0.f };
	m_vEndPos = { _float(WINCX / 2) - 200.f, _float(-WINCY / 2) + 200.f, 0.f };
	m_pTransformCom->Set_Pos(m_vStartPos.x, m_vStartPos.y, m_vStartPos.z);

	return S_OK;
}

_int CLeftHand::Update_GameObject(const _float& fTimeDelta)
{

	
	int iExit = CCharacter::Update_GameObject(fTimeDelta);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
	
	//switch (m_eNowState)
	//{
	//case CLeftHand::IDLE:
	//	Update_Idle(fTimeDelta);
	//	break;
	//case CLeftHand::PISTOL_RELOAD:
	//	Update_Reload_Pistol(fTimeDelta);
	//	break;
	//case CLeftHand::SHOTGUN_RELOAD:
	//	Update_Reload_ShotGun(fTimeDelta);
	//	break;
	//default:
	//	break;
	//}
	//

	
	return iExit;
}

void CLeftHand::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CCharacter::LateUpdate_GameObject(fTimeDelta);
}

void CLeftHand::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pAnimationCom->Render_Animation();

	m_pBufferCom->Render_Buffer();
	m_pAnimationCom->LateRender_Animation();
}



HRESULT CLeftHand::Add_Component()
{
	if (FAILED(CCharacter::Add_Component()))
		return E_FAIL;

	Engine::CComponent* pComponent = nullptr;

	// Animation
	pComponent = m_pAnimationCom = dynamic_cast<Engine::CAnimation*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_LeftAnimation"));	

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });

	return S_OK;
}

CLeftHand* CLeftHand::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLeftHand* pTest = new CLeftHand(pGraphicDev);

	if (FAILED(pTest->Ready_GameObject()))
	{
		Safe_Release(pTest);
		MSG_BOX("Left Hand Create Failed");
		return nullptr;
	}

	return pTest;
}

void CLeftHand::Free()
{
	CCharacter::Free();
}





void CLeftHand::Change_State(eState eState)
{
	m_eNowState = eState;
	m_pAnimationCom->Change_Animation((_uint)eState);
	m_pAnimationCom->PlayFromStart();
}

void CLeftHand::Update_Idle(const _float& fTimeDelta)
{
	m_pTransformCom->Set_Pos(m_vStartPos.x, m_vStartPos.y, m_vStartPos.z);

}

void CLeftHand::Update_Reload_Pistol(const _float& fTimeDelta)
{
	if (m_pAnimationCom->IsEnd())
	{		
		Change_State(IDLE);
		return;
	}
		
}

void CLeftHand::Update_Reload_ShotGun(const _float& fTimeDelta)
{
	if (m_pAnimationCom->IsEnd())
	{	
		Change_State(IDLE);
		return;
	}
}