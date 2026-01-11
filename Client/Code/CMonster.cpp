#include "pch.h"
#include "CMonster.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CManagement.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	:CCharacter(pGraphicDev), m_pAnimationCom(nullptr)
	,m_fAttackableDist(100.f), m_vDir({0,0,0}), m_fSpeed(10.f)
	, m_pPlayerTransformCom(nullptr), m_pPlayerCollisionCom(nullptr)
{
	m_eOBJ_ID = OBJ_MONSTER;
	m_iID = Make_ID();	
}

CMonster::CMonster(const CMonster& rhs)
	:CCharacter(rhs), m_pAnimationCom(nullptr), m_fAttackableDist(100.f)
	, m_vDir(rhs.m_vDir), m_fSpeed(rhs.m_fSpeed)
	, m_pPlayerTransformCom(nullptr), m_pPlayerCollisionCom(nullptr)
{
	m_eOBJ_ID = OBJ_MONSTER;
	m_iID = Make_ID();
}

CMonster::~CMonster()
{
}

HRESULT CMonster::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	return S_OK;
}

_int CMonster::Update_GameObject(const _float& fTimeDelta)
{
	if (IsDead()) return RET_DEAD;
	int iExit = CCharacter::Update_GameObject(fTimeDelta);

	MONSTER_STATE state = (MONSTER_STATE)(m_pStateCom->GetCurrentStateID());
	if (state == MS_IDLE)
	{
		_vec3 vDist;
		if(FAILED(GetDistVecToPlayer(vDist))) return iExit;
		_float distLen = D3DXVec3Length(&vDist);
		D3DXVec3Normalize(&m_vDir, &vDist);

		if (m_fAttackableDist >= distLen)
		{
			ChangeState(MS_ATTACK_IDLE);
		}
	}
	return iExit;
}

void CMonster::LateUpdate_GameObject(const _float& fTimeDelta)
{
	SetBillboard();
	CCharacter::LateUpdate_GameObject(fTimeDelta);
}

void CMonster::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pAnimationCom->Render_Animation();
	m_pBufferCom->Render_Buffer();
}

HRESULT CMonster::Add_Component()
{
	if (FAILED(CCharacter::Add_Component())) return E_FAIL;
	Engine::CComponent* pComponent = nullptr;


	return S_OK;
}

void CMonster::SetBillboard()
{
	_matrix matView, matBill;

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);
	_vec3 camPos;
	memcpy(&camPos, &matView.m[3], sizeof(_vec3));
	_vec3 myPos = *m_pTransformCom->Get_Info(INFO_POS);
	_vec3 myScale = m_pTransformCom->m_vScale;

	//방향 주의! 카메라의 방향을 처다봐야 뒷면이 랜더링 됨 
	_vec3 look = myPos - camPos;
	look.y = 0.0f;
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
	memcpy(&matBill.m[2], &look , sizeof(_vec3));
	memcpy(&matBill.m[3], &myPos, sizeof(_vec3));
	m_pTransformCom->Set_World(&matBill);
}


void CMonster::Free()
{
	CCharacter::Free();
}

HRESULT CMonster::GetDistVecToPlayer(_vec3& pOutDist)
{
	if (GetPlayerTransform() == nullptr) return E_FAIL;

 	_vec3* playerPos = GetPlayerTransform()->Get_Info(INFO_POS);
	_vec3* myPos = m_pTransformCom->Get_Info(INFO_POS);

	pOutDist = *playerPos - *myPos;
	return S_OK;
}

Engine::CTransform* CMonster::GetPlayerTransform()
{
	if (!m_pPlayerTransformCom)
	{
		m_pPlayerTransformCom =
			static_cast<CTransform*>(CManagement::GetInstance()->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", OBJ_PLAYER, L"Com_Transform"));
	}
	return m_pPlayerTransformCom;
}

Engine::CCollision* CMonster::GetPlayerCollision()
{
	if (!m_pPlayerCollisionCom)
	{
		m_pPlayerCollisionCom =
			static_cast<CCollision*>(CManagement::GetInstance()->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", OBJ_PLAYER, L"Com_Collision"));
	}
	return m_pPlayerCollisionCom;
}

void CMonster::Launch()
{
	
}

void CMonster::SetLaunched()
{
	_uint curState = m_pStateCom->GetCurrentStateID();
	if (curState == MS_HIT || curState == MS_DEAD) return;
	ChangeState(MS_LAUNCH);
}

void CMonster::Activate()
{
	CCharacter::Activate();
}

void CMonster::Deactivate()
{
	CCharacter::Deactivate();
	m_fHP = m_fMaxHP;
}
