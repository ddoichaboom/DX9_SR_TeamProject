#include "CCollider.h"
#include "CCubeCol.h"
#include "CTransform.h"
#include "CRenderer.h"

CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphicDev)
: CGameObject(pGraphicDev)
, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pPrtTransformCom(nullptr)
{
	D3DXMatrixIdentity(&m_matWorld);
}

CCollider::~CCollider()
{

}

HRESULT CCollider::Ready_Collider(CTransform* _prtTransComp)
{
	if (!_prtTransComp) return E_FAIL;
	m_pPrtTransformCom = _prtTransComp;
	m_pPrtTransformCom->AddRef();

	if (FAILED(Add_Component())) return E_FAIL;
	return S_OK;
}

_int CCollider::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_DEBUG, this);
	
	return iExit;
}

void CCollider::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	//m_matWorld = (*m_pTransformCom->Get_World()) * (*m_pPrtTransformCom->Get_World());
	m_matWorld = (*m_pTransformCom->Get_World());
	//위치값만 적용
	memcpy(m_matWorld.m[3], m_pPrtTransformCom->Get_World()->m[3], sizeof(_vec3));
}

void CCollider::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matWorld);
	m_pBufferCom->Render_Buffer();
}

HRESULT	CCollider::Add_Component()
{
	m_pBufferCom = CCubeCol::Create(m_pGraphicDev);
	if (!m_pBufferCom) return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", m_pBufferCom });

	m_pTransformCom = CTransform::Create(m_pGraphicDev);
	if (!m_pTransformCom) return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Tranform", m_pTransformCom });

	return S_OK;
}

_vec3* CCollider::GetVtx()
{
	return m_pBufferCom->GetVtx();
}

_matrix  CCollider::GetWorldMatrix()
{
	return m_matWorld;
}

void  CCollider::Collision(CollisionInfo info)
{
	m_BindFunc(info);
}

void CCollider::Set_Scale(_vec3 _scale)
{
	m_pTransformCom->m_vScale = _scale;
}

_vec3 CCollider::Get_Scale()
{
	return m_pTransformCom->m_vScale;
}


void CCollider::Set_RelativePos(_vec3 _pos)
{
	m_pTransformCom->Set_Pos(_pos);
}

_vec3 CCollider::Get_RelativePos()
{
	_vec3 pos;
	m_pTransformCom->Get_Info(INFO_POS, &pos);
	return pos;
}

CCollider* CCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev, CTransform* _prtTransComp)
{
	CCollider* pCollider = new CCollider(pGraphicDev);
	if (FAILED(pCollider->Ready_Collider(_prtTransComp)))
	{
		Safe_Release(pCollider);
		MSG_BOX("Collider Create Faild");
		return nullptr;
	}
	return pCollider;
}

void CCollider::Free()
{
	Safe_Release(m_pPrtTransformCom);
	CGameObject::Free();
}