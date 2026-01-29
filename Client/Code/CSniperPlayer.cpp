#include "pch.h"
#include "CSniperPlayer.h"
#include "CSniferCamera.h"
#include "CSRightHand.h"
#include "CLeftPart.h"

#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CUIManager.h"
#include "CManagement.h"
#include "CPoolMgr.h"

#include "CHitUI.h"
#include "CSniperUI.h"


CSniperPlayer::CSniperPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCharacter(pGraphicDev, 20.f), m_pMainCollider(nullptr)
	, m_pHitUI(nullptr), m_pCamera(nullptr), m_pSniperUI(nullptr)
{
	m_eOBJ_ID = OBJ_PLAYER;
	m_iID = 0;
}

CSniperPlayer::CSniperPlayer(const CSniperPlayer& rhs)
	: CCharacter(rhs), m_pMainCollider(nullptr), m_pHitUI(nullptr), m_pCamera(nullptr), m_pSniperUI(nullptr)
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

	m_pTransformCom->m_vScale = { 2,15,2 };
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
	m_pLeftHand = CLeftPart::Create(m_pGraphicDev);
	if (!m_pLeftHand) return E_FAIL;
	m_pLeftHand->ChangeState(IDLE);

	m_pRightHand = CSRightHand::Create(m_pGraphicDev);
	if (!m_pRightHand) return E_FAIL;

	//Effect
	m_pHitUI = CHitUI::Create(m_pGraphicDev);
	if(!m_pHitUI) return E_FAIL;
	m_pHitUI->SetDead();

	m_pSniperUI = CSniperUI::Create(m_pGraphicDev);
	if (!m_pSniperUI) return E_FAIL;

	//Set Data
	m_fMouseSpeed = m_fBaseMouseSpeed;
	ChangeState(SN_INTRO);
	m_fAttackDamage = 100.f;

	return S_OK;
}

_int CSniperPlayer::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CCharacter::Update_GameObject(fTimeDelta);
	m_fTime += fTimeDelta;

	if (m_pCamera) m_pCamera->Update_GameObject(fTimeDelta);
	if (m_bRenderStop)
	{
		if (m_pSniperUI) m_pSniperUI->Update_GameObject(fTimeDelta);
	}
	else
	{
		if (m_pRightHand) m_pRightHand->Update_GameObject(fTimeDelta);
		if (m_pLeftHand)
		{
			if(m_pStateCom->GetCurrentStateID() != SN_INTRO)
				m_pLeftHand->Update_GameObject(fTimeDelta);
		}
	}

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
	if (!m_bRenderStop)
	{
		if (m_pRightHand) m_pRightHand->LateUpdate_GameObject(fTimeDelta);
		if (m_pLeftHand) m_pLeftHand->LateUpdate_GameObject(fTimeDelta);
	}


	//Shoot
	if (CDInputMgr::GetInstance()->Mouse_Down(DIM_LB))
	{
		if (m_bRenderStop) // 조준선 UI랜더링 중 이라면 
		{
			Shoot();
		}
	}

}

void CSniperPlayer::Render_GameObject()
{
}

HRESULT CSniperPlayer::Add_Component()
{
	if (FAILED(CCharacter::Add_Component())) return E_FAIL;
	return S_OK;
}

void CSniperPlayer::Key_Input(const _float& fTimeDelta)
{
	if (!m_bCanInput) return; 

	//카메라 회전 
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

	// 키 입력
	//Zoom
	if (CDInputMgr::GetInstance()->Mouse_Down(DIM_RB))
	{
		if(m_pStateCom->GetCurrentStateID() == SN_IDLE)
		{
			if(m_pRightHand->IsState_IDLE())
				ChangeState(SN_ATTACK);
		}
		else
		{
			ChangeState(SN_IDLE);
			m_pRightHand->SetState_IDLE();
		}
	}

}


bool CSniperPlayer::Get_IsAimState()
{
	return m_pStateCom->GetCurrentStateID() == SN_ATTACK;
}

void CSniperPlayer::ChangeState(_uint nextStateID)
{
	m_fTime = 0.f;
	if(m_pStateCom) m_pStateCom->ChangeState<CSniperPlayer>(nextStateID);
}

void CSniperPlayer::Intro_Begin()
{
	DisableInput();
	if(m_pRightHand) m_pRightHand->SetState_INTRO();
}

void CSniperPlayer::Intro()
{
	if (m_pRightHand->CanAnimationEnd())
	{
		EnableInput();
		ChangeState(SN_IDLE);
	}
}

void CSniperPlayer::Idle()
{

}

void CSniperPlayer::ZoomIn()
{
	m_pRightHand->SetState_ATTACK();
	m_pCamera->ZoomIn(D3DXToRadian(m_fZoomFOV));
	m_fMouseSpeed = m_fZoomMouseSpeed;
}

void CSniperPlayer::Attack()
{
	if (!m_bRenderStop && m_pRightHand->IsAnimationEnd())
		m_bRenderStop = true;
	if (m_bShoot && m_pCamera->IsCameraShaking() == false)
	{
		//ChangeState(SN_IDLE);
	}
}	

void CSniperPlayer::Shoot()
{
	if (m_bShoot) return;
	m_bShoot = true;
	m_pCamera->CameraShake();
	CheckedPickedMonster();
	CheckedPicked(L"Environment_Layer", OBJ_ITEM);
}

void CSniperPlayer::ZoomOut()
{
	m_bRenderStop = false;
	if (m_bShoot) m_pRightHand->SetState_RELOAD();
	else m_pRightHand->SetState_IDLE();

	m_bShoot = false;
	m_pCamera->ZoomOut();
	m_fMouseSpeed = m_fBaseMouseSpeed;
}

void CSniperPlayer::OnCollision(CollisionInfo info)
{
	if (info.fDamage > 0.f)
	{
		if (m_pHitUI->IsDead())
		{
			m_pHitUI->Reset();
		}
		m_fHP -= info.fDamage;
		if (m_fHP < 0.f) m_fHP = 0.f;
		//Dead 처리 안함
	}
}

void CSniperPlayer::CheckedPickedMonster()
{
	CollisionInfo info = { this, {0,0,0}, m_fAttackDamage };

	CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");
	if (!pLayer) return;

	auto pairIter = pLayer->Get_Objects(OBJ_MONSTER);
	for (auto iter = pairIter.first; iter != pairIter.second; iter++)
	{
		CCollision* pCollision = static_cast<CCollision*>(iter->second->Get_Component(ID_DYNAMIC, L"Com_Collision"));
		if (!pCollision) continue;

		auto& mapCollider = pCollision->GetColliderMap();
		if (mapCollider.empty()) continue;
		//몬스터의 CollisionCom에 있는 전체 Collider 
		for (auto& pairCollider : mapCollider)
		{
			bool bPicked = CCollision::Collision_Mouse(g_hWnd, m_pGraphicDev, pairCollider.second);
			if (bPicked)
			{
				pairCollider.second->Collision(info);
				return;
			}
		}
	}
}

void CSniperPlayer::CheckedPicked(const _tchar* layerName, OBJ_ID eID)
{
	CollisionInfo info = { this, {0,0,0}, m_fAttackDamage };

	CLayer* pLayer = CManagement::GetInstance()->Get_Layer(layerName);
	if (!pLayer) return;

	auto pairIter = pLayer->Get_Objects(eID);
	for (auto iter = pairIter.first; iter != pairIter.second; iter++)
	{
		CCollision* pCollision = static_cast<CCollision*>(iter->second->Get_Component(ID_DYNAMIC, L"Com_Collision"));
		if (!pCollision) continue;

		auto& mapCollider = pCollision->GetColliderMap();
		if (mapCollider.empty()) continue;
		//몬스터의 CollisionCom에 있는 전체 Collider 
		for (auto& pairCollider : mapCollider)
		{
			bool bPicked = CCollision::Collision_Mouse(g_hWnd, m_pGraphicDev, pairCollider.second);
			if (bPicked)
			{
				pairCollider.second->Collision(info);
				return;
			}
		}
	}
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
	Safe_Release(m_pLeftHand);
	Safe_Release(m_pRightHand);
	Safe_Release(m_pHitUI);
	Safe_Release(m_pSniperUI);
	Safe_Release(m_pCamera);
	CCharacter::Free();
}
