#include "pch.h"
#include "CRoadPlayer.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CPoolMgr.h"
#include "CSoundMgr.h"

#include "CMinigun.h"


CRoadPlayer::CRoadPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	:	CCharacter(pGraphicDev)
	, m_pMainCollider(nullptr), m_pMinigun(nullptr)
{
}

CRoadPlayer::CRoadPlayer(const CRoadPlayer& rhs)
	: CCharacter(rhs)
	, m_pMainCollider(nullptr), m_pMinigun(nullptr)
{
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

void CRoadPlayer::Activate()
{
}

void CRoadPlayer::Deactivate()
{
}
