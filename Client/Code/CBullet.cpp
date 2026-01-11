#include "pch.h"
#include "CBullet.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

TextureSource CBullet::m_textureSource =
{
	0,L"../Bin/Resource/Texture/Bullet/single_bullet256.dds",false
};

CBullet::CBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr),
	m_pCollisionCom(nullptr), m_fSpeed(0.f), m_vDir{0,0,0}, m_STATE(DIR_END), m_fLifeTime(0.f), m_fTime(0.f)
	, m_pCollider(nullptr)
{
	m_eOBJ_ID = OBJ_BULLET;
	m_iID = Make_ID();
}

CBullet::CBullet(const CBullet& rhs)
	:CGameObject(rhs), m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr),
	m_pCollisionCom(nullptr), m_fSpeed(0.f), m_vDir{ 0,0,0 }, m_STATE(DIR_END), m_fLifeTime(0.f), m_fTime(0.f)
	, m_pCollider(nullptr)
{
	m_eOBJ_ID = OBJ_BULLET;
	m_iID = Make_ID();
}

CBullet::~CBullet()
{
}

CBullet* CBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBullet* bullet = new CBullet(pGraphicDev);
	if (!bullet) return nullptr;

	if (FAILED(bullet->Ready_GameObject()))
	{
		Safe_Release(bullet);
		MSG_BOX("Bullet Create Failed");
		return nullptr;
	}
	return bullet;
}

HRESULT CBullet::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	m_pCollider = m_pCollisionCom->CreateCollider(m_pTransformCom, m_szColliderName);
	if (!m_pCollider) return E_FAIL;
	m_pCollider->BindFuncToCollision([&](CollisionInfo info)
		{
			SetDead();
		});

	D3DXMatrixRotationX(&m_matPreRot, D3DXToRadian(m_RotXoffset));
	m_pTransformCom->Set_Scale(2.f, 1.f, 3.f);
	m_pTextureCom->Change_Texture(0);
	m_fSpeed = 300.f;
	m_fLifeTime = 3.0f;

	return S_OK;
}

_int CBullet::Update_GameObject(const _float& fTimeDelta)
{
	if (IsDead()) return RET_DEAD;
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	m_fTime += fTimeDelta;
	if (m_fTime >= m_fLifeTime)
	{
		SetDead();
		return RET_NONE;
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	Compute_ViewZ(m_pTransformCom->Get_Info(INFO_POS));

	return iExit;
}

void CBullet::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	//눕히기 위해 추가 회전 행렬을 초기에 곱해줌 
	_matrix world = *m_pTransformCom->Get_World();
	world = m_matPreRot * world;
	m_pTransformCom->Set_World(&world);

	m_pTransformCom->Move_Pos(&m_vDir, fTimeDelta, m_fSpeed);
}

void CBullet::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pTextureCom->Render_Texture();
	m_pBufferCom->Render_Buffer();
}

void CBullet::SetRotation(ROTATION eType, const _float& fAngle)
{
	m_pTransformCom->Rotation(eType, fAngle);
}

void CBullet::SetPos(_vec3 _pos)
{
	m_pTransformCom->Set_Pos(_pos);
}


void CBullet::SetDirection(_vec3 dir)
{
	static _float rad_Degree45 = D3DX_PI / 4.f;

	m_vDir = dir;
	_float angleY = D3DXToDegree(atan2f(m_vDir.x, m_vDir.z));
	//방향을 xz평면에 투영했을때의 길이 
	_float  hypo = sqrtf(m_vDir.x * m_vDir.x + m_vDir.z * m_vDir.z);
	_float angleX = -D3DXToDegree(atan2f(m_vDir.y, hypo));
	m_pTransformCom->Rotation(ROT_Y, angleY);
	m_pTransformCom->Rotation(ROT_X, angleX);
}



void CBullet::Activate()
{
	CGameObject::Activate();
	m_pTextureCom->Change_Texture(0);
	m_fTime = 0.f;
}

void CBullet::Deactivate()
{
	CGameObject::Deactivate();
}

HRESULT CBullet::Add_Component()
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

	//Texture
		// Animation
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BulletTexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });
	return S_OK;
}


void CBullet::Free()
{
	CGameObject::Free();
}
