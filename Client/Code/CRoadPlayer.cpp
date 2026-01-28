#include "pch.h"
#include "CRoadPlayer.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CPoolMgr.h"
#include "CSoundMgr.h"
#include "CManagement.h"

#include "CMinigun.h"
#include "CPlayerBullet.h"


CRoadPlayer::CRoadPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	:	CCharacter(pGraphicDev)
	, m_pMainCollider(nullptr), m_pMinigun(nullptr), m_fMoveSpeed(100.f)
{
	m_eOBJ_ID = OBJ_PLAYER;
}

CRoadPlayer::CRoadPlayer(const CRoadPlayer& rhs)
	: CCharacter(rhs)
	, m_pMainCollider(nullptr), m_pMinigun(nullptr), m_fMoveSpeed(100.f)
{
	m_eOBJ_ID = OBJ_PLAYER;
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

HRESULT CRoadPlayer::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;	

	m_pTransformCom->Set_Pos({ 0.f,0.f,0.f });
	m_pTransformCom->m_vScale = { 5.f, 8.f  ,1.f };

	m_pMainCollider = m_pCollisionCom->CreateCollider(this, m_szMainColliderName);

	if (!m_pMainCollider) return E_FAIL;
	
	m_pMainCollider->Set_Scale(_vec3(5.f, 8.f, 2.f));
	m_pMainCollider->BindFuncToCollision([&](CollisionInfo info)
		{
			OnCollision(info);
		});


	//미니건 생성
	m_pMinigun = CMinigun::Create(m_pGraphicDev);

	if (m_pMinigun == nullptr)
		return E_FAIL;
	
	m_pMinigun->Set_Parent(this);

	return S_OK;
}

_int CRoadPlayer::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bDead) return RET_DEAD;
	
	_int iExit = CCharacter::Update_GameObject(fTimeDelta);

	
	m_pMinigun->Update_GameObject(fTimeDelta);

	
	return iExit;
}

void CRoadPlayer::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CCharacter::LateUpdate_GameObject(fTimeDelta);
	m_pMinigun->LateUpdate_GameObject(fTimeDelta);

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
	CCharacter::Free();
}

void CRoadPlayer::OnCollision(CollisionInfo info)
{
}

void CRoadPlayer::Key_Input(const _float& fTimeDelta)
{
	Engine::CTransform* pTransform = static_cast<CTransform*>(Engine::CManagement::GetInstance()->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", OBJ_CAM, L"Com_Transform"));
	_vec3 vLook, vRight, vLookExCludeY;
	pTransform->Get_Info(INFO_LOOK, &vLook);
	pTransform->Get_Info(INFO_RIGHT, &vRight);
	D3DXVec3Normalize(&vRight, &vRight);

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
