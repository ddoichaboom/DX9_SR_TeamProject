#include "pch.h"
#include "CPlayer.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"

#include "CManagement.h"

#include "CPlayerPart.h"
#include "CLeftPart.h"
#include "CRightPart.h"
#include "CMiddlePart.h"




CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCharacter(pGraphicDev)
	, m_pLeftPart(nullptr), m_pRightPart(nullptr), m_pMiddlePart(nullptr)
	, m_bCheck(false), m_eNowState(PLAYER_UNACTIVE), m_eWeaponState(WS_PISTOL)
{
	m_eOBJ_ID = OBJ_PLAYER;
	//OBJ_Player가 0이고 , PlayerPart 는 Player 생성자 이후에 생기므로 iCount > 0이기때문에 
	//id를 0로 해도 무방
	m_iID = 0;
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CCharacter(rhs)
	, m_pLeftPart(nullptr), m_pRightPart(nullptr), m_pMiddlePart(nullptr)
	, m_bCheck(false), m_eNowState(PLAYER_UNACTIVE), m_eWeaponState(WS_PISTOL)
{
	m_eOBJ_ID = OBJ_PLAYER;
	m_iID = 0;
}

CPlayer::~CPlayer()
{
}

void CPlayer::Change_State(PLAYER_STATE eState)
{
	m_eNowState = eState;

	switch (eState)
	{
	case CPlayer::PLAYER_IDLE:
		m_pLeftPart->Change_State(CLeftPart::LS_IDLE);
		m_pMiddlePart->Change_State(CMiddlePart::MS_UNACTIVE);

		if(m_eWeaponState == WS_PISTOL)
			m_pRightPart->Change_State(CRightPart::RS_IDLE_PISTOL);
		else if(m_eWeaponState == WS_SHOTGUN)
			m_pRightPart->Change_State(CRightPart::RS_IDLE_SHOTGUN);
		
		break;
	case CPlayer::PLAYER_ATTACK:
		m_pLeftPart->Change_State(CLeftPart::LS_IDLE);
		m_pMiddlePart->Change_State(CMiddlePart::MS_UNACTIVE);

		if (m_eWeaponState == WS_PISTOL)
			m_pRightPart->Change_State(CRightPart::RS_ATTACK_PISTOL);
		else if (m_eWeaponState == WS_SHOTGUN)
			m_pRightPart->Change_State(CRightPart::RS_ATTACK_SHOTGUN);
		break;
	case CPlayer::PLAYER_RELOAD:
		
		m_pMiddlePart->Change_State(CMiddlePart::MS_UNACTIVE);

		if (m_eWeaponState == WS_PISTOL)
		{
			m_pLeftPart->Change_State(CLeftPart::LS_RELOAD_PISTOL);
			m_pRightPart->Change_State(CRightPart::RS_RELOAD_PISTOL);
		}			
		else if (m_eWeaponState == WS_SHOTGUN)
		{
			m_pLeftPart->Change_State(CLeftPart::LS_RELOAD_SHOTGUN);
			m_pRightPart->Change_State(CRightPart::RS_RELOAD_SHOTGUN);
		}
			
		break;
	case CPlayer::PLAYER_KICK:

		m_pLeftPart->Change_State(CLeftPart::LS_IDLE);
		m_pMiddlePart->Change_State(CMiddlePart::MS_KICK);
		if (m_eWeaponState == WS_PISTOL)
			m_pRightPart->Change_State(CRightPart::RS_IDLE_PISTOL);
		else if (m_eWeaponState == WS_SHOTGUN)
			m_pRightPart->Change_State(CRightPart::RS_IDLE_SHOTGUN);
		break;

	case CPlayer::PLAYER_DRINK:
		m_pLeftPart->Change_State(CLeftPart::LS_UNACTIVE);
		m_pMiddlePart->Change_State(CMiddlePart::MS_SODA);
		if (m_eWeaponState == WS_PISTOL)
			m_pRightPart->Change_State(CRightPart::RS_IDLE_PISTOL);
		else if (m_eWeaponState == WS_SHOTGUN)
			m_pRightPart->Change_State(CRightPart::RS_IDLE_SHOTGUN);
		break;
	case CPlayer::PLAYER_SLIDE:

		break;
	default:
		break;
	}
}

void CPlayer::Set_WeaponState(WEAPON_STATE eState)
{
	
	m_pLeftPart->Set_WeaponState(eState);
	m_pRightPart->Set_WeaponState(eState);
	m_pMiddlePart->Set_WeaponState(eState);
}

HRESULT CPlayer::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	if (FAILED(Add_PlayerPart()))
		return E_FAIL;
	Change_State(PLAYER_IDLE);

	//m_pTransformCom->m_vScale = { 6.f, 6.f, 1.f };

	return S_OK;
}

_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CCharacter::Update_GameObject(fTimeDelta);

	m_pMiddlePart->Update_GameObject(fTimeDelta);
	m_pRightPart->Update_GameObject(fTimeDelta);
	m_pLeftPart->Update_GameObject(fTimeDelta);
	
	

	Check_AnimationState();

	return iExit;
}

void CPlayer::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Key_Input(fTimeDelta);

	CCharacter::LateUpdate_GameObject(fTimeDelta);

	m_pMiddlePart->LateUpdate_GameObject(fTimeDelta);
	m_pRightPart->LateUpdate_GameObject(fTimeDelta);
	m_pLeftPart->LateUpdate_GameObject(fTimeDelta);
}

void CPlayer::Render_GameObject()
{

}

HRESULT CPlayer::Add_Component()
{
	if (FAILED(CCharacter::Add_Component())) return E_FAIL;
	return S_OK;
}

void CPlayer::Key_Input(const _float& fTimeDelta)
{
	Engine::CTransform* pTransform = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()->
		Get_Component(ID_DYNAMIC, L"Environment_Layer", OBJ_CAM, L"Com_Transform"));

	_vec3 vLook, vRight;
	pTransform->Get_Info(INFO_LOOK, &vLook);

	_vec3 vLookExCludeY = vLook;
	vLookExCludeY.y = 0;

	pTransform->Get_Info(INFO_RIGHT, &vRight);

	// 앞으로 이동
	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_W) & 0x80)
	{
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLookExCludeY, &vLookExCludeY), fTimeDelta, 20.f);

	}

	// 왼쪽 이동
	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_A) & 0x80)
	{
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, -20.f);

	}

	// 뒤로 이동
	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_S) & 0x80)
	{
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLookExCludeY, &vLookExCludeY), fTimeDelta, -20.f);
	}

	// 오른쪽 이동
	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_D) & 0x80)
	{
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, 20.f);
	}


	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_SPACE) & 0x80)
	{
		if (m_eNowState != PLAYER_KICK)
		{
			Change_State(PLAYER_KICK);
		}

	}

	// 장전
	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_R) & 0x80)
	{
		Change_State(PLAYER_RELOAD);

	}

	// 공격 
	if (CDInputMgr::GetInstance()->Mouse_Down(DIM_LB))
	{
		Change_State(PLAYER_ATTACK);
		// 방승희 임시 추가
		CheckPickedMonster();
		// 추가 끝
	}


	// 대쉬 
	if (CDInputMgr::GetInstance()->Get_DIMouseState(DIM_RB) & 0x80)
	{
		Change_State(PLAYER_DRINK);

		
	}

}

HRESULT CPlayer::Add_PlayerPart()
{

	m_pMiddlePart = CMiddlePart::Create(m_pGraphicDev);

	if (nullptr == m_pMiddlePart)
		return E_FAIL;

	m_pRightPart = CRightPart::Create(m_pGraphicDev);

	if (nullptr == m_pRightPart)
		return E_FAIL;


	m_pLeftPart = CLeftPart::Create(m_pGraphicDev);

	if (nullptr == m_pLeftPart)
		return E_FAIL;
	

	return S_OK;
}

void CPlayer::Check_AnimationState()
{
	if ( (m_eNowState == PLAYER_DRINK || m_eNowState == PLAYER_KICK) && m_pMiddlePart->IsAnimationEnd())
	{
		Change_State(PLAYER_IDLE);
	}
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayer* pPlayer = new CPlayer(pGraphicDev);

	if (FAILED(pPlayer->Ready_GameObject()))
	{
		Safe_Release(pPlayer);
		MSG_BOX("pPlayer Create Failed");
		return nullptr;
	}

	return pPlayer;
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev,
								_vec3 vPos,
								_vec3 vRot,
								_vec3 vScale)
{
	CPlayer* pPlayer = new CPlayer(pGraphicDev);

	if (FAILED(pPlayer->Ready_GameObject()))
	{
		Safe_Release(pPlayer);
		MSG_BOX("pPlayer Create Failed");
		return nullptr;
	}

	CTransform* pTransform = dynamic_cast<Engine::CTransform*>(
		pPlayer->Get_Component(ID_DYNAMIC, L"Com_Transform"));

	pTransform->Set_Pos(vPos);
	pTransform->Set_Angle(vRot.x, vRot.y, vRot.z);
	pTransform->Set_Scale(vScale.x, vScale.y, vScale.z);
	pTransform->Update_Component(0.f);

	return pPlayer;
}

void CPlayer::Free()
{
	Safe_Release(m_pLeftPart);
	Safe_Release(m_pRightPart);
	Safe_Release(m_pMiddlePart);

	CCharacter::Free();
}

//몬스터 전체를 가져와서 마우스와 피킹 체크 
void CPlayer::CheckPickedMonster()
{
	CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");
	if (!pLayer) return;

	auto pairIter = pLayer->Get_Objects(OBJ_MONSTER);
	//multimap<OBJ_ID, CGameObject*> 에 대한 반복자
	//OBJ_ID를 키로 가진 오브젝트들의 반복자 범위를 반환 = 몬스터 전체 목록
	for (auto iter = pairIter.first; iter != pairIter.second; iter++)
	{
		CCollision* pCollision = static_cast<CCollision*>(iter->second->Get_Component(ID_DYNAMIC, L"Com_Collision"));
		if (!pCollision) continue;

		auto& mapCollider = pCollision->GetColliderMap();
		if (mapCollider.empty()) continue;
		//몬스터의 CollisionCom에 있는 전체 Collider 
		for (auto pairCollider : mapCollider)
		{
			bool bPicked = CCollision::Collision_Mouse(g_hWnd, m_pGraphicDev, pairCollider.second);
			if (bPicked)
			{
				CollisionInfo info = { NULL, {0,0,0}, m_fAtk };
				pairCollider.second->Collision(info);
				//한 콜라이더에서 충돌이 일어났다면 이 몬스터의 다른 콜라이더와는 충돌체크 하지않음
				break;
			}
		}
	}

}
