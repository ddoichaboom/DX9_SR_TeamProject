#include "pch.h"
#include "CCharacter.h"
#include "CProtoMgr.h"

CCharacter::CCharacter(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pStateCom(nullptr), m_pCollisionCom(nullptr), m_fTime(0.f)
	, m_bFall(false), m_fVelocity(0.f)
	, m_bJump(false), m_fJumpStartY(0.f), m_fJumpDuration(0.6f), m_fJumpHeight(20.f)
	, m_bDash(false), m_fDashTime(0.f), m_fDashDuration(0.3f), m_fDashDistance(80.f)
{
}

CCharacter::CCharacter(const CCharacter& rhs)
	: CGameObject(rhs)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pStateCom(nullptr), m_pCollisionCom(nullptr), m_fTime(0.f)
	, m_bFall(false), m_fVelocity(0.f)
	, m_bJump(false), m_fJumpStartY(0.f), m_fJumpDuration(0.6f), m_fJumpHeight(20.f)
	, m_bDash(false), m_fDashTime(0.f), m_fDashDuration(0.3f), m_fDashDistance(80.f)
{
}

CCharacter::~CCharacter()
{
}

HRESULT CCharacter::Ready_GameObject()
{
	return S_OK;
}

_int CCharacter::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);
	return iExit;
}

void CCharacter::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

HRESULT CCharacter::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;
	
	//VIBuffer
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	//Collision
	pComponent = m_pCollisionCom = dynamic_cast<Engine::CCollision*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collision"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collision", pComponent });

	//StateComponent
	pComponent = m_pStateCom = dynamic_cast<Engine::CStateComponent*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_StateComponent"));

	//Owner 지정해주기!! 
	m_pStateCom->SetOnwer(this);

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_StateComponent", pComponent });

	return S_OK;
}


void CCharacter::Free()
{
	CGameObject::Free();
}

void CCharacter::Activate()
{
	CGameObject::Activate();
}

void CCharacter::Deactivate()
{
	CGameObject::Deactivate();
}

void CCharacter::SetPos(_vec3 _pos)
{
	if (!m_pTransformCom) return;
	m_pTransformCom->Set_Pos(_pos);
}

void CCharacter::Rotate(ROTATION _Axis, _float _degree)
{
	if (!m_pTransformCom) return;
	m_pTransformCom->Rotation(_Axis, _degree);
}

void CCharacter::Gravity(const _float& fTimeDelta)
{
	_float fVelocity = Get_Velocity();
	fVelocity -= 9.81f * (fTimeDelta + 0.25f);
	m_fVelocity = fVelocity;
}

void CCharacter::Set_OnFloor(const _float& fTimeDelta)
{
	_float fY = 0.f;
	_float fBottom = 0.f;
	_vec3 vPosition = m_pTransformCom->m_vInfo[INFO_POS];

	if (m_bJump)
	{
		Update_Jump(fTimeDelta);
		return;
	}
	else if (m_bDash)
	{
		Update_Dash(fTimeDelta);
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

void CCharacter::Update_Jump(const _float& fTimeDelta)
{
	m_fJumpTime += fTimeDelta;
	float t = m_fJumpTime / m_fJumpDuration;

	if (t >= 1.f)
	{
		t = 1.f;
		m_bJump = false;
		m_bFall = true;
	}

	float easeOutQuad = 1.f - (1.f - t) * (1.f - t);

	float fNewY = m_fJumpStartY + easeOutQuad * m_fJumpHeight;

	m_pTransformCom->m_vInfo[INFO_POS].y = fNewY;
}

void CCharacter::Update_Dash(const _float& fTimeDelta)
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
	//float easeOutQuint = 1.f - powf(1.f - t, 5);


	_float dashDistance = easeOutQuad * m_fDashDistance;
	//_float dashDistance = easeOutQuint * m_fDashDistance;
	_vec3 newPos = m_vDashStart + m_vDashDir * dashDistance;
	
	m_pTransformCom->Set_Pos(newPos);
}

_bool CCharacter::Get_OnFloor()
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