#include "pch.h"
#include "CRoadPlayer.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CPoolMgr.h"
#include "CSoundMgr.h"
#include "CManagement.h"
#include "CUIManager.h"

#include "CMinigun.h"
#include "CPlayerBullet.h"
#include "CHitUI.h"


CRoadPlayer::CRoadPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	:	CCharacter(pGraphicDev)
	, m_pMainCollider(nullptr), m_pHitUI(nullptr)
	,m_pMinigun(nullptr), m_fMoveSpeed(100.f), m_bStageEnd(false)
{
	m_eOBJ_ID = OBJ_PLAYER;
	m_fMaxHP = m_fHP = 100.f;
}

CRoadPlayer::CRoadPlayer(const CRoadPlayer& rhs)
	: CCharacter(rhs)
	, m_pMainCollider(nullptr), m_pHitUI(nullptr)
	, m_pMinigun(nullptr), m_fMoveSpeed(100.f), m_bStageEnd(false)
{
	m_eOBJ_ID = OBJ_PLAYER;
	m_fMaxHP = m_fHP = 100.f;
}

CRoadPlayer::~CRoadPlayer()
{
}

CRoadPlayer* CRoadPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRoadPlayer* pPlayer = new CRoadPlayer(pGraphicDev);

	if (FAILED(pPlayer->Ready_GameObject()))
	{
		Safe_Release(pPlayer);
		MSG_BOX("Player Create Failed");
		return nullptr;
	}

	return pPlayer;
}

void CRoadPlayer::OnEvent(EVENT_TYPE _type, EventData* _pData)
{
	if (_type == EVENT_ENDING)
	{
		m_bStageEnd = true;
	}

	else if (_type == EVENT_MONSTER_DEAD)
	{
		MonsterData* pData = static_cast<MonsterData*>(_pData);
		CUIManager::GetInstance()->Create_TextUI(m_pGraphicDev, pData->eTag, pData->value);
	}
}

HRESULT CRoadPlayer::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;	

	m_pTransformCom->Set_Pos({ 0.f,20.f,0.f });
	m_pTransformCom->m_vScale = { 10.f, 10.f  ,1.f };

	m_pMainCollider = m_pCollisionCom->CreateCollider(this, m_szMainColliderName);

	if (!m_pMainCollider) return E_FAIL;
	
	m_pMainCollider->Set_Scale(_vec3(10.f, 10.f, 2.f));
	m_pMainCollider->BindFuncToCollision([&](CollisionInfo info)
		{
			OnCollision(info);
		});


	//미니건 생성
	m_pMinigun = CMinigun::Create(m_pGraphicDev);

	if (m_pMinigun == nullptr)
		return E_FAIL;
	
	m_pMinigun->Set_Parent(this);

	m_pHitUI = CHitUI::Create(m_pGraphicDev);
	m_pHitUI->SetDead();

	CEventMgr::GetInstance()->Subscribe(EVENT_ENDING, this);
	CEventMgr::GetInstance()->Subscribe(EVENT_MONSTER_DEAD, this);

	return S_OK;
}

_int CRoadPlayer::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bDead) return RET_DEAD;
	
	_int iExit = CCharacter::Update_GameObject(fTimeDelta);
	m_pMinigun->Update_GameObject(fTimeDelta);	
	if (m_pHitUI && m_pHitUI->IsDead() == false)
	{
		m_pHitUI->Update_GameObject(fTimeDelta);
	}
		
	return iExit;
}

void CRoadPlayer::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CCharacter::LateUpdate_GameObject(fTimeDelta);
	m_pMinigun->LateUpdate_GameObject(fTimeDelta);

	if (m_bStageEnd)
		return;

	Key_Input(fTimeDelta);
}

void CRoadPlayer::Render_GameObject()
{
	
}

HRESULT CRoadPlayer::Add_Component()
{
	if (FAILED(CCharacter::Add_Component())) return E_FAIL;
	
	return S_OK;
}

void CRoadPlayer::Free()
{
	Safe_Release(m_pMinigun);
	Safe_Release(m_pHitUI);
	CCharacter::Free();
}


void CRoadPlayer::OnCollision(CollisionInfo info)
{
	if (info.fDamage > 0.f)
	{
		if (m_pHitUI->IsDead())
		{
			m_pHitUI->Reset();
		}
	}

	if (info.eDir != CDIR_NONE)
	{
		Move_ByCollision(info.eDir, info.vDiff);
	}
}

void CRoadPlayer::Key_Input(const _float& fTimeDelta)
{
	Engine::CTransform* pTransform = static_cast<CTransform*>(Engine::CManagement::GetInstance()->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", OBJ_CAM, L"Com_Transform"));
	_vec3 vLook, vRight, vLookExCludeY;
	pTransform->Get_Info(INFO_LOOK, &vLook);
	//pTransform->Get_Info(INFO_RIGHT, &vRight);
	//D3DXVec3Normalize(&vRight, &vRight);
	vRight = {1.f, 0.f,0.f };

	MOVE_DIR eDir = CDInputMgr::GetInstance()->Get_Direction();

	switch (eDir)
	{
	case Engine::DIR_NONE:
	case Engine::DIR_UP:
	case Engine::DIR_DOWN:
		break;
	case Engine::DIR_LEFTUP:
	case Engine::DIR_LEFT:
	case Engine::DIR_LEFTDOWN:		
		m_pTransformCom->Move_Pos(&vRight, fTimeDelta, -m_fMoveSpeed);
		break;
	case Engine::DIR_RIGHTUP:
	case Engine::DIR_RIGHT:
	case Engine::DIR_RIGHTDOWN:		
		m_pTransformCom->Move_Pos(&vRight, fTimeDelta, m_fMoveSpeed);
		break;
	}


	if (CDInputMgr::GetInstance()->Key_Down(DIK_G))
	{
		_vec3 vPos = *m_pTransformCom->Get_Info(INFO_POS);
		vPos.y = 0;
	}
}

void CRoadPlayer::Activate()
{
	CCharacter::Activate();
}

void CRoadPlayer::Deactivate()
{
	CCharacter::Deactivate();
}

void CRoadPlayer::Shoot()
{
	CPlayerBullet* pBullet = CPoolMgr::GetInstance()->Get_Object<CPlayerBullet>();
	if (!pBullet) return;

	

	Engine::CTransform* pTransform = static_cast<CTransform*>(Engine::CManagement::GetInstance()->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", OBJ_CAM, L"Com_Transform"));
	_vec3 vLook, vRight;
	pTransform->Get_Info(INFO_LOOK, &vLook);
	_vec3 myPos = *m_pTransformCom->Get_Info(INFO_POS);		
	D3DXVec3Normalize(&vLook, &vLook);
	myPos += vLook * 8.f;

	pBullet->SetPos(myPos);
	pBullet->SetDirection(vLook);

	CLayer* layer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");
	if (!layer) pBullet->ReturnToPool();
	else layer->Add_GameObject(pBullet);
}
