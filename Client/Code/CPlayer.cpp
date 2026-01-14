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
#include "CKatana.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCharacter(pGraphicDev, 15.f)
	, m_pLeftPart(nullptr), m_pRightPart(nullptr), m_pMiddlePart(nullptr)
	, m_eWeaponState(SW_NONE), m_fMoveSpeed(100.f)
	, m_bFall(false), m_fVelocity(0.f), m_fJumpTime(0.f)
	, m_bJump(false), m_fJumpStartY(0.f), m_fJumpDuration(0.6f), m_fJumpHeight(20.f)
	, m_bDash(false), m_fDashTime(0.f), m_fDashDuration(0.3f), m_fDashDistance(80.f)
	, m_fKickAttack(1.f), m_pKickCollider(nullptr), m_pMainCollider(nullptr)
	, m_eNowState(MAIN_END), m_bOnCollision(false)
{

	m_eOBJ_ID = OBJ_PLAYER;
	m_iID = 0;		
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CCharacter(rhs)
	, m_pLeftPart(rhs.m_pLeftPart), m_pRightPart(rhs.m_pRightPart), m_pMiddlePart(rhs.m_pMiddlePart)
	, m_eWeaponState(SW_NONE), m_fMoveSpeed(100.f)
	, m_bFall(false), m_fVelocity(0.f), m_fJumpTime(0.f)
	, m_bJump(false), m_fJumpStartY(0.f), m_fJumpDuration(0.6f), m_fJumpHeight(20.f)
	, m_bDash(false), m_fDashTime(0.f), m_fDashDuration(0.3f), m_fDashDistance(80.f)
	, m_fKickAttack(1.f), m_pKickCollider(nullptr), m_pMainCollider(nullptr)
	, m_eNowState(MAIN_END), m_bOnCollision(false)
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

	if (FAILED(Add_PlayerPart()))
		return E_FAIL;

	m_pTransformCom->m_vScale = { 6.f,6.f,6.f };
	m_pTransformCom->Set_Pos(0.f, 0.f, 0.f);

	m_pMainCollider = m_pCollisionCom->CreateCollider(m_pTransformCom, m_szMainColliderName);
	m_pMainCollider->Set_Scale(_vec3(4, 11, 4));
	m_pMainCollider->BindFuncToCollision([&](CollisionInfo info)
		{
			OnCollision(info);
		});

	m_pKickCollider = m_pCollisionCom->CreateCollider(m_pTransformCom, m_szKickColliderName);
	m_pKickCollider->Set_Scale(_vec3(15.f, 11.f, 15.f));	
	//m_pKickCollider->OffCollision();	

	//m_eWeaponState = SW_PISTOL;
	m_eWeaponState = SW_KATANA;

	Change_State(INTRO);

	

	return S_OK;
}

_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CCharacter::Update_GameObject(fTimeDelta);

	

	Key_Input(fTimeDelta);

	if(m_bJump)
		Update_Jump(fTimeDelta);

	if(m_bDash)
		Update_Dash(fTimeDelta);

	m_mapWeapon[m_eWeaponState]->Update_GameObject(fTimeDelta);

	State_Update(fTimeDelta);

	return iExit;
}

void CPlayer::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_bFall)
	{
		Gravity(fTimeDelta);
	}
	else
	{
		m_fVelocity = 0.f;
	}

	CCharacter::LateUpdate_GameObject(fTimeDelta);

	CheckEnterCollider();

	Set_OnFloor(fTimeDelta);

	m_mapWeapon[m_eWeaponState]->LateUpdate_GameObject(fTimeDelta);
	State_LateUpdate(fTimeDelta);
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
	m_pLeftPart = CLeftPart::Create(m_pGraphicDev);
	if (nullptr == m_pLeftPart)
		return E_FAIL;
	m_pLeftPart->SetParent(this);

	m_pRightPart = CRightPart::Create(m_pGraphicDev);
	if (nullptr == m_pRightPart)
		return E_FAIL;
	m_pRightPart->SetParent(this);

	m_pMiddlePart = CMiddlePart::Create(m_pGraphicDev);
	if (nullptr == m_pMiddlePart)
		return E_FAIL;
	m_pMiddlePart->SetParent(this);

	CWeapon* pWeapon = nullptr;

	pWeapon = CPistol::Create(m_pGraphicDev);
	if (nullptr == pWeapon)
		return E_FAIL;

	Add_Weapon(SW_PISTOL, pWeapon);

	pWeapon = CKatana::Create(m_pGraphicDev);
	if (nullptr == pWeapon)
		return E_FAIL;

	Add_Weapon(SW_KATANA, pWeapon);

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
			if (m_eNowState == IDLE || m_eNowState == ATTACK)
			{
				Change_State(ATTACK);
				return;
			}
				
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
			m_vDashDir = vRight + vLook * -1.f;
			break;
		}
		D3DXVec3Normalize(&m_vDashDir, &m_vDashDir);
		m_vDashDir.y = 0.f;
		m_bDash = true;

		return;
	}

	//점프
	if (CDInputMgr::GetInstance()->Key_Down(DIK_SPACE) && !m_bJump && !m_bDash && !m_bFall)
	{
		m_fJumpTime = 0.f;
		m_fVelocity = 0.f;
		m_fJumpStartY = m_pTransformCom->m_vInfo[INFO_POS].y;
		m_bJump = true;
		return;
	}


	// 발차기
	if (CDInputMgr::GetInstance()->Key_Down(DIK_LSHIFT))
	{
		if (m_eNowState == IDLE)
			Change_State(SLIDE);
		else
			Change_State(IDLE);
		return;
	}

	if (CDInputMgr::GetInstance()->Key_Down(DIK_E))
	{
		if(m_eNowState == IDLE)
			Change_State(KICK);
		return;
	}

	if (CDInputMgr::GetInstance()->Key_Down(DIK_Q))
	{
		if (m_eNowState == IDLE)
			Change_State(DRINK);
		return;
	}

	// 장전
	if (CDInputMgr::GetInstance()->Key_Down(DIK_R))
	{
		if (m_eNowState == IDLE)
			Change_State(RELOAD);
		return;
	}
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

void CPlayer::CheckKickedMonster(COLLIDER_TAG eTag, _float fAttack)
{
	list<pair<_float, CCollider*>> pickedList;	
	CollisionInfo info = { NULL, {0,0,0}, fAttack, eTag };

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
			bool bPicked = CCollision::CheckCollision(m_pKickCollider, pairCollider.second);
			if (bPicked)
			{
				pickedList.push_back({ iter->second->Get_ViewZ() ,pairCollider.second });
			}
		}
	}

	if (pickedList.empty()) 
		return;

	for (auto& obj : pickedList)
	{
		obj.second->Collision(info);
	}

}

void CPlayer::CheckEnterCollider()
{
	list<pair<_float, CCollider*>> pickedList;
	CollisionInfo info = { NULL, {0,0,0}, 0.f, TAG_NONE };

	CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"Environment_Layer");
	if (!pLayer) return;

	auto pairIter = pLayer->Get_Objects(OBJ_COL);

	_vec3 vPos = *m_pTransformCom->Get_Info(INFO_POS);
	_vec3 vColPos = {};
	_vec3 vDiff = {};
	_vec3 vDir = {};
	_int iCallCount = 0;

	for (auto iter = pairIter.first; iter != pairIter.second; iter++)
	{
		CCollision* pCollision = static_cast<CCollision*>(iter->second->Get_Component(ID_DYNAMIC, L"Com_Collision"));
		if (!pCollision) continue;

		auto& mapCollider = pCollision->GetColliderMap();
		if (mapCollider.empty()) continue;

		for (auto& pairCollider : mapCollider)
		{
			vColPos = pairCollider.second->Get_ParentPos();				
			//vColPos = pairCollider.second->Get_RelativePos();				
			vDir = vColPos - vPos;
			bool bPicked = CCollision::CheckCollision_Diff(m_pMainCollider, pairCollider.second, &vDiff);
			if (bPicked)
			{
				if (vDiff.x < vDiff.y && vDiff.x < vDiff.z)
				{
					float dir = (vDir.x < 0.f) ? 1.f : -1.f;
					vPos.x += vDiff.x * dir;
					
					m_bDash = false;
					iCallCount++;
				}
				else if (vDiff.y < vDiff.x && vDiff.y < vDiff.z)
				{
					float dir = (vDir.y > 0.f) ? -1.f : 1.f;
					vPos.y += vDiff.y * dir;			

					if (dir < 0.f)
					{
						m_bJump = false;
						m_bDash = false;
						m_bFall = true;
						m_fVelocity = 0.f;
					}
					else
					{
						m_bJump = false;						
						m_bFall = false;
						m_fVelocity = 0.f;
						iCallCount++;
					}

				}
				else
				{
					float dir = (vDir.z < 0.f) ? 1.f : -1.f;
					vPos.z += vDiff.z * dir;


					m_bDash = false;
					iCallCount++;

				}						

				
				m_pTransformCom->Set_Pos(vPos);
			}
		}
	}
	
	m_bOnCollision = iCallCount > 0;
}

void CPlayer::Gravity(const _float& fTimeDelta)
{
	_float fVelocity = Get_Velocity();
	fVelocity -= 9.81f * (fTimeDelta + 0.25f);
	m_fVelocity = fVelocity;
}

_bool CPlayer::CheckOnFloor(_float* pHeight)
{
	CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"Environment_Layer");
	if (!pLayer) 
		return false;

	_vec3 vPosition = m_pTransformCom->m_vInfo[INFO_POS];
	auto pairIter = pLayer->Get_Objects(OBJ_FLOOR);
	for (auto iter = pairIter.first; iter != pairIter.second; iter++)
	{
		CTransform* pTransform = static_cast<CTransform*>(iter->second->Get_Component(ID_STATIC, L"Com_Transform"));

		if (pTransform && pTransform->Check_OnRange(&vPosition, pHeight))
			return true;
	}

	return false;
}

void CPlayer::Set_OnFloor(const _float& fTimeDelta)
{
	_float fHeight = 0.f;
	_vec3 vPosition = m_pTransformCom->m_vInfo[INFO_POS];
	//_float fBottom = vPosition.y - m_pTransformCom->Get_Scale().y;
	_float fBottom = vPosition.y - 15.f;
	

	if (CheckOnFloor(&fHeight))
	{
		if (m_bJump || m_bDash)
		{			
			return;
		}
		else if (m_bFall)
		{			
			vPosition.y += m_fVelocity * fTimeDelta;			
			fBottom = vPosition.y - 15.f;

			if (fBottom <= fHeight)
			{
				m_bFall = false;
				//vPosition.y = fHeight + m_pTransformCom->Get_Scale().y;
				vPosition.y = fHeight + 15.f;
			}

		}
		else
		{
			if (false == m_bOnCollision)
			{
				//vPosition.y = fHeight + m_pTransformCom->Get_Scale().y;
				vPosition.y = fHeight + 15.f;
			}
				

		}
	}
	else
	{
		if (!m_bOnCollision)
		{
			vPosition.y += m_fVelocity * fTimeDelta;
			m_bFall = true;
		}
			
	}

	

	m_pTransformCom->Set_Pos(vPosition);
}

_bool CPlayer::Get_OnFloor()
{
	_float fHeight = 0.f;
	_vec3 vPosition = m_pTransformCom->m_vInfo[INFO_POS];
	_float fBottom = vPosition.y - 15.f;

	if (CheckOnFloor(&fHeight))
	{
		if (fBottom <= fHeight)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

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

void CPlayer::Intro_Func()
{
	//m_pMiddlePart->ChangeState(GetStateID(INTRO, SW_END));
	switch (m_eWeaponState)
	{
	case CPlayer::SW_NONE:
		break;
	case CPlayer::SW_PISTOL:
		break;
	case CPlayer::SW_SHOTGUN:
		break;
	case CPlayer::SW_KATANA:
		static_cast<CKatana*>(m_mapWeapon[m_eWeaponState])->ChangeState(INTRO);
		break;
	case CPlayer::SW_END:
		break;
	default:
		break;
	}
	
}

void CPlayer::Fire_Func()
{
	m_mapWeapon[m_eWeaponState]->Fire();
	CheckPickedMonster();
}

void CPlayer::Katana_Func()
{
	m_pKickCollider->OnCollision();
	CheckKickedMonster(TAG_NONE, m_mapWeapon[m_eWeaponState]->Get_Power());
}

void CPlayer::Reload_Func()
{
	m_mapWeapon[m_eWeaponState]->Reload();
}

void CPlayer::Kick_Func()
{
	m_pKickCollider->OnCollision();
	CheckKickedMonster(TAG_KICK,m_fKickAttack);
}

void CPlayer::Slide_Func()
{
	CheckKickedMonster(TAG_KICK, m_fKickAttack);
}

void CPlayer::Change_State(_uint eState)
{
	if (eState == m_eNowState)
		return;
	
	if (--m_mapCallCnt[eState] > 0)
		return;

	if (m_eNowState != MAIN_END)
		State_Exit();

	m_eNowState = (STATE_MAIN)eState;
	State_Enter();
}

void CPlayer::State_Enter()
{
	switch (m_eNowState)
	{
	case CPlayer::INTRO:
		Intro_Enter();
		break;
	case CPlayer::IDLE:
		Idle_Enter();
		break;
	case CPlayer::RELOAD:
		Reload_Enter();
		break;
	case CPlayer::ATTACK:
		Attack_Enter();
		break;
	case CPlayer::KICK:
		Kick_Enter();
		break;
	case CPlayer::DRINK:
		Drink_Enter();
		break;
	case CPlayer::SLIDE:
		Slide_Enter();
		break;
	}
}

void CPlayer::State_Update(const _float& fTimeDelta)
{
	switch (m_eNowState)
	{
	case CPlayer::INTRO:
		Intro_Update(fTimeDelta);
		break;
	case CPlayer::IDLE:
		Idle_Update(fTimeDelta);
		break;
	case CPlayer::RELOAD:
		Reload_Update(fTimeDelta);
		break;
	case CPlayer::ATTACK:
		Attack_Update(fTimeDelta);
		break;
	case CPlayer::KICK:
		Kick_Update(fTimeDelta);
		break;
	case CPlayer::DRINK:
		Drink_Update(fTimeDelta);
		break;
	case CPlayer::SLIDE:
		Slide_Update(fTimeDelta);
		break;
	}
}

void CPlayer::State_LateUpdate(const _float& fTimeDelta)
{
	switch (m_eNowState)
	{
	case CPlayer::INTRO:
		Intro_LateUpdate(fTimeDelta);
		break;
	case CPlayer::IDLE:
		Idle_LateUpdate(fTimeDelta);
		break;
	case CPlayer::RELOAD:
		Reload_LateUpdate(fTimeDelta);
		break;
	case CPlayer::ATTACK:
		Attack_LateUpdate(fTimeDelta);
		break;
	case CPlayer::KICK:
		Kick_LateUpdate(fTimeDelta);
		break;
	case CPlayer::DRINK:
		Drink_LateUpdate(fTimeDelta);
		break;
	case CPlayer::SLIDE:
		Slide_LateUpdate(fTimeDelta);
		break;
	}
}

void CPlayer::State_Exit()
{
	switch (m_eNowState)
	{
	case CPlayer::INTRO:
		Intro_Exit();
		break;
	case CPlayer::IDLE:
		Idle_Exit();
		break;
	case CPlayer::RELOAD:
		Reload_Exit();
		break;
	case CPlayer::ATTACK:
		Attack_Exit();
		break;
	case CPlayer::KICK:
		Kick_Exit();
		break;
	case CPlayer::DRINK:
		Drink_Exit();
		break;
	case CPlayer::SLIDE:
		Slide_Exit();
		break;
	}
}

void CPlayer::Intro_Enter()
{
	switch (m_eWeaponState)
	{
	case CPlayer::SW_NONE:
		//m_iCallCnt = 1;
		m_mapCallCnt[INTRO] = 1;
		m_pMiddlePart->ChangeState(GetStateID(INTRO, SW_PISTOL));
		break;
	case CPlayer::SW_PISTOL:
		m_mapCallCnt[INTRO] = 1;
		m_pMiddlePart->ChangeState(GetStateID(INTRO, SW_PISTOL));
		break;
	case CPlayer::SW_SHOTGUN:
		break;
	case CPlayer::SW_KATANA:
		m_mapCallCnt[INTRO] = 2;
		m_pRightPart->ChangeState(GetStateID(INTRO, SW_KATANA));
		m_pLeftPart->ChangeState(GetStateID(INTRO, SW_KATANA));	
		break;
	case CPlayer::SW_END:
		break;
	}
}

void CPlayer::Intro_Update(const _float& fTimeDelta)
{
	switch (m_eWeaponState)
	{
	case CPlayer::SW_NONE:
		m_pMiddlePart->Update_GameObject(fTimeDelta);
		break;
	case CPlayer::SW_PISTOL:
		m_pMiddlePart->Update_GameObject(fTimeDelta);
		break;
	case CPlayer::SW_SHOTGUN:
		break;
	case CPlayer::SW_KATANA:
		m_pRightPart->Update_GameObject(fTimeDelta);
		m_pLeftPart->Update_GameObject(fTimeDelta);
		break;
	case CPlayer::SW_END:
		break;
	}
}

void CPlayer::Intro_LateUpdate(const _float& fTimeDelta)
{
	switch (m_eWeaponState)
	{
	case CPlayer::SW_NONE:
		m_pMiddlePart->LateUpdate_GameObject(fTimeDelta);
		break;
	case CPlayer::SW_PISTOL:
		m_pMiddlePart->LateUpdate_GameObject(fTimeDelta);
		break;
	case CPlayer::SW_SHOTGUN:
		break;
	case CPlayer::SW_KATANA:
		m_pRightPart->LateUpdate_GameObject(fTimeDelta);
		m_pLeftPart->LateUpdate_GameObject(fTimeDelta);
		break;
	case CPlayer::SW_END:
		break;
	}
}

void CPlayer::Intro_Exit()
{
}

void CPlayer::Idle_Enter()
{
	m_mapCallCnt[IDLE] = 1;
	m_pLeftPart->ChangeState(IDLE);
	m_pRightPart->ChangeState(GetStateID(IDLE, m_eWeaponState));
	m_pMiddlePart->ChangeState(IDLE);
	//m_pRightPart->ChangeState(GetStateID(IDLE, SW_PISTOL));
}

void CPlayer::Idle_Update(const _float& fTimeDelta)
{
	m_pMiddlePart->Update_GameObject(fTimeDelta);
	m_pRightPart->Update_GameObject(fTimeDelta);
	m_pLeftPart->Update_GameObject(fTimeDelta);
}

void CPlayer::Idle_LateUpdate(const _float& fTimeDelta)
{
	m_pMiddlePart->LateUpdate_GameObject(fTimeDelta);
	m_pRightPart->LateUpdate_GameObject(fTimeDelta);
	m_pLeftPart->LateUpdate_GameObject(fTimeDelta);
}

void CPlayer::Idle_Exit()
{
}

void CPlayer::Reload_Enter()
{
	switch (m_eWeaponState)
	{
	case CPlayer::SW_NONE:

		break;
	case CPlayer::SW_PISTOL:
		m_mapCallCnt[RELOAD] = 2;
		m_mapWeapon[m_eWeaponState]->Set_ShootAble(false);
		m_pLeftPart->ChangeState(GetStateID(RELOAD, m_eWeaponState));
		m_pRightPart->ChangeState(GetStateID(RELOAD, m_eWeaponState));
		break;
	case CPlayer::SW_SHOTGUN:
		break;
	case CPlayer::SW_KATANA:
		m_mapCallCnt[RELOAD] = 0;
		Change_State(IDLE);
		break;
	case CPlayer::SW_END:
		break;
	}
}

void CPlayer::Reload_Update(const _float& fTimeDelta)
{
	m_pMiddlePart->Update_GameObject(fTimeDelta);
	m_pRightPart->Update_GameObject(fTimeDelta);
	m_pLeftPart->Update_GameObject(fTimeDelta);
}

void CPlayer::Reload_LateUpdate(const _float& fTimeDelta)
{
	m_pMiddlePart->LateUpdate_GameObject(fTimeDelta);
	m_pRightPart->LateUpdate_GameObject(fTimeDelta);
	m_pLeftPart->LateUpdate_GameObject(fTimeDelta);
}

void CPlayer::Reload_Exit()
{
}

void CPlayer::Attack_Enter()
{
	switch (m_eWeaponState)
	{
	case CPlayer::SW_NONE:
		break;
	case CPlayer::SW_PISTOL:
		m_mapCallCnt[ATTACK] = 1;
		m_pRightPart->ChangeState(GetStateID(ATTACK, m_eWeaponState));
		break;
	case CPlayer::SW_SHOTGUN:
		break;
	case CPlayer::SW_KATANA:
		m_mapCallCnt[ATTACK] = 1;
		m_mapWeapon[m_eWeaponState]->Fire();
		break;
	case CPlayer::SW_END:
		break;
	default:
		break;
	}
	//m_pRightPart->ChangeState(GetStateID(ATTACK, m_eWeaponState));
}

void CPlayer::Attack_Update(const _float& fTimeDelta)
{
	m_pMiddlePart->Update_GameObject(fTimeDelta);
	if(m_eWeaponState == SW_PISTOL)
		m_pRightPart->Update_GameObject(fTimeDelta);
	m_pLeftPart->Update_GameObject(fTimeDelta);
}

void CPlayer::Attack_LateUpdate(const _float& fTimeDelta)
{
	m_pMiddlePart->LateUpdate_GameObject(fTimeDelta);
	if (m_eWeaponState == SW_PISTOL)
		m_pRightPart->LateUpdate_GameObject(fTimeDelta);
	m_pLeftPart->LateUpdate_GameObject(fTimeDelta);
}

void CPlayer::Attack_Exit()
{

}

void CPlayer::Kick_Enter()
{
	m_mapCallCnt[KICK] = 1;
	m_pMiddlePart->ChangeState(KICK);
}

void CPlayer::Kick_Update(const _float& fTimeDelta)
{
	m_pMiddlePart->Update_GameObject(fTimeDelta);
	m_pRightPart->Update_GameObject(fTimeDelta);
	m_pLeftPart->Update_GameObject(fTimeDelta);
}

void CPlayer::Kick_LateUpdate(const _float& fTimeDelta)
{
	m_pMiddlePart->LateUpdate_GameObject(fTimeDelta);
	m_pRightPart->LateUpdate_GameObject(fTimeDelta);
	m_pLeftPart->LateUpdate_GameObject(fTimeDelta);
}

void CPlayer::Kick_Exit()
{
}

void CPlayer::Drink_Enter()
{
	m_mapCallCnt[DRINK] = 1;
	m_pMiddlePart->ChangeState(DRINK);
}

void CPlayer::Drink_Update(const _float& fTimeDelta)
{
	m_pMiddlePart->Update_GameObject(fTimeDelta);
	m_pRightPart->Update_GameObject(fTimeDelta);
}

void CPlayer::Drink_LateUpdate(const _float& fTimeDelta)
{
	m_pMiddlePart->LateUpdate_GameObject(fTimeDelta);
	m_pRightPart->LateUpdate_GameObject(fTimeDelta);
}

void CPlayer::Drink_Exit()
{
}

void CPlayer::Slide_Enter()
{
	m_mapCallCnt[SLIDE] = 1;
	m_pMiddlePart->ChangeState(SLIDE);
}

void CPlayer::Slide_Update(const _float& fTimeDelta)
{
	m_pMiddlePart->Update_GameObject(fTimeDelta);
	m_pRightPart->Update_GameObject(fTimeDelta);
	m_pLeftPart->Update_GameObject(fTimeDelta);
}

void CPlayer::Slide_LateUpdate(const _float& fTimeDelta)
{
	m_pMiddlePart->LateUpdate_GameObject(fTimeDelta);
	m_pRightPart->LateUpdate_GameObject(fTimeDelta);
	m_pLeftPart->LateUpdate_GameObject(fTimeDelta);
}

void CPlayer::Slide_Exit()
{
	m_pMiddlePart->ChangeState(IDLE);
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

void CPlayer::Add_Weapon(_byte eWeaponTag, CWeapon* pWeapon)
{
	if (m_mapWeapon.count((STATE_WEAPON)eWeaponTag) > 0)
		return;

	m_mapWeapon.insert({ (STATE_WEAPON)eWeaponTag , pWeapon });

	pWeapon->Set_Parent(m_pMiddlePart);
	pWeapon->Set_WeaponState(eWeaponTag);

	if (m_mapWeapon.size() == 1)
		pWeapon->Set_Select(true);
}

void CPlayer::Change_Weapon(_byte eWeaponTag)
{
	if (m_mapWeapon.count((STATE_WEAPON)eWeaponTag) <= 0)
		return;

	m_mapWeapon[m_eWeaponState]->Set_Select(false);
	m_eWeaponState = (STATE_WEAPON)eWeaponTag;
	m_mapWeapon[m_eWeaponState]->Set_Select(true);
	
	Change_State(INTRO);
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

void CPlayer::Free()
{	
	CCharacter::Free();
	Safe_Release(m_pLeftPart);
	Safe_Release(m_pRightPart);
	Safe_Release(m_pMiddlePart);
	for_each(m_mapWeapon.begin(), m_mapWeapon.end(), CDeleteMap());
	m_mapWeapon.clear();
}

void CPlayer::OnCollision(CollisionInfo info)
{

}