#include "pch.h"
#include "CWhiteMan.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CCollider.h"
#include "CDInputMgr.h"
#include "CCalculator.h"


CWhiteMan::CWhiteMan(LPDIRECT3DDEVICE9 pGraphicDev)
	:CMonster(pGraphicDev), m_pCollider(nullptr)
{
}

CWhiteMan::CWhiteMan(const CWhiteMan& rhs)
	:CMonster(rhs), m_pCollider(nullptr)
{
}

CWhiteMan::~CWhiteMan()
{

}

HRESULT CWhiteMan::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;

	m_pCollider = CCollider::Create(m_pGraphicDev, m_pTransformCom);
	m_pCollider->Set_Scale(_vec3(0.8, 1.0, 1.5));
	m_pCollider->BindFuncToCollision([&](CollisionInfo info)
		{
			OnCollision(info);
		});

	if (!m_pCollider) return E_FAIL;

	ChangeState(MS_IDLE);
	//m_pAnimationCom->Change_Animation(m_EState);
	m_pTransformCom->m_vScale = { 5,13,1 };
	m_pTransformCom->Set_Pos(0, 0, 20.f);

	return S_OK;
}

_int CWhiteMan::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CMonster::Update_GameObject(fTimeDelta);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	m_pCollider->Update_GameObject(fTimeDelta);

	_vec3 info;
	m_pTransformCom->Get_Info(INFO_POS, &info);
	Compute_ViewZ(&info);

	//TEST
	if (CDInputMgr::GetInstance()->Get_DIMouseState(DIM_LB) & 0x80)
	{
		CollisionInfo info;
		bool result = m_pCalculatorCom->Check_PickedCollider(g_hWnd, m_pCollider);
		if (result) m_pCollider->Collision(info);
	}

	UpdateState();

	return iExit;
}

void CWhiteMan::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CMonster::LateUpdate_GameObject(fTimeDelta);
	m_pCollider->LateUpdate_GameObject(fTimeDelta);

}

void CWhiteMan::Render_GameObject()
{
	CMonster::Render_GameObject();
	m_pCollider->Render_GameObject();
}

HRESULT CWhiteMan::Add_Component()
{
	if (FAILED(CMonster::Add_Component())) return E_FAIL;
	Engine::CComponent* pComponent = nullptr;

	// Animation
	pComponent = m_pAnimationCom = dynamic_cast<Engine::CAnimation*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_WhiteManAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });

	//Calculator
	pComponent = m_pCalculatorCom = dynamic_cast<Engine::CCalculator*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Calculator"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });


	return S_OK;
}


void CWhiteMan::ChangeState(MONSTER_STATE nextState)
{
	if (nextState == m_EState) return;
	m_EState = nextState;
	m_pAnimationCom->Change_Animation(m_EState);
}

void CWhiteMan::Free()
{
	Safe_Release(m_pCollider);
	CMonster::Free();
}

void CWhiteMan::OnCollision(CollisionInfo info)
{
	ChangeState(MS_HIT);
}

CWhiteMan* CWhiteMan::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CWhiteMan* pWhite = new CWhiteMan(pGraphicDev);

	if (FAILED(pWhite->Ready_GameObject()))
	{
		Safe_Release(pWhite);
		MSG_BOX("White Man Create Failed");
		return nullptr;
	}

	return pWhite;
}

void CWhiteMan::UpdateState()
{
	if (m_EState == MS_HIT)
	{
		if (m_pAnimationCom->IsEnd())
		{
			ChangeState(MS_ATTACK);
		}
	}
}

