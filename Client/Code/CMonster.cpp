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
	return S_OK;
}

void CMonster::Free()
{
	CCharacter::Free();
}
