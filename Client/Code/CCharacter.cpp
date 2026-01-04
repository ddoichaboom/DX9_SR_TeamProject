#include "pch.h"
#include "CCharacter.h"
#include "CProtoMgr.h"

CCharacter::CCharacter(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr),
	m_pStateCom(nullptr), m_pCollisionCom(nullptr), m_fTime(0.f)
{
}

CCharacter::CCharacter(const CCharacter& rhs)
	: CGameObject(rhs), m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr),
	 m_pStateCom(nullptr), m_pCollisionCom(nullptr), m_fTime(0.f)
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