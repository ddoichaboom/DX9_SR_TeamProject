#include "pch.h"
#include "CPlayerBullet.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

TextureSource CPlayerBullet::m_textureSource =
{
	0,L"../Bin/Resource/Texture/Bullet/bullet256.dds"
};

AnimationSource CPlayerBullet::m_vAnimSource =
{
	0, 0,3,3, true, 0.04f
};


CPlayerBullet::CPlayerBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	:CBullet(pGraphicDev), m_pAnimationCom(nullptr)
{
	m_bMonsterBullet = false;
	m_fAttackDamage = 1.f;
}

CPlayerBullet::CPlayerBullet(const CPlayerBullet& rhs)
	:CBullet(rhs), m_pAnimationCom(nullptr)
{
	m_bMonsterBullet = false;
	m_fAttackDamage = 1.f;
}

CPlayerBullet::~CPlayerBullet()
{
}

CPlayerBullet* CPlayerBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayerBullet* bullet = new CPlayerBullet(pGraphicDev);
	if (!bullet) return nullptr;

	if (FAILED(bullet->Ready_GameObject()))
	{
		Safe_Release(bullet);
		MSG_BOX("Player Bullet Create Failed");
		return nullptr;
	}
	return bullet;
}

HRESULT CPlayerBullet::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;

	m_pCollider = m_pCollisionCom->CreateCollider(this, m_szColliderName);
	if (!m_pCollider) return E_FAIL;
	m_pCollider->BindFuncToCollision([&](CollisionInfo info)
		{
			if (info.pTarget->GetOBJID() == OBJ_COL) Explosion();
			SetDead();
		});

	//D3DXMatrixRotationX(&m_matPreRot, D3DXToRadian(m_RotXoffset));
	//m_pTransformCom->Set_Scale(6.f, 6.f, 6.f);
	m_pTransformCom->Set_Scale(7.f, 7.f, 7.f);
	m_pCollider->Set_Scale({ 5.f, 5.f, 5.f });
	m_fSpeed = 800.f;
	m_fLifeTime = 1.0f;

	m_pAnimationCom->Change_Animation(0);

	//m_RotXoffset = 100.f;
	return S_OK;
}

_int CPlayerBullet::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CBullet::Update_GameObject(fTimeDelta);
	return iExit;
}

void CPlayerBullet::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CBullet::LateUpdate_GameObject(fTimeDelta);
	SetBillboard();
}

void CPlayerBullet::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pAnimationCom->Render_Animation();
	m_pBufferCom->Render_Buffer();
}

HRESULT CPlayerBullet::Add_Component()
{
	CComponent* pComponent = NULL;

	//VIBuffer
	pComponent = m_pBufferCom = static_cast<Engine::CRcTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

	if (NULL == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom = static_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (NULL == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	//Collision
	pComponent = m_pCollisionCom = static_cast<Engine::CCollision*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collision"));

	if (NULL == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collision", pComponent });

	//Animation
	pComponent = m_pAnimationCom = static_cast<Engine::CAnimation*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_PlayerBulletAnimation"));

	if (NULL == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });

	return S_OK;
}

void CPlayerBullet::Free()
{
	CBullet::Free();
}

void CPlayerBullet::Activate()
{
	CGameObject::Activate();
	m_pAnimationCom->Change_Animation(0);
	m_pAnimationCom->PlayFromStart();
	m_fTime = 0.f;
}

void CPlayerBullet::Deactivate()
{
	CGameObject::Deactivate();
	m_pAnimationCom->Stop();
}

void CPlayerBullet::SetDirection(_vec3 dir)
{
	m_vDir = dir;

}
void CPlayerBullet::SetBillboard()
{
	_vec3 vRight, vUp, vLook;
	_matrix View, Bill;
	_vec3 myPos = *m_pTransformCom->Get_Info(INFO_POS);
	vUp = { 0,1,0 };

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &View);
	D3DXMatrixInverse(&View, NULL, &View);
	_vec3 camPos = { View._41, View._42, View._43 };
	vLook = myPos - camPos;
	D3DXVec3Normalize(&vLook, &vLook);

	D3DXVec3Cross(&vRight, &vUp, &vLook);
	D3DXVec3Normalize(&vRight, &vRight);

	D3DXVec3Cross(&vUp, &vLook, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);

	_vec3 vScale = m_pTransformCom->Get_Scale();

	D3DXMatrixIdentity(&Bill);
	vRight *= vScale.x;
	vUp *= vScale.y;
	vLook *= vScale.z;

	memcpy(&Bill.m[0], &vRight, sizeof(_vec3));
	memcpy(&Bill.m[1], &vUp, sizeof(_vec3));
	memcpy(&Bill.m[2], &vLook, sizeof(_vec3));
	memcpy(&Bill.m[3], &myPos, sizeof(_vec3));
	m_pTransformCom->Set_World(&Bill);

}