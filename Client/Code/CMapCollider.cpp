#include "pch.h"
#include "CManagement.h"
#include "CMapCollider.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CMapCollider::CMapCollider(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev) 
	, m_pTransformCom(nullptr), m_pCollisionCom(nullptr), m_pCollider(nullptr),
	m_vPos{0.f,0.f,0.f}, m_vScale{1.f,1.f,1.f}, m_eColliderTag(TAG_NONE)
{
	m_eOBJ_ID = OBJ_COL;
}

CMapCollider::CMapCollider(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale)
	:CGameObject(pGraphicDev)
	, m_pTransformCom(nullptr), m_pCollisionCom(nullptr), m_pCollider(nullptr)
	, m_vPos(vPos), m_vScale(vScale)
{
	m_eOBJ_ID = OBJ_COL;
}

CMapCollider::CMapCollider(const CMapCollider& rhs)
	: CGameObject(rhs)
	, m_pTransformCom(nullptr), m_pCollisionCom(nullptr), m_pCollider(nullptr)
{
	m_eOBJ_ID = OBJ_COL;
}

CMapCollider::~CMapCollider()
{
}

HRESULT CMapCollider::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->m_vScale = { 1.f, 1.f, 1.f };
	m_pTransformCom->Set_Pos(m_vPos);
	
	m_pCollider = m_pCollisionCom->CreateCollider(this, m_szColliderName);
	m_pCollider->Set_Scale(m_vScale);

	//Transform -Static 
	m_pTransformCom->Update_Component(0.f);

	return S_OK;
}

_int CMapCollider::Update_GameObject(const _float& fTimeDelta)
{
	if (IsDead()) 
		return RET_DEAD;
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);
	return iExit;
}


void CMapCollider::Render_GameObject()
{
	m_pCollider->Render_GameObject();
}

HRESULT CMapCollider::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;
	pComponent = m_pTransformCom = static_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pCollisionCom = static_cast<Engine::CCollision*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collision"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collision", pComponent });

	return S_OK;
}

void CMapCollider::SetPos(_vec3 _pos)
{
	m_pTransformCom->Set_Pos(_pos);
	m_pTransformCom->Update_Component(1.f);
}
void CMapCollider::SetScale(_vec3 _scale)
{
	m_pTransformCom->Set_Scale(_scale);
	m_pTransformCom->Update_Component(1.f);
}

void CMapCollider::Set_ColliderScale(_vec3 _scale)
{
	m_vScale = _scale;

	if (m_pCollider)
	{
		m_pCollider->Set_Scale(m_vScale);
	}
}

void CMapCollider::Activate()
{
	CGameObject::Activate();
}

void CMapCollider::Deactivate()
{
	CGameObject::Deactivate();
}

CMapCollider* CMapCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMapCollider* pMapCol = new CMapCollider(pGraphicDev);

	if (FAILED(pMapCol->Ready_GameObject()))
	{
		Safe_Release(pMapCol);
		MSG_BOX("MapCollider Create Failed");
		return nullptr;
	}

	return pMapCol;
}


CMapCollider* CMapCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale)
{
	CMapCollider* pMapCol = new CMapCollider(pGraphicDev, vPos, vScale);

	if (FAILED(pMapCol->Ready_GameObject()))
	{
		Safe_Release(pMapCol);
		MSG_BOX("MapCollider Create Failed");
		return nullptr;
	}

	return pMapCol;
}

void CMapCollider::Free()
{
	CGameObject::Free();
}
