#include "pch.h"
#include "CPlayer.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"

#include "CManagement.h"

#include "CLeftPart.h"
#include "CRightPart.h"
#include "CMiddlePart.h"

#include "CPistol.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCharacter(pGraphicDev, 15.f)
	, m_pLeftPart(nullptr), m_pRightPart(nullptr), m_pMiddlePart(nullptr)
	, m_eWeaponState(SW_NONE), m_fMoveSpeed(100.f)
	, m_bFall(false), m_fVelocity(0.f), m_fJumpTime(0.f)
	, m_bJump(false), m_fJumpStartY(0.f), m_fJumpDuration(0.6f), m_fJumpHeight(20.f)
	, m_bDash(false), m_fDashTime(0.f), m_fDashDuration(0.3f), m_fDashDistance(80.f)
{

	m_eOBJ_ID = OBJ_PLAYER;
	m_iID = 0;		
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CCharacter(rhs)
	, m_pLeftPart(nullptr), m_pRightPart(nullptr), m_pMiddlePart(nullptr)
	, m_eWeaponState(SW_NONE), m_fMoveSpeed(100.f)
	, m_bFall(false), m_fVelocity(0.f), m_fJumpTime(0.f)
	, m_bJump(false), m_fJumpStartY(0.f), m_fJumpDuration(0.6f), m_fJumpHeight(20.f)
	, m_bDash(false), m_fDashTime(0.f), m_fDashDuration(0.3f), m_fDashDistance(80.f)
{
	m_eOBJ_ID = OBJ_PLAYER;
	m_iID = 0;
}

CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->m_vScale = { 6,6,6 };
	m_pTransformCom->Set_Pos(0.f, 0.f, 0.f);

	m_pCollisionCom->CreateCollider(m_pTransformCom);

	CCollider* m_pCollider = m_pCollisionCom->GetCollider();
	if (!m_pCollider)
		return E_FAIL;

	m_pCollider->Set_Scale(_vec3(4, 11, 4));
	m_pCollider->BindFuncToCollision([&](CollisionInfo info)
		{
			OnCollision(info);
		});

	

	m_eWeaponState = SW_PISTOL;

	return S_OK;
}

_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CCharacter::Update_GameObject(fTimeDelta);

	return _int();
}

void CPlayer::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Key_Input(fTimeDelta);

	if (m_bFall)
	{
		Gravity(fTimeDelta);
	}
	else
	{
		m_fVelocity = 0.f;
	}

	Set_OnFloor(fTimeDelta);
	
	
		

	CCharacter::LateUpdate_GameObject(fTimeDelta);
}

void CPlayer::Render_GameObject()
{

}

HRESULT CPlayer::Add_Component()
{
	if (FAILED(CCharacter::Add_Component()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Add_PlayerPart()
{

	return S_OK;
}

void CPlayer::Key_Input(const _float& fTimeDelta)
{
	Engine::CTransform* pTransform = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()->Get_Component(ID_DYNAMIC, L"Environment_Layer", OBJ_CAM, L"Com_Transform"));
	_vec3 vLook, vRight;
	pTransform->Get_Info(INFO_LOOK, &vLook);

	_vec3 vLookExCludeY = vLook;
	vLookExCludeY.y = 0;

	pTransform->Get_Info(INFO_RIGHT, &vRight);

	D3DXVec3Normalize(&vRight, &vRight);
	D3DXVec3Normalize(&vLookExCludeY, &vLookExCludeY);

	Move_Input(fTimeDelta, vRight, vLookExCludeY);
	Action_Input(fTimeDelta, vLookExCludeY);
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayer* pPlayer = new CPlayer(pGraphicDev);

	if (FAILED(pPlayer->Ready_GameObject()))
	{
		Safe_Release(pPlayer);
		MSG_BOX("Player Create Failed");
		return nullptr;
	}

	return pPlayer;
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot, _vec3 vScale)
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

void CPlayer::Set_LeftPart(CLeftPart* pLeft)
{
	m_pLeftPart = pLeft;
	m_pLeftPart->SetParent(this);
}

void CPlayer::Set_RightPart(CRightPart* pRight)
{
	m_pRightPart = pRight;
	m_pRightPart->SetParent(this);
}

void CPlayer::Set_MiddlePart(CMiddlePart* pMiddle)
{
	m_pMiddlePart = pMiddle;
	m_pMiddlePart->SetParent(this);
}

void CPlayer::Add_Weapon(_byte eWeaponTag, CWeapon* pWeapon)
{
	if (m_mapWeapon.count((STATE_WEAPON)eWeaponTag) > 0)
		return;

	m_mapWeapon.insert({ (STATE_WEAPON)eWeaponTag , pWeapon });

	if (m_mapWeapon.size() == 1)
		pWeapon->Set_Select(true);
}

void CPlayer::Free()
{
	CCharacter::Free();
}

void CPlayer::OnCollision(CollisionInfo info)
{

}

//몬스터 전체를 가져와서 마우스와 피킹 체크 
void CPlayer::CheckPickedMonster()
{
	list<pair<_float, CCollider*>> pickedList;

	_float fAttack = m_mapWeapon[m_eWeaponState]->Get_Power();

	CollisionInfo info = { NULL, {0,0,0}, fAttack };

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
		for (auto& pairCollider : mapCollider)
		{
			bool bPicked = CCollision::Collision_Mouse(g_hWnd, m_pGraphicDev, pairCollider.second);
			if (bPicked)
			{
				pickedList.push_back({ iter->second->Get_ViewZ() ,pairCollider.second });
			}
		}

	}

	if (pickedList.empty()) return;
	//카메라 거리순 정렬
	pickedList.sort([&](auto& _First, auto& _Second)
		{
			return _First.first < _Second.first;
		});

	//피킹된 대상 중 카메라와 제일 가까운 콜라이더만 충돌처리하기 
	CCollider* NearPickedCollider = pickedList.front().second;
	NearPickedCollider->Collision(info);
}

void CPlayer::Move_Input(const _float& fTimeDelta, const _vec3& vRight, const _vec3& vLook)
{
	MOVE_DIR eDir = CDInputMgr::GetInstance()->Get_Direction();
	float fMoveSpeed = m_fMoveSpeed * 0.5f * sqrtf(2);
	switch (eDir)
	{
	case Engine::DIR_NONE:
		break;
	case Engine::DIR_UP:
		m_pTransformCom->Move_Pos(&vLook, fTimeDelta, m_fMoveSpeed);
		break;
	case Engine::DIR_DOWN:
		m_pTransformCom->Move_Pos(&vLook, fTimeDelta, -m_fMoveSpeed);
		break;
	case Engine::DIR_LEFT:
		m_pTransformCom->Move_Pos(&vRight, fTimeDelta, -m_fMoveSpeed);
		break;
	case Engine::DIR_RIGHT:
		m_pTransformCom->Move_Pos(&vRight, fTimeDelta, m_fMoveSpeed);
		break;
	case Engine::DIR_LEFTUP:		
		m_pTransformCom->Move_Pos(&vRight, fTimeDelta, -fMoveSpeed);
		m_pTransformCom->Move_Pos(&vLook, fTimeDelta, fMoveSpeed);
		break;
	case Engine::DIR_LEFTDOWN:		
		m_pTransformCom->Move_Pos(&vRight, fTimeDelta, -fMoveSpeed);
		m_pTransformCom->Move_Pos(&vLook, fTimeDelta, -fMoveSpeed);
		break;
	case Engine::DIR_RIGHTUP:		
		m_pTransformCom->Move_Pos(&vRight, fTimeDelta, fMoveSpeed);
		m_pTransformCom->Move_Pos(&vLook, fTimeDelta, fMoveSpeed);
		break;
	case Engine::DIR_RIGHTDOWN:		
		m_pTransformCom->Move_Pos(&vRight, fTimeDelta, fMoveSpeed);
		m_pTransformCom->Move_Pos(&vLook, fTimeDelta, -fMoveSpeed);
		break;	
	}
}

void CPlayer::Action_Input(const _float& fTimeDelta, const _vec3& vLook)
{
	// 일반 공격
	if (CDInputMgr::GetInstance()->Mouse_Down(DIM_LB))
	{
		if (m_mapWeapon[m_eWeaponState]->Can_Fire())
		{
			m_pRightPart->ChangeState(GetStateID(ATTACK, m_eWeaponState));
		}
	}

	// 대쉬
	if (CDInputMgr::GetInstance()->Mouse_Down(DIM_RB) && !m_bDash)
	{
		m_fDashTime = 0.f;
		m_fVelocity = 0.f;
		m_vDashStart = m_pTransformCom->m_vInfo[INFO_POS];
		
		MOVE_DIR eDir = CDInputMgr::GetInstance()->Get_Direction();
		_vec3	vLook = *m_pTransformCom->Get_Info(INFO_LOOK);
		_vec3	vRight = *m_pTransformCom->Get_Info(INFO_RIGHT);
		D3DXVec3Normalize(&vLook, &vLook);
		D3DXVec3Normalize(&vRight, &vRight);
		switch (eDir)
		{
			// FRONT
		case Engine::DIR_NONE:			
		case Engine::DIR_UP:
			m_vDashDir = vLook;
			break;
		case Engine::DIR_DOWN:
			m_vDashDir = vLook * -1.f;
			break;
		case Engine::DIR_LEFT:
			m_vDashDir = vRight * -1.f;
			break;
		case Engine::DIR_RIGHT:
			m_vDashDir = vRight;
			break;
		case Engine::DIR_LEFTUP:
			m_vDashDir = vRight * -1.f + vLook;
			break;
		case Engine::DIR_LEFTDOWN:
			m_vDashDir = vRight * -1.f + vLook * -1.f;
			break;
		case Engine::DIR_RIGHTUP:
			m_vDashDir = vRight + vLook;
			break;
		case Engine::DIR_RIGHTDOWN:
			m_vDashDir = vRight + vLook*-1.f;
			break;
		}
		D3DXVec3Normalize(&m_vDashDir, &m_vDashDir);
		m_vDashDir.y = 0.f;
		m_bDash = true;
	}

	//점프
	if (CDInputMgr::GetInstance()->Key_Down(DIK_SPACE) && !m_bJump && !m_bDash)
	{
		m_fJumpTime = 0.f;
		m_fVelocity = 0.f;
		m_fJumpStartY = m_pTransformCom->m_vInfo[INFO_POS].y;
		m_bJump = true;
	}


	// 발차기
	if (CDInputMgr::GetInstance()->Key_Down(DIK_LSHIFT))
	{
		m_pMiddlePart->ChangeState(KICK);
	}

	if (CDInputMgr::GetInstance()->Key_Down(DIK_E))
	{
		m_pMiddlePart->ChangeState(DRINK);
	}

	// 장전
	if (CDInputMgr::GetInstance()->Key_Down(DIK_R))
	{
		if (m_pLeftPart->Get_ActionAble() && m_pRightPart->Get_ActionAble())
		{
			m_mapWeapon[m_eWeaponState]->Set_ShootAble(false);
			m_pLeftPart->ChangeState(GetStateID(RELOAD, m_eWeaponState));
			m_pRightPart->ChangeState(GetStateID(RELOAD, m_eWeaponState));
			m_pMiddlePart->ChangeState(IDLE);
		}
	}
}

void CPlayer::Fire()
{
	m_mapWeapon[m_eWeaponState]->Fire();
	CheckPickedMonster();
}

void CPlayer::Reload()
{
	m_mapWeapon[m_eWeaponState]->Reload();
}


void CPlayer::Gravity(const _float& fTimeDelta)
{
	_float fVelocity = Get_Velocity();
	fVelocity -= 9.81f * (fTimeDelta + 0.25f);
	m_fVelocity = fVelocity;
}

void CPlayer::Set_OnFloor(const _float& fTimeDelta)
{
	_float fY = 0.f;
	_float fBottom = 0.f;
	_vec3 vPosition = m_pTransformCom->m_vInfo[INFO_POS];

	if (m_bJump || m_bDash)
	{
		if (m_bJump)
		{
			Update_Jump(fTimeDelta);
		}
		if (m_bDash)
		{
			Update_Dash(fTimeDelta);
		}

		return;
	}
	else if (m_bFall)
	{
		vPosition.y += m_fVelocity * fTimeDelta;
		fBottom = vPosition.y - 15.f;

		if (fBottom <= fY)
		{
			m_bFall = false;
			vPosition.y = fY + 15.f;
		}

	}
	else
	{
		vPosition.y = fY + 15.f;

	}

	m_pTransformCom->Set_Pos(vPosition);
}

void CPlayer::Update_Jump(const _float& fTimeDelta)
{
	m_fJumpTime += fTimeDelta;
	float t = m_fJumpTime / m_fJumpDuration;

	if (t >= 1.f)
	{
		t = 1.f;
		m_bJump = false;
		if (!m_bDash)
			m_bFall = true;
	}

	float easeOutQuad = 1.f - (1.f - t) * (1.f - t);

	float fNewY = m_fJumpStartY + easeOutQuad * m_fJumpHeight;

	m_pTransformCom->m_vInfo[INFO_POS].y = fNewY;
}

void CPlayer::Update_Dash(const _float& fTimeDelta)
{
	m_fDashTime += fTimeDelta;
	float t = m_fDashTime / m_fDashDuration;
	if (t >= 1.f)
	{
		t = 1.f;
		m_bDash = false;
		m_bFall = true;
	}
	float easeOutQuad = 1.f - (1.f - t) * (1.f - t);
	_float dashDistance = easeOutQuad * m_fDashDistance;
	_vec3 newPos = m_vDashStart + m_vDashDir * dashDistance;

	m_pTransformCom->Set_Pos(newPos);
}

_bool CPlayer::Get_OnFloor()
{
	_float fY = 0.f;
	_vec3 vPosition = m_pTransformCom->m_vInfo[INFO_POS];
	_float fBottom = vPosition.y - 15.f;

	if (fBottom <= fY)
	{
		return true;
	}
	else
	{
		return false;
	}
}