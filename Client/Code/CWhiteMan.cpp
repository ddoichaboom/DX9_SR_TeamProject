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
	,{ MS_FOUND,1,4,3, false, 0.11f, 1.f}	//Aiming
	,{ GetStateID(MS_ATTACK, SUB_BEGIN),1,3,2, true, 0.11f,0.9f}	//AttackStart
	,{ MS_ATTACK,1,4,3, true, 0.09f}	//Attack2
	,{ MS_WALK,1,6,5, true, 0.11f}	//Walk
	,{ MS_HIT,1,3,2, false, 0.11f,0.9f}	//Hit
	,{ MS_DEAD,6,3,2, true, 0.11f,1.f}	//DeadBack
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

	m_pTransformCom->m_vScale = { 5,13,1 };
	m_pTransformCom->Set_Pos(0, 0, 20.f);

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

	_vec3 info;
	m_pTransformCom->Get_Info(INFO_POS, &info);
	Compute_ViewZ(&info);

	//TEST
	//TODO : 플레이어에 공격 구현되면 지우기 
	if (CDInputMgr::GetInstance()->Mouse_Down(DIM_LB))
	{
		m_pCollisionCom->Collision_Mouse(g_hWnd);
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
	//템플릿 멤버함수! 주의 ! 
	m_pStateCom->ChangeState<CWhiteMan>(nextStateID);
	//추가로 처리해야 할 작업이 있을까봐 따로 함수로 구현
}

void CWhiteMan::Free()
{
	CMonster::Free();
}

void CWhiteMan::OnCollision(CollisionInfo info)
{
	ChangeState(MS_HIT);
}

void CWhiteMan::Begin_Idle()
{
}

void CWhiteMan::Begin_Attack()
{
}

void CWhiteMan::Attack()
{
	
}

void CWhiteMan::End_Attack()
{
}

void CWhiteMan::Begin_Hit()
{
}

void CWhiteMan::Hit()
{
	ChangeState(MS_ATTACK);
}
