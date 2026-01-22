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

#include "CEventMgr.h"
//#include "CShopBG.h"
#include "CHitUI.h"
#include "CPoolMgr.h"
#include "CFloor.h"
#include "CUIManager.h"
#include "CMapCollider.h"
#include "CMonster.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCharacter(pGraphicDev, 15.f)
	, m_pLeftPart(nullptr), m_pRightPart(nullptr), m_pMiddlePart(nullptr)
	, m_eWeaponState(WEAPON_NONE), m_fMoveSpeed(100.f)
	, m_bFall(false), m_fVelocity(0.f), m_fJumpTime(0.f)
	, m_bJump(false), m_fJumpStartY(0.f), m_fJumpDuration(0.6f), m_fJumpHeight(20.f)
	, m_bDash(false), m_fDashTime(0.f), m_fDashDuration(0.3f), m_fDashDistance(80.f)
	, m_fKickAttack(1.f), m_pKickCollider(nullptr), m_pMainCollider(nullptr)
	, m_eNowState(MAIN_END), m_bOnCollision(false), m_pHitUI(nullptr)
	, m_fHP(20.f), m_fMaxHP(20.f), m_fTime(0.f), m_fStageTime(0.f), m_bStage(false)
	, m_bSlope(false), m_bSideDash(false)
	, m_pColHitObj(nullptr),m_pTakeDownObject(nullptr), m_pTakeDownCollider(nullptr)
{

	m_eOBJ_ID = OBJ_PLAYER;
	m_iID = 0;
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CCharacter(rhs)
	, m_pLeftPart(rhs.m_pLeftPart), m_pRightPart(rhs.m_pRightPart), m_pMiddlePart(rhs.m_pMiddlePart)
	, m_eWeaponState(WEAPON_NONE), m_fMoveSpeed(100.f)
	, m_bFall(false), m_fVelocity(0.f), m_fJumpTime(0.f)
	, m_bJump(false), m_fJumpStartY(0.f), m_fJumpDuration(0.6f), m_fJumpHeight(20.f)
	, m_bDash(false), m_fDashTime(0.f), m_fDashDuration(0.3f), m_fDashDistance(80.f)
	, m_fKickAttack(1.f), m_pKickCollider(nullptr), m_pMainCollider(nullptr)
	, m_eNowState(MAIN_END), m_bOnCollision(false), m_pHitUI(nullptr)
	, m_fHP(20.f), m_fMaxHP(20.f), m_fTime(0.f), m_fStageTime(0.f), m_bStage(false)
	, m_bSlope(false), m_bSideDash(false)
	, m_pColHitObj(nullptr), m_pTakeDownObject(nullptr), m_pTakeDownCollider(nullptr)
{
	m_eOBJ_ID = OBJ_PLAYER;
	m_iID = 0;
}

CPlayer::~CPlayer()
{
}

void CPlayer::OnEvent(EVENT_TYPE _type, EventData* _pData)
{
	if (_type == EVENT_STAGE_START)
	{
		m_fHP = m_fMaxHP;
		m_fTime = 0.f;
		m_fStageTime = 0.f;
		m_bStage = true;
	}

	else if (_type == EVENT_STAGE_END)
	{
		m_bStage = false;
		Change_State(SHOP);
	}

	else if (_type == EVENT_READY_NEXT_STAGE)
	{
		Change_State(READY_NEXT);
	}

	else if (_type == EVENT_MONSTER_DEAD)
	{
		MonsterData* pData = static_cast<MonsterData*>(_pData);
		CUIManager::GetInstance()->Create_TextUI(m_pGraphicDev, pData->eTag, pData->value);
		Add_HP(pData->value);
	}
	
	else if (_type == EVENT_DRINK)
	{
		Drink_Func();
	}

	else if (_type == EVENT_TAKEDOWN_END )
	{
		m_pTakeDownObject->StartUpdate();

		//처형 후 Slide로 이어짐 
		CollisionInfo info = { this, _vec3(),10,TAG_SLIDE };
		m_pTakeDownCollider->Collision(info);
		//m_pTakeDownObject->Make_DeadText(TAG_TAKEDOWN, 10);
		m_pTakeDownObject = nullptr;
		m_pTakeDownCollider = nullptr;
	}

}

HRESULT CPlayer::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	if (FAILED(Add_PlayerPart()))
		return E_FAIL;

	CEventMgr::GetInstance()->Subscribe(EVENT_DOOR_IN, this);
	CEventMgr::GetInstance()->Subscribe(EVENT_MONSTER_DEAD, this);
	CEventMgr::GetInstance()->Subscribe(EVENT_STAGE_START, this);
	CEventMgr::GetInstance()->Subscribe(EVENT_STAGE_END, this);
	CEventMgr::GetInstance()->Subscribe(EVENT_READY_NEXT_STAGE, this);
	CEventMgr::GetInstance()->Subscribe(EVENT_DRINK, this);
	CEventMgr::GetInstance()->Subscribe(EVENT_TAKEDOWN_END, this);

	m_pTransformCom->m_vScale = { 6.f,6.f,6.f };
	m_pTransformCom->Set_Pos(0.f, 0.f, 0.f);

	m_pMainCollider = m_pCollisionCom->CreateCollider(this, m_szMainColliderName);
	m_pMainCollider->Set_Scale(_vec3(4, 15, 4));
	m_pMainCollider->BindFuncToCollision([&](CollisionInfo info)
		{
			OnCollision(info);
		});
	//TODO : 추가 (방)
	m_pCollisionCom->SetMainCollider(m_szMainColliderName);


	m_pKickCollider = m_pCollisionCom->CreateCollider(this, m_szKickColliderName);
	m_pKickCollider->Set_Scale(_vec3(15.f, 15.f, 15.f));
	//m_pKickCollider->OffCollision();	

	m_eWeaponState = WEAPON_PISTOL;
	//m_eWeaponState = WEAPON_KATANA;

	Change_State(INTRO);

	if (m_pHitUI) return E_FAIL;

	return S_OK;
}

_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CCharacter::Update_GameObject(fTimeDelta);

	if (m_bStage)
	{
		m_fTime += fTimeDelta;
		m_fStageTime += fTimeDelta;

		if (m_fTime >= 1.f)
		{
			Add_HP(-1.f);
			m_fTime = 0.f;
		}
	}

	if (m_eNowState == SLIDE && m_bSlope == false)
	{
		Change_State(IDLE);
	}

	if (m_bSideDash)
		Update_SideDash(fTimeDelta);


	if (m_eNowState != SHOP && m_eNowState != READY_NEXT && m_bStage)
		Key_Input(fTimeDelta);

	if (m_bJump)
		Update_Jump(fTimeDelta);

	if (m_bDash)
		Update_Dash(fTimeDelta);



	if (m_eNowState != SHOP && m_eNowState != READY_NEXT)
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

	//CheckEnterCollider();
	if (false == m_bSideDash)
		Set_OnFloor(fTimeDelta);

	if (m_eNowState != SHOP && m_eNowState != READY_NEXT)
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

	Add_Weapon(WEAPON_PISTOL, pWeapon);

	pWeapon = CKatana::Create(m_pGraphicDev);
	if (nullptr == pWeapon)
		return E_FAIL;

	Add_Weapon(WEAPON_KATANA, pWeapon);


	// UI 
	return S_OK;
}

void CPlayer::Key_Input(const _float& fTimeDelta)
{
	//Engine::CTransform* pTransform = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()->Get_Component(ID_DYNAMIC, L"Environment_Layer", OBJ_CAM, L"Com_Transform"));
	Engine::CTransform* pTransform = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", OBJ_CAM, L"Com_Transform"));
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
		CUIManager::GetInstance()->Set_OnDashUI(true);
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
}

void CPlayer::Action_Input(const _float& fTimeDelta, const _vec3& vLook)
{
	//if (m_eNowState != IDLE && m_eNowState != DRINK)
	//	return;
	//
	if (CDInputMgr::GetInstance()->Key_Down(DIK_R))
	{
		if (m_eNowState == IDLE && !m_pLeftPart->Get_Relaod() && !m_pRightPart->Get_Reload())
			Change_State(RELOAD);
		return;
	}

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
		if (m_eNowState == IDLE)
			Change_State(KICK);
		return;
	}

	if (CDInputMgr::GetInstance()->Key_Down(DIK_Q))
	{

		CEventMgr::GetInstance()->Broadcast(EVENT_DRINK, nullptr);
		
		return;
	}

	//방승희 추가 
	//근접 처형 Take Down
	if (CDInputMgr::GetInstance()->Key_Down(DIK_F))
	{
		if (CheckTakeDownMonster(&m_pTakeDownObject, &m_pTakeDownCollider))
		{
			CUIManager::GetInstance()->Change_UIState(UI_TAKEDOWN);
			//TOOD : 이벤트 끝날 때까지 키 입력 막기 
			//몬스터 업데이트 중단을 통해 랜더 + 액션 증딘시킴 
			m_pTakeDownObject->StopUpdate();
		}
	}


	// 장전
	if (CDInputMgr::GetInstance()->Key_Down(DIK_1))
	{
		if (m_eWeaponState != WEAPON_PISTOL)
			Change_Weapon(WEAPON_PISTOL);
		return;
	}
	if (CDInputMgr::GetInstance()->Key_Down(DIK_2))
	{
		CEventMgr::GetInstance()->Broadcast(EVENT_STAGE_END, nullptr);
		return;
	}

	if (CDInputMgr::GetInstance()->Key_Down(DIK_3))
	{
		if (m_eWeaponState != WEAPON_KATANA)
			Change_Weapon(WEAPON_KATANA);
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
	_vec3	vLook, vPos;
	_float cosFov = cosf(D3DXToRadian(60.f));
	vLook = *m_pTransformCom->Get_Info(INFO_LOOK);
	vPos = *m_pTransformCom->Get_Info(INFO_POS);

	D3DXVec3Normalize(&vLook, &vLook);

	CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");
	if (!pLayer) return;

	auto pairIter = pLayer->Get_Objects(OBJ_MONSTER);
	//multimap<OBJ_ID, CGameObject*> 에 대한 반복자
	//OBJ_ID를 키로 가진 오브젝트들의 반복자 범위를 반환 = 몬스터 전체 목록
	for (auto iter = pairIter.first; iter != pairIter.second; iter++)
	{
		CCollision* pCollision = static_cast<CCollision*>(iter->second->Get_Component(ID_DYNAMIC, L"Com_Collision"));
		CTransform* pTransform = static_cast<CTransform*>(iter->second->Get_Component(ID_DYNAMIC, L"Com_Transform"));

		if (!pCollision) continue;
		if (!pTransform) continue;

		_vec3 vDir = *pTransform->Get_Info(INFO_POS) - vPos;
		D3DXVec3Normalize(&vDir, &vDir);

		_float fDot = D3DXVec3Dot(&vLook, &vDir);

		auto& mapCollider = pCollision->GetColliderMap();
		if (mapCollider.empty()) continue;
		//몬스터의 CollisionCom에 있는 전체 Collider 
		for (auto& pairCollider : mapCollider)
		{
			bool bPicked = CCollision::CheckCollision(m_pKickCollider, pairCollider.second);
			if (bPicked)
			{
				if (fDot >= cosFov)
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

bool CPlayer::CheckTakeDownMonster(CMonster** _pOut, CCollider** _pOutCollider)
{
	_vec3	vLook, vPos;
	_float cosFov = cosf(D3DXToRadian(60.f));
	vLook = *m_pTransformCom->Get_Info(INFO_LOOK);
	vPos = *m_pTransformCom->Get_Info(INFO_POS);

	D3DXVec3Normalize(&vLook, &vLook);

	CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");
	if (!pLayer) return false;

	auto pairIter = pLayer->Get_Objects(OBJ_MONSTER);
	//multimap<OBJ_ID, CGameObject*> 에 대한 반복자
	//OBJ_ID를 키로 가진 오브젝트들의 반복자 범위를 반환 = 몬스터 전체 목록
	for (auto iter = pairIter.first; iter != pairIter.second; iter++)
	{
		CMonster* monster = static_cast<CMonster*>(iter->second);
		if (!monster || !monster->CanTakeDown()) continue;

		CCollision* pCollision = static_cast<CCollision*>(iter->second->Get_Component(ID_DYNAMIC, L"Com_Collision"));
		CTransform* pTransform = static_cast<CTransform*>(iter->second->Get_Component(ID_DYNAMIC, L"Com_Transform"));

		if (!pCollision) continue;
		if (!pTransform) continue;

		_vec3 vDir = *pTransform->Get_Info(INFO_POS) - vPos;
		D3DXVec3Normalize(&vDir, &vDir);

		_float fDot = D3DXVec3Dot(&vLook, &vDir);

		CCollider * monCollider = pCollision->GetCollider();
		if (!monCollider) continue;

		bool bPicked = CCollision::CheckCollision(m_pKickCollider, monCollider);
		if (bPicked)
		{
			if (fDot >= cosFov)
			{
				*_pOut = monster;
				*_pOutCollider = monCollider;
				return true;
			}
		}
	}

	return false;
}

void CPlayer::Gravity(const _float& fTimeDelta)
{
	_float fVelocity = Get_Velocity();
	fVelocity -= 9.81f * (fTimeDelta + 0.25f);
	m_fVelocity = fVelocity;
}

_bool CPlayer::CheckOnFloor(const _float& fTimeDelta,_float* pHeight)
{
	CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"Environment_Layer");
	if (!pLayer) return false;
		

	_vec3 vPosition = m_pTransformCom->m_vInfo[INFO_POS];
	auto pairIter = pLayer->Get_Objects(OBJ_FLOOR);

	_float	fMaxY = -FLT_MAX;
	_bool	bFound = false;

	for (auto iter = pairIter.first; iter != pairIter.second; iter++)
	{
		CTransform* pTransform = static_cast<CTransform*>(iter->second->Get_Component(ID_STATIC, L"Com_Transform"));
		CGameObject* pGameObject = iter->second;
		COLLIDER_TAG eTag = static_cast<CFloor*>(pGameObject)->Get_ColliderTag();
		_float fCurrentFloorY = 0.f;


		if (eTag == TAG_NONE || eTag == TAG_ACID)
		{
			if (pTransform->Check_OnRange(&vPosition, &fCurrentFloorY))
			{
				if (fCurrentFloorY <= vPosition.y && fCurrentFloorY > fMaxY)
				{
					fMaxY = fCurrentFloorY;
					bFound = true;
				}
			}
		}
		else if (eTag == TAG_SLOPE)
		{
			{
				CRcTexUp* pRcTex = static_cast<CRcTexUp*>(iter->second->Get_Component(ID_STATIC, L"Com_Buffer"));
				_vec3 vPickPos;
				if (Picking_OnFloor(&vPickPos, pRcTex, pTransform))
				{
					_vec3 vNormal = { 0.f, 0.f, -1.f };
					_matrix matWorld = *pTransform->Get_World();
					D3DXVec3TransformNormal(&vNormal, &vNormal, &matWorld);
					D3DXVec3Normalize(&vNormal, &vNormal);

					_vec3 vLook = *m_pTransformCom->Get_Info(INFO_LOOK);
					D3DXVec3Normalize(&vLook, &vLook);

					_float fDot = D3DXVec3Dot(&vLook, &vNormal);


					if (fDot < 0.f) 
					{
						m_bSlope = false;
						*pHeight = vPickPos.y;
					}
					else if (fDot > 0.f) 
					{
						m_bSlope = true;
						_vec3 vSlopeDir;
						_float fSlopeInclination = D3DXVec3Dot(&vLook, &vNormal);
						vSlopeDir = vLook - (vNormal * fSlopeInclination);
						D3DXVec3Normalize(&vSlopeDir, &vSlopeDir);

						_vec3 vPos = m_pTransformCom->m_vInfo[INFO_POS];
						vPos += vSlopeDir * m_fMoveSpeed * fTimeDelta;

						vPos.y = vPickPos.y;

						m_pTransformCom->Set_Pos(vPos);
					}					
					return true;
				}
			}
		}
			
	}

	if (bFound)
	{
		m_bSlope = false;
		*pHeight = fMaxY;
		return true;
	}

	//TODO 제거하기 . BossSTage용 임시 코드 
	if (pairIter.second == pairIter.first)
	{
		*pHeight = 0.f;
		return true;
	}

	return false;
}

void CPlayer::Set_OnFloor(const _float& fTimeDelta)
{
	_float fHeight = 0.f;
	_vec3 vPosition = m_pTransformCom->m_vInfo[INFO_POS];
	//_float fBottom = vPosition.y - m_pTransformCom->Get_Scale().y;
	_float fBottom = vPosition.y - m_pMainCollider->Get_Scale().y;


	if (CheckOnFloor(fTimeDelta ,&fHeight))
	{
		
		if (m_bJump || m_bDash || m_bSideDash || m_bSlope)
		{
			if (m_bSlope)
			{
				Change_State(SLIDE);
			}			
			return;
		}
		else if (m_bFall)
		{
			vPosition.y += m_fVelocity * fTimeDelta;
			fBottom = vPosition.y - m_pMainCollider->Get_Scale().y;

			if (fBottom <= fHeight)
			{
				m_bFall = false;
				//vPosition.y = fHeight + m_pTransformCom->Get_Scale().y;
				vPosition.y = fHeight + m_pMainCollider->Get_Scale().y;
			}
		}
		else
		{
			fBottom = vPosition.y - m_pMainCollider->Get_Scale().y;
			if (fBottom > fHeight)
			{				
				m_bFall = true;
				vPosition.y += m_fVelocity * fTimeDelta;
			}
			else
			{
				vPosition.y = fHeight + m_pMainCollider->Get_Scale().y;
			}
		}
	}
	else
	{
		vPosition.y += m_fVelocity * fTimeDelta;
		m_bFall = true;
	}



	m_pTransformCom->Set_Pos(vPosition);
}

_bool CPlayer::Picking_OnFloor(_vec3* pHit, CRcTexUp* pFloorBufferCom, CTransform* pFloorTransformCom)
{
	_vec3       vRayPos = *m_pTransformCom->Get_Info(INFO_POS);
	vRayPos.y += 10.f;
	_vec3       vRayDir{ 0.f, -1.f, 0.f };
	D3DXVec3Normalize(&vRayDir, &vRayDir);


	_matrix matWorld = *(pFloorTransformCom->Get_World());
	D3DXMatrixInverse(&matWorld, 0, &matWorld);

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matWorld);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matWorld);
	D3DXVec3Normalize(&vRayDir, &vRayDir);

	const _vec3* pTerrainVtxPos = pFloorBufferCom->Get_VtxPos();
	_float  fU(0.f), fV(0.f), fDist(0.f);

	_vec3 vHit{};
	if (D3DXIntersectTri(
		&pTerrainVtxPos[0],
		&pTerrainVtxPos[1],
		&pTerrainVtxPos[2],
		&vRayPos, &vRayDir,
		&fU, &fV, &fDist))
	{
		vHit = vRayPos + vRayDir * fDist;
		D3DXVec3TransformCoord(&vHit, &vHit, pFloorTransformCom->Get_World());
		*pHit = vHit;
		return true;
	}


	if (D3DXIntersectTri(
		&pTerrainVtxPos[0],
		&pTerrainVtxPos[3],
		&pTerrainVtxPos[2],
		&vRayPos, &vRayDir,
		&fU, &fV, &fDist))
	{
		vHit = vRayPos + vRayDir * fDist;
		D3DXVec3TransformCoord(&vHit, &vHit, pFloorTransformCom->Get_World());
		*pHit = vHit;
		return true;
	}


	return false;
}

_float CPlayer::Compute_HeightOnFloor(const _vec3* pPos, const _vec3* pFloorVtxPos, const _ulong& dwCntX, const _ulong& dwCntZ)
{
	_ulong  dwIndex = _ulong(pPos->z / 1.f) * dwCntX + _ulong(pPos->x / 1.f);

	_float  fRatioX = (pPos->x - pFloorVtxPos[dwIndex + dwCntX].x) / 1.f;
	_float  fRatioZ = (pFloorVtxPos[dwIndex + dwCntX].z - pPos->z) / 1.f;

	D3DXPLANE   Plane;

	// 오른쪽 위
	if (fRatioX > fRatioZ)
	{
		D3DXPlaneFromPoints(&Plane,
			&pFloorVtxPos[dwIndex + dwCntX],
			&pFloorVtxPos[dwIndex + dwCntX + 1],
			&pFloorVtxPos[dwIndex + 1]);
	}

	// 왼쪽 아래
	else
	{
		D3DXPlaneFromPoints(&Plane,
			&pFloorVtxPos[dwIndex + dwCntX],
			&pFloorVtxPos[dwIndex + 1],
			&pFloorVtxPos[dwIndex]);
	}

	return (-Plane.a * pPos->x - Plane.c * pPos->z - Plane.d) / Plane.b;
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

		CUIManager::GetInstance()->Set_OnDashUI(false);
	}
	float easeOutQuad = 1.f - (1.f - t) * (1.f - t);
	_float dashDistance = easeOutQuad * m_fDashDistance;
	_vec3 newPos = m_vDashStart + m_vDashDir * dashDistance;

	m_pTransformCom->Set_Pos(newPos);
}

void CPlayer::Update_SideDash(const _float& fTimeDelta)
{

	CTransform* pTransform = static_cast<CTransform*>(m_pColHitObj->Get_Component(ID_STATIC, L"Com_Transform"));
	CCollision* pCollision = static_cast<CCollision*>(m_pColHitObj->Get_Component(ID_DYNAMIC, L"Com_Collision"));

	_vec3 vDirection = -m_vDiffDir;
	D3DXVec3Normalize(&vDirection, &vDirection);	

	_vec3 vPos = *m_pTransformCom->Get_Info(INFO_POS);
	_vec3 vDir = { 0.f,0.f,0.f };
	vPos.y = pTransform->Get_Info(INFO_POS)->y;

	COLLIDER_TAG eTag = static_cast<CMapCollider*>(m_pColHitObj)->Get_ColliderTag();
	if (eTag == TAG_SIDE_DASH_X)
	{
		vDir.x = 1.f;
	}
	else if (eTag == TAG_SIDE_DASH_Z)
	{
		vDir.z = 1.f;
	}

	vPos = vPos + vDir * m_fMoveSpeed * fTimeDelta;
	m_pTransformCom->Set_Pos(vPos);

	if (CCollision::Collision_Ray(pCollision->GetCollider(), *m_pTransformCom->Get_Info(INFO_POS), vDirection))
	{
		

	}
	else
	{
		m_bSideDash = false;		
		m_pColHitObj = nullptr;
		m_vDiffDir = {};

		m_fJumpTime = 0.f;
		m_fVelocity = 0.f;
		m_fJumpStartY = m_pTransformCom->m_vInfo[INFO_POS].y;
		CUIManager::GetInstance()->Set_OnDashUI(false);
		m_bJump = true;
	}

}

void CPlayer::Update_TickDamagaed(const _float& fTimeDelta)
{
}

void CPlayer::Intro_Func()
{
	switch (m_eWeaponState)
	{
	case WEAPON_NONE:
		break;
	case WEAPON_PISTOL:
		break;
	case WEAPON_SHOTGUN:
		break;
	case WEAPON_KATANA:
		static_cast<CKatana*>(m_mapWeapon[m_eWeaponState])->ChangeState(INTRO);
		break;
	case WEAPON_END:
		break;
	default:
		break;
	}
}

/// <summary>
/// 공격 이벤트
/// </summary>
void CPlayer::Fire_Func()
{
	m_mapWeapon[m_eWeaponState]->Fire();
	CheckPickedMonster();
}

/// <summary>
/// 카타나 충돌 이벤트
/// </summary>
void CPlayer::Katana_Func()
{
	m_pKickCollider->OnCollision();
	CheckKickedMonster(TAG_KATANA, m_mapWeapon[m_eWeaponState]->Get_Power());
}

void CPlayer::Reload_Func()
{
	m_mapWeapon[m_eWeaponState]->Reload();
}


/// <summary>
/// 발차기 충돌 이벤트
/// </summary>
void CPlayer::Kick_Func()
{
	m_pKickCollider->OnCollision();
	CheckKickedMonster(TAG_KICK, m_fKickAttack);
}

/// <summary>
/// 슬라이딩 이벤트 ( 콜라이더 충돌 ) 
/// </summary>
void CPlayer::Slide_Func()
{
	CheckKickedMonster(TAG_SLIDE, m_fKickAttack);
}

/// <summary>
/// 상점 이벤트 시작
/// </summary>
void CPlayer::Shop_Func()
{
	Change_State(SHOP);
}

/// <summary>
/// 드링킹 이벤트
/// </summary>
void CPlayer::Drink_Func()
{
	Add_HP(m_fMaxHP);

	if (m_eNowState == IDLE)
		Change_State(DRINK);
}

void CPlayer::Change_State(_uint eState)
{
	if (eState == m_eNowState)
		return;

	if (--m_mapCallCnt[eState] > 0)
		return;

	if (m_eNowState != MAIN_END)
		State_Exit();

	m_eNowState = (PLAYER_STATE)eState;
	State_Enter();
}

void CPlayer::State_Enter()
{
	switch (m_eNowState)
	{
	case INTRO:
		Intro_Enter();
		break;
	case IDLE:
		Idle_Enter();
		break;
	case RELOAD:
		Reload_Enter();
		break;
	case ATTACK:
		Attack_Enter();
		break;
	case KICK:
		Kick_Enter();
		break;
	case DRINK:
		Drink_Enter();
		break;
	case SLIDE:
		Slide_Enter();
		break;
	case SHOP:
		Shop_Enter();
		break;
	case READY_NEXT:
		Next_Enter();
		break;
	}
}

void CPlayer::State_Update(const _float& fTimeDelta)
{
	switch (m_eNowState)
	{
	case INTRO:
		Intro_Update(fTimeDelta);
		break;
	case IDLE:
		Idle_Update(fTimeDelta);
		break;
	case RELOAD:
		Reload_Update(fTimeDelta);
		break;
	case ATTACK:
		Attack_Update(fTimeDelta);
		break;
	case KICK:
		Kick_Update(fTimeDelta);
		break;
	case DRINK:
		Drink_Update(fTimeDelta);
		break;
	case SLIDE:
		Slide_Update(fTimeDelta);
		break;
	case SHOP:
		Shop_Update(fTimeDelta);
		break;
	case READY_NEXT:
		Next_Update(fTimeDelta);
		break;
	}
}

void CPlayer::State_LateUpdate(const _float& fTimeDelta)
{
	switch (m_eNowState)
	{
	case INTRO:
		Intro_LateUpdate(fTimeDelta);
		break;
	case IDLE:
		Idle_LateUpdate(fTimeDelta);
		break;
	case RELOAD:
		Reload_LateUpdate(fTimeDelta);
		break;
	case ATTACK:
		Attack_LateUpdate(fTimeDelta);
		break;
	case KICK:
		Kick_LateUpdate(fTimeDelta);
		break;
	case DRINK:
		Drink_LateUpdate(fTimeDelta);
		break;
	case SLIDE:
		Slide_LateUpdate(fTimeDelta);
		break;
	case SHOP:
		Shop_LateUpdate(fTimeDelta);
		break;
	case READY_NEXT:
		Next_LateUpdate(fTimeDelta);
		break;
	}
}

void CPlayer::State_Exit()
{
	switch (m_eNowState)
	{
	case INTRO:
		Intro_Exit();
		break;
	case IDLE:
		Idle_Exit();
		break;
	case RELOAD:
		Reload_Exit();
		break;
	case ATTACK:
		Attack_Exit();
		break;
	case KICK:
		Kick_Exit();
		break;
	case DRINK:
		Drink_Exit();
		break;
	case SLIDE:
		Slide_Exit();
		break;
	case SHOP:
		Shop_Exit();
		break;
	case READY_NEXT:
		Next_Exit();
		break;
	}
}

void CPlayer::Intro_Enter()
{
	switch (m_eWeaponState)
	{
	case WEAPON_NONE:
		//m_iCallCnt = 1;
		m_mapCallCnt[INTRO] = 1;
		m_pMiddlePart->ChangeState(GetStateID(INTRO, WEAPON_PISTOL));
		break;
	case WEAPON_PISTOL:
		m_mapCallCnt[INTRO] = 1;
		m_pMiddlePart->ChangeState(GetStateID(INTRO, WEAPON_PISTOL));
		break;
	case WEAPON_SHOTGUN:
		break;
	case WEAPON_KATANA:
		m_mapCallCnt[INTRO] = 1;
		m_pRightPart->ChangeState(GetStateID(INTRO, WEAPON_KATANA));
		m_pLeftPart->ChangeState(GetStateID(INTRO, WEAPON_KATANA));
		break;
	case WEAPON_END:
		break;
	}
}

void CPlayer::Intro_Update(const _float& fTimeDelta)
{
	switch (m_eWeaponState)
	{
	case WEAPON_NONE:
		m_pMiddlePart->Update_GameObject(fTimeDelta);
		break;
	case WEAPON_PISTOL:
		m_pMiddlePart->Update_GameObject(fTimeDelta);
		break;
	case WEAPON_SHOTGUN:
		break;
	case WEAPON_KATANA:
		m_pRightPart->Update_GameObject(fTimeDelta);
		m_pLeftPart->Update_GameObject(fTimeDelta);
		break;
	case WEAPON_END:
		break;
	}
}

void CPlayer::Intro_LateUpdate(const _float& fTimeDelta)
{
	switch (m_eWeaponState)
	{
	case WEAPON_NONE:
		m_pMiddlePart->LateUpdate_GameObject(fTimeDelta);
		break;
	case WEAPON_PISTOL:
		m_pMiddlePart->LateUpdate_GameObject(fTimeDelta);
		break;
	case WEAPON_SHOTGUN:
		break;
	case WEAPON_KATANA:
		m_pRightPart->LateUpdate_GameObject(fTimeDelta);
		m_pLeftPart->LateUpdate_GameObject(fTimeDelta);
		break;
	case WEAPON_END:
		break;
	}
}

void CPlayer::Intro_Exit()
{
	CEventMgr::GetInstance()->Broadcast(EVENT_STAGE_START, nullptr);
}

void CPlayer::Idle_Enter()
{
	m_mapCallCnt[IDLE] = 1;
	m_pLeftPart->ChangeState(IDLE);
	m_pRightPart->ChangeState(GetStateID(IDLE, m_eWeaponState));
	m_pMiddlePart->ChangeState(IDLE);

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
	case WEAPON_NONE:

		break;
	case WEAPON_PISTOL:
		m_mapCallCnt[RELOAD] = 2;
		m_mapWeapon[m_eWeaponState]->Set_ShootAble(false);
		m_pLeftPart->ChangeState(GetStateID(RELOAD, m_eWeaponState));
		m_pRightPart->ChangeState(GetStateID(RELOAD, m_eWeaponState));
		break;
	case WEAPON_SHOTGUN:
		break;
	case WEAPON_KATANA:
		m_mapCallCnt[RELOAD] = 0;
		Change_State(IDLE);
		break;
	case WEAPON_END:
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
	case WEAPON_NONE:
		break;
	case WEAPON_PISTOL:
		m_mapCallCnt[ATTACK] = 1;
		m_pRightPart->ChangeState(GetStateID(ATTACK, m_eWeaponState));
		break;
	case WEAPON_SHOTGUN:
		break;
	case WEAPON_KATANA:
		m_mapCallCnt[ATTACK] = 1;
		m_mapWeapon[m_eWeaponState]->Fire();
		break;
	case WEAPON_END:
		break;
	default:
		break;
	}
	//m_pRightPart->ChangeState(GetStateID(ATTACK, m_eWeaponState));
}

void CPlayer::Attack_Update(const _float& fTimeDelta)
{
	m_pMiddlePart->Update_GameObject(fTimeDelta);
	if (m_eWeaponState == WEAPON_PISTOL)
		m_pRightPart->Update_GameObject(fTimeDelta);
	m_pLeftPart->Update_GameObject(fTimeDelta);
}

void CPlayer::Attack_LateUpdate(const _float& fTimeDelta)
{
	m_pMiddlePart->LateUpdate_GameObject(fTimeDelta);
	if (m_eWeaponState == WEAPON_PISTOL)
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
	m_mapCallCnt[DRINK] = 0;
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
	CUIManager::GetInstance()->Set_OnDashUI(true);
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
	CUIManager::GetInstance()->Set_OnDashUI(false);
}

void CPlayer::Shop_Enter()
{
	
	m_pMiddlePart->ChangeState(SHOP);
	//m_pShopBG->ChangeState(SHOP);
}

void CPlayer::Shop_Update(const _float& fTimeDelta)
{
	m_pMiddlePart->Update_GameObject(fTimeDelta);
	//m_pShopBG->Update_GameObject(fTimeDelta);
}

void CPlayer::Shop_LateUpdate(const _float& fTimeDelta)
{
	m_pMiddlePart->LateUpdate_GameObject(fTimeDelta);
	//m_pShopBG->LateUpdate_GameObject(fTimeDelta);
}

void CPlayer::Shop_Exit()
{
}

void CPlayer::Next_Enter()
{
	m_pMiddlePart->ChangeState(READY_NEXT);
}

void CPlayer::Next_Update(const _float& fTimeDelta)
{
	m_pMiddlePart->Update_GameObject(fTimeDelta);

}

void CPlayer::Next_LateUpdate(const _float& fTimeDelta)
{
	m_pMiddlePart->LateUpdate_GameObject(fTimeDelta);
}

void CPlayer::Next_Exit()
{
	CEventMgr::GetInstance()->Broadcast(EVENT_NEXT_STAGE, nullptr);
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
	if (m_mapWeapon.count((WEAPON_STATE)eWeaponTag) > 0)
		return;

	m_mapWeapon.insert({ (WEAPON_STATE)eWeaponTag , pWeapon });

	pWeapon->Set_Parent(m_pMiddlePart);
	pWeapon->Set_WeaponState(eWeaponTag);

	if (m_mapWeapon.size() == 1)
		pWeapon->Set_Select(true);
}

void CPlayer::Change_Weapon(_byte eWeaponTag)
{
	if (m_mapWeapon.count((WEAPON_STATE)eWeaponTag) <= 0)
		return;

	m_mapWeapon[m_eWeaponState]->Set_Select(false);
	m_eWeaponState = (WEAPON_STATE)eWeaponTag;
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


void CPlayer::Move_ByCollision(COL_DIR& dir, _vec3 _diff)
{
	_vec3 vPos = *m_pTransformCom->Get_Info(INFO_POS);
	if (dir == CDIR_X)
	{
		vPos.x += _diff.x;
		m_bDash = false;
		m_bOnCollision = true;
	}
	else if (dir == CDIR_Y)
	{
		vPos.y += _diff.y;
		if (_diff.y < 0.f)
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
			m_bOnCollision = true;
		}
	}
	else
	{
		vPos.z += _diff.z;
		m_bDash = false;
		m_bOnCollision = true;
	}

	m_pTransformCom->Set_Pos(vPos);
}


void CPlayer::OnCollision(CollisionInfo info)
{
	if (info.eDir != CDIR_NONE)
	{
		Move_ByCollision(info.eDir, info.vDiff);

		if (info.eTag == TAG_SIDE_DASH_X || info.eTag == TAG_SIDE_DASH_Z)
		{
			m_bSideDash = true;
			m_pColHitObj = info.pTarget;

			m_vDiffDir = info.vDiff;

			switch (info.eDir)
			{
			case CDIR_NONE:
				break;
			case CDIR_X:
				m_vDiffDir = { info.vDiff.x, 0.f,0.f };
				break;
			case CDIR_Z:
				m_vDiffDir = { 0.f, 0.f,info.vDiff.z };
				break;
			default:
				break;
			}
			CUIManager::GetInstance()->Set_OnDashUI(true);
			m_bJump = false;
			m_bFall = false;
		}		
	}

	//방승희 추가. 이펙트 용 임시 코드
	//TODO : Damage에 따라 상태 변경 또는 함수 호출하기. 
	if (info.fDamage > 0.f)
	{
		if (m_pHitUI==nullptr || m_pHitUI->IsDead())
		{
			m_pHitUI = CPoolMgr::GetInstance()->Get_Object<CHitUI>();
			m_pHitUI->Reset();
			CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer")->Add_GameObject(m_pHitUI);
		}
	}

}


void CPlayer::Get_Hit(_float fDamage)
{
	Add_HP(-fDamage);
	// Effect
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
