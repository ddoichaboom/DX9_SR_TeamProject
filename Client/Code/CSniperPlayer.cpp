#include "pch.h"
#include "CSniperPlayer.h"
#include "CSniferCamera.h"
#include "CSRightHand.h"
#include "CSLeftHand.h"

#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CUIManager.h"
#include "CManagement.h"
#include "CHitUI.h"
#include "CPoolMgr.h"


CSniperPlayer::CSniperPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCharacter(pGraphicDev, 20.f), m_pMainCollider(nullptr), m_pHitUI(nullptr), m_pCamera(nullptr)
{
	m_eOBJ_ID = OBJ_PLAYER;
	m_iID = 0;
}

CSniperPlayer::CSniperPlayer(const CSniperPlayer& rhs)
	: CCharacter(rhs), m_pMainCollider(nullptr), m_pHitUI(nullptr), m_pCamera(nullptr)
{
	m_eOBJ_ID = OBJ_PLAYER;
	m_iID = 0;
}

CSniperPlayer::~CSniperPlayer()
{

}

void CSniperPlayer::CreateStateData()
{
	auto Mgr = CDataMgr<CSniperPlayer>::GetInstance();
	//클래스 당 한번만 실행되게
	if (Mgr->IsStateEmpty() == false) return;

	CState<CSniperPlayer>* State = new CState<CSniperPlayer>(&CSniperPlayer::Intro_Begin, &CSniperPlayer::Intro, nullptr);
	Mgr->AddState(SN_INTRO, State);

	State = new CState<CSniperPlayer>(nullptr, &CSniperPlayer::Idle, nullptr);
	Mgr->AddState(SN_IDLE, State);

	State = new CState<CSniperPlayer>(&CSniperPlayer::ZoomIn, &CSniperPlayer::Attack, &CSniperPlayer::ZoomOut);
	Mgr->AddState(SN_ATTACK, State);
}

HRESULT CSniperPlayer::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CreateStateData();

	m_pTransformCom->m_vScale = { 1,1,1 };
	m_pTransformCom->Set_Pos(0.f, 0.f, 0.f);

	//Camera
	_vec3 vLook = *m_pTransformCom->Get_Info(INFO_POS) + _vec3(0, 0, 1);
	_vec3 vUp = { 0.f, 1.f, 0.f };
	m_pCamera = CSniferCamera::Create(m_pGraphicDev, this, m_pTransformCom->Get_Info(INFO_POS),
		&vLook, &vUp);

	m_pCamera->Set_AngleLimit(ROT_X, -90.f, 90.f);
	m_pCamera->Set_AngleLimit(ROT_Y, -45.f, 45.f);

	//Collider
	m_pMainCollider = m_pCollisionCom->CreateCollider(this, m_szMainColliderName);
	m_pMainCollider->Set_Scale(_vec3(4, 15, 4));
	m_pMainCollider->BindFuncToCollision([&](CollisionInfo info)
		{
			OnCollision(info);
		});

	m_pCollisionCom->SetMainCollider(m_szMainColliderName);

	//Create Hands 
	//m_pLeftHand = CSLeftHand::Create

	m_pRightHand = CSRightHand::Create(m_pGraphicDev);

	//Effect
	//m_pHitUI = CHitUI::Create(m_pGraphicDev);
	//m_pHitUI->SetDead();

	Change_State(SN_INTRO);
	return S_OK;
}

_int CSniperPlayer::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CCharacter::Update_GameObject(fTimeDelta);
	if (m_pCamera) m_pCamera->Update_GameObject(fTimeDelta);
	if (m_pRightHand) m_pRightHand->Update_GameObject(fTimeDelta);

	Key_Input(fTimeDelta);

	if (m_pHitUI && m_pHitUI->IsDead() == false)
	{
		m_pHitUI->Update_GameObject(fTimeDelta);
	}
	return iExit;
}

void CSniperPlayer::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CCharacter::LateUpdate_GameObject(fTimeDelta);
	if (m_pCamera) m_pCamera->LateUpdate_GameObject(fTimeDelta);
	if (m_pRightHand) m_pRightHand->LateUpdate_GameObject(fTimeDelta);

}

void CSniperPlayer::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
}

HRESULT CSniperPlayer::Add_Component()
{
	if (FAILED(CCharacter::Add_Component())) return E_FAIL;
	return S_OK;
}

void CSniperPlayer::Key_Input(const _float& fTimeDelta)
{
	if (!m_bCanInput) return; 

	_long	dwMouseMove(0);
	if (dwMouseMove = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_Y))
	{
		_float moveValue = D3DXToDegree(dwMouseMove * m_fMouseSpeed);
		if (m_pCamera) m_pCamera->Set_Rot(ROT_X, moveValue);
	}

	if (dwMouseMove = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_X))
	{
		_float moveValue = D3DXToDegree(dwMouseMove * m_fMouseSpeed);
		if (m_pCamera) m_pCamera->Set_Rot(ROT_Y, moveValue);
	}
}


void CSniperPlayer::Change_State(_uint eState)
{
	m_fTime = 0.f;
	m_pStateCom->ChangeState<CSniperPlayer>(eState);
}

void CSniperPlayer::Intro_Begin()
{
	DisableInput();
	m_pRightHand->SetState_INTRO();
}

void CSniperPlayer::Intro()
{
	if (m_pRightHand->CanAnimationEnd()) EnableInput();
}

void CSniperPlayer::Idle()
{

}

void CSniperPlayer::ZoomIn()
{
}

void CSniperPlayer::Attack()
{
}


void CSniperPlayer::ZoomOut()
{
}

void CSniperPlayer::OnCollision(CollisionInfo info)
{

}

CSniperPlayer* CSniperPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSniperPlayer* pSnifer = new CSniperPlayer(pGraphicDev);

	if (FAILED(pSnifer->Ready_GameObject()))
	{
		Safe_Release(pSnifer);
		MSG_BOX("Snifer Player Create Failed");
		return nullptr;
	}

	return pSnifer;
}
CSniperPlayer* CSniperPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
	CSniperPlayer* pSnifer = new CSniperPlayer(pGraphicDev);

	if (FAILED(pSnifer->Ready_GameObject()))
	{
		Safe_Release(pSnifer);
		MSG_BOX("Snifer Player Create Failed");
		return nullptr;
	}

	CTransform* pTransform = static_cast<Engine::CTransform*>(
		pSnifer->Get_Component(ID_DYNAMIC, L"Com_Transform"));
	pTransform->Set_Pos(vPos);
	pTransform->Update_Component(0.f);

	return pSnifer;
}

void CSniperPlayer::Free()
{
	Safe_Release(m_pCamera);
	CCharacter::Free();
}
