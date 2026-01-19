#include "CCollider.h"
#include "CCubeCol.h"
#include "CTransform.h"
#include "CRenderer.h"
#include "CParticleEmitter.h"

CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphicDev)
: CGameObject(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr)
, m_pPrtTransformCom(nullptr), m_bCanCollision(true), m_bRotToPrt(false),m_pOwner(nullptr)
{
	D3DXMatrixIdentity(&m_matWorld);
}

CCollider::~CCollider()
{

}

HRESULT CCollider::Ready_Collider(CGameObject* _Owner)
{
	if (!_Owner) return E_FAIL;
	m_pOwner = _Owner;

	m_pPrtTransformCom = static_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, L"Com_Transform"));
	if (!m_pPrtTransformCom) 
	{
		m_pPrtTransformCom = static_cast<CTransform*>(m_pOwner->Get_Component(ID_STATIC, L"Com_Transform"));
		if (!m_pPrtTransformCom) return E_FAIL;
	}

	if (FAILED(Add_Component())) return E_FAIL;

	m_eOBJ_ID = OBJ_COL;
	m_iID = Make_ID();

	return S_OK;
}

_int CCollider::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CGameObject::Update_GameObject(fTimeDelta);

	if(m_bCanCollision)
		CRenderer::GetInstance()->Add_RenderGroup(RENDER_DEBUG, this);
	
	return iExit;
}

void CCollider::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_matWorld = (*m_pTransformCom->Get_World());
	//위치값만 적용
	_matrix* prtWorld = m_pPrtTransformCom->Get_World();
	m_matWorld._41 += prtWorld->_41;
	m_matWorld._42 += prtWorld->_42;
	m_matWorld._43 += prtWorld->_43;

	//회전 적용 
	if (m_bRotToPrt)
	{
		//Prt의 각 축을 정규화하여 스케일값을 뺀 부모의 회전 축을 얻음
		for (_uint i = 0; i < INFO_POS; ++i)
		{
			_vec3 vecAxis;
			memcpy(&vecAxis, &prtWorld->m[i], sizeof(_vec3));
			D3DXVec3Normalize(&vecAxis, &vecAxis); // 회전 축
			vecAxis *= m_pTransformCom->m_vScale[i]; // 내 스케일을 곱해줌 

			memcpy(&m_matWorld.m[i], &vecAxis, sizeof(_vec3));
		}
	}
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
	//들어온게 지형충돌이면 충돌처리함
	if (info.eDir == CDIR_NONE && !m_bCanCollision)  return;
	if(m_BindFunc)	m_BindFunc(info);
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

_vec3 CCollider::Get_ParentPos()
{
	_vec3 pos;
	m_pPrtTransformCom->Get_Info(INFO_POS, &pos);
	return pos;
}

CCollider* CCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* _Owner)
{
	CCollider* pCollider = new CCollider(pGraphicDev);
	if (FAILED(pCollider->Ready_Collider(_Owner)))
	{
		Safe_Release(pCollider);
		MSG_BOX("Collider Create Faild");
		return nullptr;
	}
	return pCollider;
}

void CCollider::Free()
{
	CGameObject::Free();
}