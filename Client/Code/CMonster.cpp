#include "pch.h"
#include "CMonster.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CManagement.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	:CCharacter(pGraphicDev), m_pAnimationCom(nullptr)
	,m_fAttackableDist(100.f), m_vDir({0,0,0}), m_fSpeed(10.f), m_fHP(10.f)
{
}

CMonster::CMonster(const CMonster& rhs)
	:CCharacter(rhs), m_pAnimationCom(nullptr), m_fAttackableDist(100.f), m_fHP(10.f)
	, m_vDir(rhs.m_vDir), m_fSpeed(rhs.m_fSpeed)
{
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

	m_pAnimationCom->LateRender_Animation();
}

HRESULT CMonster::Add_Component()
{
	if (FAILED(CCharacter::Add_Component())) return E_FAIL;
	Engine::CComponent* pComponent = nullptr;

	// Animation
	pComponent = m_pAnimationCom = dynamic_cast<Engine::CAnimation*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_WhiteManAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });


	return S_OK;
}

//카메라위치->몬스터위치 방향으로 미리 처다보게 만들어서 회전시키면 카메라를 항상 처다본다
//현 몬스터들은 바라보는 방향 반대에서 랜더링되므로 카메라 방향을 향해야함 
//뷰포트 각도로 처리하면 마우스 움직임에도 영향을 받으므로 위치값을 기준으로 하기 
void CMonster::SetBillboard()
{
	_matrix matWorld, matView, matBill, matScale, matScaleInverse;

	matWorld = *m_pTransformCom->Get_World();

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);
	_vec3* Front = m_pTransformCom->Get_Info(INFO_LOOK);
	_vec3 dir;
	memcpy(&dir, &matView.m[3], sizeof(_vec3));
	dir = *m_pTransformCom->Get_Info(INFO_POS)- dir; // 카메라 -> 몬스터 방향
	D3DXVec3Normalize(&dir, &dir);
	_float value = acosf(D3DXVec3Dot(Front, &dir));
	if (dir.x <= 0.f) value *= -1.f;

	D3DXMatrixRotationY(&matBill, value);

	D3DXMatrixScaling(&matScale, m_pTransformCom->m_vScale.x, m_pTransformCom->m_vScale.y, m_pTransformCom->m_vScale.z);

	D3DXMatrixInverse(&matScaleInverse, 0, &matScale);

	matWorld = matScaleInverse * matWorld;
	matWorld = matScale * matBill * matWorld;
	m_pTransformCom->Set_World(&matWorld);

	_vec3		vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

}

void CMonster::Free()
{
	CCharacter::Free();
}

HRESULT CMonster::GetDistVecToPlayer(_vec3& pOutDist)
{
	CTransform* playerTranform =  
		static_cast<CTransform*>(CManagement::GetInstance()->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));
	if (playerTranform == nullptr) return E_FAIL;

 	_vec3* playerPos = playerTranform->Get_Info(INFO_POS);
	_vec3* myPos = m_pTransformCom->Get_Info(INFO_POS);

	pOutDist = *playerPos - *myPos;
	return S_OK;
}
