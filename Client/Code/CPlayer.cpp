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
	: CCharacter(pGraphicDev)
	, m_pLeftPart(nullptr), m_pRightPart(nullptr), m_pMiddlePart(nullptr)
	, m_eWeaponState(SW_NONE), m_fMoveSpeed(25.f)
	, m_bJump(false), m_fVelocity(0.f), m_fJumpTime(0.f)
{

	m_eOBJ_ID = OBJ_PLAYER;
	m_iID = 0;

}

CPlayer::CPlayer(const CPlayer& rhs)
	: CCharacter(rhs)
	, m_pLeftPart(nullptr), m_pRightPart(nullptr), m_pMiddlePart(nullptr)
	, m_eWeaponState(SW_NONE), m_fMoveSpeed(25.f)
	, m_bJump(false), m_fVelocity(0.f), m_fJumpTime(0.f)
{
	m_eOBJ_ID = OBJ_PLAYER;
	//OBJ_Player가 0이고 , PlayerPart 는 Player 생성자 이후에 생기므로 iCount > 0이기때문에 
	//id를 0로 해도 무방
	m_iID = 0;
}

CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->m_vScale = { 6,6,1 };
	m_pTransformCom->Set_Pos(0, 0, 0.f);

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
				_float fAtk = m_mapWeapon[m_eWeaponState]->Get_Power();
				CollisionInfo info = { NULL, {0,0,0}, fAtk };
				pairCollider.second->Collision(info);
				//한 콜라이더에서 충돌이 일어났다면 이 몬스터의 다른 콜라이더와는 충돌체크 하지않음
				break;
			}
		}
	}
}

void CPlayer::Move_Input(const _float& fTimeDelta, const _vec3& vRight, const _vec3& vLook)
{
	if (CDInputMgr::GetInstance()->Key_Pressing(DIK_W))
	{
		m_pTransformCom->Move_Pos(&vLook, fTimeDelta, m_fMoveSpeed);
	}

	// 왼쪽 이동
	if (CDInputMgr::GetInstance()->Key_Pressing(DIK_A))
	{
		m_pTransformCom->Move_Pos(&vRight, fTimeDelta, -m_fMoveSpeed);
	}

	// 뒤로 이동
	if (CDInputMgr::GetInstance()->Key_Pressing(DIK_S))
	{
		m_pTransformCom->Move_Pos(&vLook, fTimeDelta, -m_fMoveSpeed);
	}

	// 오른쪽 이동
	if (CDInputMgr::GetInstance()->Key_Pressing(DIK_D))
	{
		m_pTransformCom->Move_Pos(&vRight, fTimeDelta, m_fMoveSpeed);
	}

	//점프
	if (CDInputMgr::GetInstance()->Key_Down(DIK_SPACE) && !m_bJump)
	{
		m_bJump = true; 
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
	if (CDInputMgr::GetInstance()->Mouse_Down(DIM_RB))
	{

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

void CPlayer::Gravity(const _float fTimeDelta)
{
	_float fVelocity = Get_Velocity();
	fVelocity -= 9.81f * fTimeDelta;
	Set_Velocity(fVelocity);
}
