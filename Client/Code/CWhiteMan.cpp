#include "pch.h"
#include "CWhiteMan.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"

//-------------------------------------------------------------------------
// Texture , Animation Data
//-------------------------------------------------------------------------
vector<TextureSource> CWhiteMan::m_vTextureSource =
{
	 { MS_IDLE,		L"../Bin/Resource/Texture/Monster/WhiteMan/white_Idle_1024.png" }
	,{ CStateComponent::MakeStateID(MS_ATTACK_IDLE, SUB_BEGIN),
		L"../Bin/Resource/Texture/Monster/WhiteMan/white_Aiming_1024.png"}
	,{ MS_ATTACK_IDLE, L"../Bin/Resource/Texture/Monster/WhiteMan/white_AttackIdle_1024.png"}
	,{ MS_ATTACK,	L"../Bin/Resource/Texture/Monster/WhiteMan/white_Attack2_1024.png" }
	,{ MS_WALK,		L"../Bin/Resource/Texture/Monster/WhiteMan/white_Walk_1024.png" }
	,{ MS_HIT,		L"../Bin/Resource/Texture/Monster/WhiteMan/white_Hit_1024.png" }
	,{ MS_DEAD,		L"../Bin/Resource/Texture/Monster/WhiteMan/white_DeadBack_512.png" }
};
//Loop 인 애니메이션은 Ratio 세팅 금지(디폴트로 두기) . Ratio먹이면 다음 애니메이션이 안나옴 
vector<AnimationSource> CWhiteMan::m_vAnimSource =
{
	{  MS_IDLE ,1,5,5, true, 0.13f}					//IDLE
	,{ CStateComponent::MakeStateID(MS_ATTACK_IDLE, SUB_BEGIN),1,4,3, false, 0.08f, 1.f}				//Aiming
	,{ MS_ATTACK_IDLE,1,3,2, true, 0.11f}			//Attack_Idle
	,{ MS_ATTACK,1,4,3, false, 0.08f}				//Attack
	,{ MS_WALK,1,6,5, true, 0.11f}					//Walk
	,{ MS_HIT,1,3,2, false, 0.11f, 0.9f,true}		//Hit
	,{ MS_DEAD,6,3,2, false, 0.08f, 1.f}			//DeadBack
};

CWhiteMan::CWhiteMan(LPDIRECT3DDEVICE9 pGraphicDev)
	:CMonster(pGraphicDev)
{
}

CWhiteMan::CWhiteMan(const CWhiteMan& rhs)
	:CMonster(rhs)
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
	
	//IdleState
	CState<CWhiteMan>* State = new CState<CWhiteMan>(nullptr, &CWhiteMan::Idle, nullptr);
	Mgr->AddState(MS_IDLE, State);

	//Attack Idle State
	State = new CState<CWhiteMan>(&CWhiteMan::Begin_Attack, &CWhiteMan::Idle_Attack, &CWhiteMan::End_Attack);
	Mgr->AddState(MS_ATTACK_IDLE, State);

	//Hit State
	State = new CState<CWhiteMan>(nullptr, &CWhiteMan::Hit, nullptr);
	Mgr->AddState(MS_HIT, State);

	//Dead State
	State = new CState<CWhiteMan>(nullptr, &CWhiteMan::Dead, nullptr);
	Mgr->AddState(MS_DEAD, State);
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

	m_pTransformCom->m_vScale = { 5,13,1 };
	m_pTransformCom->Set_Pos(0, 0, 120.f);

	//콜라이더 생성 방법 
	m_pCollisionCom->CreateCollider(m_pTransformCom);

	CCollider* m_pCollider = m_pCollisionCom->GetCollider();
	if (!m_pCollider) return E_FAIL;

	m_pCollider->Set_Scale(_vec3(4,11,4));
	//콜라이더가 충돌되면 호출될 함수를 바인딩하기. CollisionInfo는 충돌 정보 
	//웬만하면 아래처럼 람다로 넣기
	m_pCollider->BindFuncToCollision([&](CollisionInfo info)
		{
			OnCollision(info);
		});


	return S_OK;
}

_int CWhiteMan::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CMonster::Update_GameObject(fTimeDelta);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	m_fTime += fTimeDelta;

	_vec3 info;
	m_pTransformCom->Get_Info(INFO_POS, &info);
	Compute_ViewZ(&info);

	//TEST
	//TODO : 플레이어에 공격 구현되면 지우기 
	if (CDInputMgr::GetInstance()->Mouse_Down(DIM_LB))
	{
		bool bPicked = m_pCollisionCom->Collision_Mouse(g_hWnd);
		if (bPicked)
		{
			CollisionInfo info = { NULL, {0,0,0}, 6.f };
			m_pCollisionCom->SetCollision(info);
		}
	}

	return iExit;
}

void CWhiteMan::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CMonster::LateUpdate_GameObject(fTimeDelta);
	//현재 상태에 맞는 애니메이션으로 자동 전환
	m_pAnimationCom->Update_State(m_pStateCom->GetCurrentStateID());
}

void CWhiteMan::Render_GameObject()
{
	CMonster::Render_GameObject();
}

HRESULT CWhiteMan::Add_Component()
{
	if (FAILED(CMonster::Add_Component())) return E_FAIL;
	Engine::CComponent* pComponent = nullptr;

	return S_OK;
}

void CWhiteMan::ChangeState(_uint nextStateID)
{
	//m_fTime은 맨 위로 고정! ChangeState에서 실행되는 함수(Begin,End)에서 fTime을 바꿀수도있음
	m_fTime = 0.f;
	//템플릿 멤버함수! 주의 ! 
	m_pStateCom->ChangeState<CWhiteMan>(nextStateID);
}

void CWhiteMan::Free()
{
	CMonster::Free();
}

void CWhiteMan::OnCollision(CollisionInfo info)
{
	m_fHP -= info.fDamage;
	if (m_fHP <= 0.f)
	{
		//TEST
		//TODO : 애니메이션에 텍스처별 비율 조정 구현하기 
		m_pTransformCom->m_vScale.x = m_pTransformCom->m_vScale.y;
		ChangeState(MS_DEAD);
	}
	else ChangeState(MS_HIT);
}

void CWhiteMan::Idle()
{

}


void CWhiteMan::Begin_Attack()
{
	if (m_pStateCom->GetPrevStateID() == MS_HIT)
	{
		m_fTime = m_fAttackDelayTime * 0.7f;
	}
	else m_fTime = m_fAttackDelayTime;
}

void CWhiteMan::Idle_Attack()
{
	//attack begin 애니메이션이 플레이 중이면 대기
	if (m_pAnimationCom->GetSubState() == SUB_BEGIN) return;
	if (m_fTime >= m_fAttackDelayTime)
	{
		Shoot();
		m_fTime = 0.f;
	}

}

void CWhiteMan::End_Attack()
{
}

void CWhiteMan::Shoot()
{
	m_pAnimationCom->PlayOnce(MS_ATTACK);
}

void CWhiteMan::Hit()
{
	if (m_pAnimationCom->CanEnd())
	{
		ChangeState(MS_ATTACK_IDLE);
		m_pAnimationCom->Update_State(MS_ATTACK_IDLE);
		m_pAnimationCom->PlayNextAnim();
	}
}

void CWhiteMan::Dead()
{
	if (m_pAnimationCom->IsEnd())
	{
		SetDead();
		m_pCollisionCom->OffCollision();
	}
}
