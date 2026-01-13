#include "pch.h"
#include "CManagement.h"
#include "CDebugObject.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CDebugObject::CDebugObject(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev) 
	, m_pTransformCom(nullptr), m_pCollisionCom(nullptr), m_pCollider(nullptr)
{
	m_eOBJ_ID = OBJ_COL;
}

CDebugObject::CDebugObject(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale)
	:CGameObject(pGraphicDev)
	, m_pTransformCom(nullptr), m_pCollisionCom(nullptr), m_pCollider(nullptr)
	, m_vPos(vPos), m_vScale(vScale)
{
	m_eOBJ_ID = OBJ_COL;
}

CDebugObject::CDebugObject(const CDebugObject& rhs)
	: CGameObject(rhs)
	, m_pTransformCom(nullptr), m_pCollisionCom(nullptr), m_pCollider(nullptr)
{
	m_eOBJ_ID = OBJ_COL;
}

CDebugObject::~CDebugObject()
{
}

HRESULT CDebugObject::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	// Ceil
	//m_pTransformCom->m_vScale = { 1.f, 1.f, 1.f };
	//m_pTransformCom->Set_Pos(62.f, 35.f, 80.f);
	//

	//m_pCollider = m_pCollisionCom->CreateCollider(m_pTransformCom, m_szColliderName);
	//m_pCollider->Set_Scale(_vec3(25.f, 2.f, 45.f));

	//Wall
	m_pTransformCom->m_vScale = { 1.f, 1.f, 1.f };
	m_pTransformCom->Set_Pos(m_vPos);

	
	m_pCollider = m_pCollisionCom->CreateCollider(m_pTransformCom, m_szColliderName);
	m_pCollider->Set_Scale(m_vScale);


	m_pTransformCom->Update_Component(0.f);
	


	return S_OK;
}

_int CDebugObject::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);


	return iExit;
}

void CDebugObject::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CDebugObject::Render_GameObject()
{
}

HRESULT CDebugObject::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pCollisionCom = dynamic_cast<Engine::CCollision*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collision"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collision", pComponent });

	return S_OK;
}

void CDebugObject::Activate()
{
	CGameObject::Activate();
}

void CDebugObject::Deactivate()
{
	CGameObject::Deactivate();
}

CDebugObject* CDebugObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale)
{
	CDebugObject* pDebug = new CDebugObject(pGraphicDev, vPos, vScale);

	if (FAILED(pDebug->Ready_GameObject()))
	{
		Safe_Release(pDebug);
		MSG_BOX("DebugObject Create Failed");
		return nullptr;
	}

	return pDebug;
}

void CDebugObject::Free()
{
	CGameObject::Free();
}

void CDebugObject::OnCollision(CollisionInfo info)
{

}
