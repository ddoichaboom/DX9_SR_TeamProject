#include "pch.h"
#include "CMonster.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	:CCharacter(pGraphicDev), m_pAnimationCom(nullptr)
	,m_fPerceiveDist(20.f), m_pTarget(nullptr)
{
}

CMonster::CMonster(const CMonster& rhs)
	:CCharacter(rhs), m_pAnimationCom(nullptr)
	, m_fPerceiveDist(20.f), m_pTarget(nullptr)
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
	return iExit;
}

void CMonster::LateUpdate_GameObject(const _float& fTimeDelta)
{
	_matrix matWorld, matView, matBill, matScale, matScaleInverse;

	matWorld = *m_pTransformCom->Get_World();

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixIdentity(&matBill);
	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;
	D3DXMatrixInverse(&matBill, 0, &matBill);



	D3DXMatrixScaling(&matScale, m_pTransformCom->m_vScale.x, m_pTransformCom->m_vScale.y, m_pTransformCom->m_vScale.z);

	D3DXMatrixInverse(&matScaleInverse, 0, &matScale);

	matWorld = matScaleInverse * matWorld;

	matWorld = matScale * matBill * matWorld;

	m_pTransformCom->Set_World(&matWorld);

	_vec3		vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

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

void CMonster::Free()
{
	CCharacter::Free();
}
