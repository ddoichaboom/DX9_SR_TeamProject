#include "pch.h"
#include "CRocket.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CToonFlash.h"
#include "CToonFog.h"
#include "CExplosion.h"
#include "CManagement.h"
#include "CPoolMgr.h"

TextureSource CRocket::m_textureSource =
{
	0,L"../Bin/Resource/Texture/BOSS/Boss_Missile.dds",true,1,3,3
};

_vec2 CRocket::m_DirFrame[DIR_END] =
{
	{0,0},{1,0},{2,0},{3,0},{0,1},{1,1},{2,1}, {3,1}
};

CRocket::CRocket(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr),
	m_pCollisionCom(nullptr), m_fSpeed(0.f), m_vDir{ 0,0,0 }, m_fLifeTime(0.f), m_fTime(0.f)
	, m_pCollider(nullptr), m_fVerticalAngle(0.f),m_fHorizonAngle(0.f),
	m_pToonFlash(nullptr),m_pToonFog(nullptr), m_fAttackDamage(5.f)
{
	m_eOBJ_ID = OBJ_BULLET;
	m_iID = Make_ID();
}

CRocket::CRocket(const CRocket& rhs)
	:CGameObject(rhs), m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr),
	m_pCollisionCom(nullptr), m_fSpeed(0.f), m_vDir{ 0,0,0 },  m_fLifeTime(0.f), m_fTime(0.f)
	, m_pCollider(nullptr), m_fVerticalAngle(0.f), m_fHorizonAngle(0.f),
	m_pToonFlash(nullptr), m_pToonFog(nullptr), m_fAttackDamage(5.f)
{
	m_eOBJ_ID = OBJ_BULLET;
	m_iID = Make_ID();
}

CRocket::~CRocket()
{
}

CRocket* CRocket::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRocket* pRocket = new CRocket(pGraphicDev);
	if (!pRocket) return nullptr;

	if (FAILED(pRocket->Ready_GameObject()))
	{
		Safe_Release(pRocket);
		MSG_BOX("Rocket Create Failed");
		return nullptr;
	}
	return pRocket;
}

HRESULT CRocket::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	m_pCollider = m_pCollisionCom->CreateCollider(this, m_szColliderName);
	if (!m_pCollider) return E_FAIL;

	m_pCollider->Set_Scale({ 7.f, 7.f, 7.f });
	m_pCollider->BindFuncToCollision([&](CollisionInfo info)
		{
			Explosion();
			SetDead();
		});


	m_pTransformCom->Set_Scale(10.f, 10.f, 3.f);
	//m_pTransformCom->Set_Scale(6.f, 6.f, 3.f);
	m_pTextureCom->Change_Texture(0);
	m_fSpeed = 300.f;
	m_fLifeTime = 3.0f;

	m_pToonFlash = CToonFlash::Create(m_pGraphicDev);
	m_pToonFog = CToonFog::Create(m_pGraphicDev);
	m_pToonFog->SetOwnerTransform(m_pTransformCom);

	m_fAttackDamage = 5.f;

	return S_OK;
}

_int CRocket::Update_GameObject(const _float& fTimeDelta)
{
	if (IsDead()) return RET_DEAD;
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	m_fTime += fTimeDelta;
	if (m_fTime >= m_fLifeTime)
	{
		SetDead();
		return RET_NONE;
	}


	D3DXVec3TransformCoord(&vToonFlashPos, &vToonFlashLocalPos, m_pTransformCom->Get_World());
	m_pToonFlash->SetFlashPos(vToonFlashPos);
	m_pToonFlash->Update_GameObject(fTimeDelta);
	m_pToonFog->Update_GameObject(fTimeDelta);

	m_pTransformCom->Move_Pos(&m_vDir, fTimeDelta, m_fSpeed);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);
	Compute_ViewZ(m_pTransformCom->Get_Info(INFO_POS));

	return iExit;
}

void CRocket::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	SetBillboard();
	m_pTextureCom->Set_Frame(m_vCurFrame);
}

void CRocket::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pTextureCom->Render_Texture();
	m_pBufferCom->Render_Buffer();
}

void CRocket::SetPos(_vec3 _pos)
{
	m_pTransformCom->Set_Pos(_pos);
}

void CRocket::SetDirection(_vec3 _dir)
{
	m_vDir = _dir;
	m_pToonFog->SetDirection(_dir);
}

void CRocket::Activate()
{
	CGameObject::Activate();
	m_pTextureCom->Change_Texture(0);

	m_fTime = 0.f;
}

void CRocket::Deactivate()
{
	CGameObject::Deactivate();
	m_pToonFog->Reset();
	m_pToonFlash->Reset();
}

HRESULT CRocket::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	//VIBuffer
	pComponent = m_pBufferCom = static_cast<Engine::CRcTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom = static_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	//Collision
	pComponent = m_pCollisionCom = static_cast<Engine::CCollision*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collision"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collision", pComponent });

	//Texture
	pComponent = m_pTextureCom = static_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RocketTexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });
	return S_OK;
}

void CRocket::Explosion()
{
	CExplosion* exp = CPoolMgr::GetInstance()->Get_Object<CExplosion>();
	if (exp)
	{
		CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer")->Add_GameObject(exp);
		_vec3 pos = *m_pTransformCom->Get_Info(INFO_POS) - m_vDir * 5.f;
		exp->SetPos(pos);
		exp->SetSize({ 1.5f, 1.5f });
		exp->Reset();
	}
}


void CRocket::SetBillboard()
{
	static _float _degree5 = D3DX_PI / 36.f;
	static _float _degree10 = D3DX_PI / 18.f;
	static _float _degree20 = D3DX_PI / 9.f;
	static _float _degree30 = D3DX_PI / 6.f;
	static _float _degree45 = D3DX_PI / 4.f;

	_matrix matView, matBill;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);
	_vec3 camPos;
	memcpy(&camPos, &matView.m[3], sizeof(_vec3));
	_vec3 myPos = *m_pTransformCom->Get_Info(INFO_POS);
	_vec3 myScale = m_pTransformCom->m_vScale;

	_vec3 look = myPos - camPos;
	D3DXVec3Normalize(&look, &look);

	_vec3 right;
	_vec3 up = { 0.0f, 1.0f, 0.0f };
	D3DXVec3Cross(&right, &up, &look);
	D3DXVec3Normalize(&right, &right);

	D3DXVec3Cross(&up, &look, &right);
	D3DXVec3Normalize(&up, &up);

	D3DXMatrixIdentity(&matBill);
	right *= myScale.x;
	up *= myScale.y;
	look *= myScale.z;

	memcpy(&matBill.m[0], &right, sizeof(_vec3));
	memcpy(&matBill.m[1], &up, sizeof(_vec3));
	memcpy(&matBill.m[2], &look, sizeof(_vec3));
	memcpy(&matBill.m[3], &myPos, sizeof(_vec3));
	m_pTransformCom->Set_World(&matBill);






	//각도별 텍스쳐 선택
	_vec3 Front = look * -1.f;
	_vec3 dir =  m_vDir;
	_float angle;

	//정면체크 
	Front.y = 0.f; dir.y = 0.f;
	D3DXVec3Normalize(&Front, &Front);
	D3DXVec3Normalize(&dir, &dir);
	_float fDot = D3DXVec3Dot(&Front, &dir);

	angle = acosf(D3DXVec3Dot(&Front, &dir));
	Front = { 0.f, look.y * -1.f, look.z * -1.f };
	dir = { 0.f, m_vDir.y, m_vDir.z };
	D3DXVec3Normalize(&Front, &Front);
	D3DXVec3Normalize(&dir, &dir);

	angle = acosf(D3DXVec3Dot(&Front, &dir));

	if (angle < _degree5) m_vCurFrame = m_DirFrame[FRONT_90];
	else if(angle < _degree10)  m_vCurFrame = m_DirFrame[FRONT_100];
	else if (angle < _degree20) m_vCurFrame = m_DirFrame[FRONT_110];
	else m_vCurFrame = m_DirFrame[FRONT_120];
	//else m_vCurFrame = m_DirFrame[FRONT_120];
	//정면
	//if (angle < _degree20)
	//{
	//	Front= { 0.f, look.y * -1.f, look.z * -1.f };
	//	dir = { 0.f, m_vDir.y, m_vDir.z};
	//	D3DXVec3Normalize(&Front, &Front);
	//	D3DXVec3Normalize(&dir, &dir);
	//	
	//	angle = acosf(D3DXVec3Dot(&Front, &dir));

	//	if (angle < _degree10) m_vCurFrame = m_DirFrame[FRONT_90];
	//	else m_vCurFrame = m_DirFrame[FRONT_100];
	//	//else m_vCurFrame = m_DirFrame[FRONT_120];
	//}
	//else
	//{
	//	_vec3 camRight;
	//	memcpy(&camRight, &matView.m[INFO_RIGHT], sizeof(_vec3));
	//	//카메라랑 right 방향이 같은면 right, 아니면 left 
	//	bool bLeft = D3DXVec3Dot(&camRight, &m_vDir) < 0.f;

	//	if (bLeft) m_vCurFrame = m_DirFrame[LEFT_30];
	//	else  m_vCurFrame = m_DirFrame[RIGHT_30];
	//}

}

void CRocket::Free()
{
	Safe_Release(m_pToonFlash);
	Safe_Release(m_pToonFog);
	CGameObject::Free();
}