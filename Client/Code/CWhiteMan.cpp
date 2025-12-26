#include "pch.h"
#include "CWhiteMan.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CCollider.h"
#include "CDInputMgr.h"
#include "CCalculator.h"
#include "CDataMgr.h"
#include "CStateComponent.h"

//-------------------------------------------------------------------------
// Texture , Animation Data
//-------------------------------------------------------------------------
vector<TextureSource> CWhiteMan::m_vTextureSource =
{
	 { MS_IDLE,		L"../Bin/Resource/Texture/Test/white_Idle_1024.png" }
	,{ MS_FOUND,	L"../Bin/Resource/Texture/Test/white_Aiming_1024.png" }
	,{ GetStateID(MS_ATTACK, SUB_BEGIN), L"../Bin/Resource/Texture/Test/white_AttackIdle_1024.png"}
	,{ MS_ATTACK,	L"../Bin/Resource/Texture/Test/white_Attack2_1024.png" }
	,{ MS_WALK,		L"../Bin/Resource/Texture/Test/white_Walk_1024.png" }
	,{ MS_HIT,		L"../Bin/Resource/Texture/Test/white_Hit_1024.png" }
	,{ MS_DEAD,		L"../Bin/Resource/Texture/Test/white_DeadBack_512.png" }
};

vector<AnimationSource> CWhiteMan::m_vAnimSource =
{
	{  MS_IDLE ,1,5,5, true, 0.13f}		//IDLE
	,{ MS_FOUND,1,4,3, false, 0.11f}	//Aiming
	,{ GetStateID(MS_ATTACK, SUB_BEGIN),1,3,2, true, 0.11f}	//AttackStart
	,{ MS_ATTACK,1,4,3, true, 0.09f}	//Attack2
	,{ MS_WALK,1,6,5, true, 0.11f}	//Walk
	,{ MS_HIT,1,3,2, false, 0.11f}	//Hit
	,{ MS_DEAD,6,3,2, true, 0.11f}	//DeadBack
};

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

void CWhiteMan::CreateStateData()
{
	auto Mgr = CDataMgr<CWhiteMan>::GetInstance();
	//클래스 당 한번만 실행되게
	if (Mgr->IsStateEmpty() == false) return;

	//Create State  
	//순서대로 Begin , Update, End 함수 포인터에 대한 매개변수
	//&클래스명::함수명으로 넣기 , 없으면 nullptr
	CState<CWhiteMan>* State = new CState<CWhiteMan>(&CWhiteMan::Begin_Idle, nullptr, nullptr);
	Mgr->AddState(MS_IDLE, State);

	State = new CState<CWhiteMan>(&CWhiteMan::Begin_Attack, &CWhiteMan::Attack, &CWhiteMan::End_Attack);
	Mgr->AddState(MS_ATTACK, State);

	State = new CState<CWhiteMan>(&CWhiteMan::Begin_Hit, &CWhiteMan::Hit, nullptr);
	Mgr->AddState(MS_HIT, State);

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
//Static End



HRESULT CWhiteMan::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	CreateStateData();
	ChangeState(MS_IDLE);

	m_pCollider = CCollider::Create(m_pGraphicDev, m_pTransformCom);
	m_pCollider->Set_Scale(_vec3(0.8, 1.0, 1.5));
	m_pCollider->BindFuncToCollision([&](CollisionInfo info)
		{
			OnCollision(info);
		});

	if (!m_pCollider) return E_FAIL;

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
	//TODO : 플레이어에 공격 구현되면 지우기 
	if (CDInputMgr::GetInstance()->Get_DIMouseState(DIM_LB) & 0x80)
	{
		CollisionInfo info;
		bool result = m_pCalculatorCom->Check_PickedCollider(g_hWnd, m_pCollider);
		if (result) m_pCollider->Collision(info);
	}

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

	//StateComponent
	pComponent = m_pStateCom = dynamic_cast<Engine::CStateComponent*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_StateComponent"));

	//Owner 지정해주기!! 
	m_pStateCom->SetOnwer(this);

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_StateComponent", pComponent });


	return S_OK;
}

void CWhiteMan::ChangeState(MONSTER_STATE nextState)
{
	CState<CWhiteMan>* state = CDataMgr<CWhiteMan>::GetInstance()->GetState(nextState);
	m_pStateCom->ChangeState(state);
	//State를 측정할 변수가 필요하면 m_fTime 쓰기 
	//m_fTime = 0.f;
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

void CWhiteMan::Begin_Idle()
{
	m_pAnimationCom->Change_Animation(MS_IDLE);
}

void CWhiteMan::Begin_Attack()
{
	m_pAnimationCom->Change_Animation(MS_ATTACK);
}

void CWhiteMan::Attack()
{
	
}

void CWhiteMan::End_Attack()
{
}

void CWhiteMan::Begin_Hit()
{
	m_pAnimationCom->Change_Animation(MS_HIT);
}

void CWhiteMan::Hit()
{
	if (m_pAnimationCom->IsEnd())
	{
		ChangeState(MS_ATTACK);
	}

}
