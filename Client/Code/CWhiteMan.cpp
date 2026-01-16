#include "pch.h"
#include "CWhiteMan.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CManagement.h"
#include "CBullet.h"
#include "CPoolMgr.h"


_uint CWhiteMan::ID_SLICE_DEAD = CStateComponent::MakeStateID(MS_DEAD, SUB_NONE, SLICE);
_uint CWhiteMan::ID_ELECT_DEAD = CStateComponent::MakeStateID(MS_DEAD, SUB_NONE, ELECT);
_uint CWhiteMan::ID_HEAD_DEAD = CStateComponent::MakeStateID(MS_DEAD, SUB_NONE, HEAD);

_uint CWhiteMan::ID_FLYBACK_BEGIN = CStateComponent::MakeStateID(MS_FLYBACK, SUB_BEGIN);
_uint CWhiteMan::ID_FLYBACK_END_WALL = CStateComponent::MakeStateID(MS_FLYBACK, SUB_NONE, DEST_WALL);
_uint CWhiteMan::ID_FLYBACK_END_GROUND = CStateComponent::MakeStateID(MS_FLYBACK, SUB_NONE, DEST_GROUND);

//-------------------------------------------------------------------------
// Texture , Animation Data
//-------------------------------------------------------------------------
vector<TextureSource> CWhiteMan::m_vTextureSource =
{
	 { MS_IDLE,		L"../Bin/Resource/Texture/Monster/WhiteMan/white_Idle_1024.dds" }
	,{ CStateComponent::MakeStateID(MS_ATTACK_IDLE, SUB_BEGIN),
		L"../Bin/Resource/Texture/Monster/WhiteMan/white_Aiming_1024.dds"}
	,{ MS_ATTACK_IDLE, L"../Bin/Resource/Texture/Monster/WhiteMan/white_AttackIdle_1024.dds"}
	,{ MS_ATTACK,	L"../Bin/Resource/Texture/Monster/WhiteMan/white_Attack2_1024.dds" }
	,{ MS_WALK,		L"../Bin/Resource/Texture/Monster/WhiteMan/white_Walk_1024.dds" }
	,{ MS_HIT,		L"../Bin/Resource/Texture/Monster/WhiteMan/white_Hit_1024.dds" }
	,{ MS_LAUNCH,	L"../Bin/Resource/Texture/Monster/WhiteMan/Launch_1024.dds" }

	,{ ID_SLICE_DEAD, L"../Bin/Resource/Texture/Monster/WhiteMan/KatanaDead_1024.dds" }
	,{ ID_ELECT_DEAD, L"../Bin/Resource/Texture/Monster/WhiteMan/Elect_End_1024.dds" }
	,{ ID_HEAD_DEAD, L"../Bin/Resource/Texture/Monster/WhiteMan/headDead_512.dds" }

	,{ MS_FLYBACK, L"../Bin/Resource/Texture/Monster/WhiteMan/FlyBack_1024.dds" }
	,{ ID_FLYBACK_BEGIN, L"../Bin/Resource/Texture/Monster/WhiteMan/FlyBack_Begin_1024.dds" }
	,{ ID_FLYBACK_END_WALL, L"../Bin/Resource/Texture/Monster/WhiteMan/FlyBack_End_Wall_1024.dds" }
	,{ ID_FLYBACK_END_GROUND, L"../Bin/Resource/Texture/Monster/WhiteMan/FlyBack_End_Ground_1024.dds" }

	,{ MS_DEAD,		L"../Bin/Resource/Texture/Monster/WhiteMan/white_DeadBack_512.dds" }
};
//Loop 인 애니메이션은 Ratio 세팅 금지(디폴트로 두기) . Ratio먹이면 다음 애니메이션이 안나옴 
vector<AnimationSource> CWhiteMan::m_vAnimSource =
{
	{  MS_IDLE ,1,5,5, true, 0.13f}						//IDLE
	,{ CStateComponent::MakeStateID(MS_ATTACK_IDLE, SUB_BEGIN),1,4,3, false, 0.08f, 1.f}				//Aiming
	,{ MS_ATTACK_IDLE,1,3,2, true, 0.11f}				//Attack_Idle
	,{ MS_ATTACK,1,4,3, false, 0.08f}					//Attack
	,{ MS_WALK,1,6,5, true, 0.11f}						//Walk
	,{ MS_HIT,1,2,2, false, 0.09f, 1.f, true}			//Hit
	,{ MS_LAUNCH,1,2,1, false, 0.06f, 1.f, true}		//Launch

	,{ ID_SLICE_DEAD ,3,4,4, false, 0.11f, 1.f, true}	//Slice Dead
	,{ ID_ELECT_DEAD ,3,3,2, false, 0.06f, 1.f, true}	//Elect Dead
	,{ ID_HEAD_DEAD,5,3,1, false, 0.10f, 1.f, true}		//Head Dead

	,{ MS_FLYBACK,1,3,2, true, 0.04f}						//Fly Back
	,{ ID_FLYBACK_BEGIN,1,3,2, false, 0.06f, 1.f, true}		//Fly Back Begin
	,{ ID_FLYBACK_END_WALL,3,3,1, false, 0.07f, 1.f, true}	//Fly Back End To Wall
	,{ ID_FLYBACK_END_GROUND,3,3,3, false, 0.07f, 1.f, true}//Fly Back End To Ground

	,{ MS_DEAD,6,3,2, false, 0.06f, 1.f, true}			//Dead
};

CWhiteMan::CWhiteMan(LPDIRECT3DDEVICE9 pGraphicDev)
	:CMonster(pGraphicDev), m_pHeadCollider(nullptr), m_pBodyCollider(nullptr)
{
}

CWhiteMan::CWhiteMan(const CWhiteMan& rhs)
	:CMonster(rhs), m_pHeadCollider(nullptr), m_pBodyCollider(nullptr)
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

	//Launch State 
	State = new CState<CWhiteMan>(nullptr, &CWhiteMan::Launch, nullptr);
	Mgr->AddState(MS_LAUNCH, State);


	//FlyBack State
	State = new CState<CWhiteMan>(&CWhiteMan::FlyBack_Begin, &CWhiteMan::FlyBack, nullptr);
	Mgr->AddState(MS_FLYBACK, State);

	State = new CState<CWhiteMan>(nullptr, &CWhiteMan::Fly_BlockWall, nullptr);
	Mgr->AddState(ID_FLYBACK_END_WALL, State);

	State = new CState<CWhiteMan>(nullptr, &CWhiteMan::Fly_FallGournd, nullptr);
	Mgr->AddState(ID_FLYBACK_END_GROUND, State);

	//Dead State
	State = new CState<CWhiteMan>(nullptr, &CWhiteMan::Slice, nullptr);
	Mgr->AddState(ID_SLICE_DEAD, State);

	//Dead State
	State = new CState<CWhiteMan>(nullptr, &CWhiteMan::Elect, nullptr);
	Mgr->AddState(ID_ELECT_DEAD, State);

	//Dead State
	State = new CState<CWhiteMan>(nullptr, &CWhiteMan::Dead, nullptr);
	Mgr->AddState(ID_HEAD_DEAD, State);

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

CWhiteMan* CWhiteMan::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
	CWhiteMan* pWhite = new CWhiteMan(pGraphicDev);

	if (FAILED(pWhite->Ready_GameObject()))
	{
		Safe_Release(pWhite);
		MSG_BOX("White Man Create Failed");
		return nullptr;
	}

	CTransform* pTransform = dynamic_cast<Engine::CTransform*>(
		pWhite->Get_Component(ID_DYNAMIC, L"Com_Transform"));
	pTransform->Set_Pos(vPos);
	pTransform->Update_Component(0.f);

	return pWhite;
}
//Static End



HRESULT CWhiteMan::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	//상태 객체 생성
	CreateStateData();
	ChangeState(MS_IDLE);

	//애니메이션 텍스쳐에 맞게 스케일 조정용 
	//애니메이션에 스케일 다른 텍스쳐가 있을때만 바인딩하기 
	m_pAnimationCom->Bind_OnChangedFunc([&](_float _aspect) { OnAnimationChange(_aspect); });

	m_pTransformCom->m_vScale = { 5.f, 13.f  ,1.f };

	//Collider 생성 
	m_pHeadCollider = m_pCollisionCom->CreateCollider(this, m_szHeadColliderName);

	if (!m_pHeadCollider) return E_FAIL;

	m_pHeadCollider->Set_RelativePos(_vec3(0,9.5f,0));
	m_pHeadCollider->Set_Scale(_vec3(2,2,2));
	//콜라이더가 충돌되면 호출될 함수를 바인딩하기. CollisionInfo는 충돌 정보 
	//웬만하면 아래처럼 람다로 넣기
	m_pHeadCollider->BindFuncToCollision([&](CollisionInfo info)
		{
			OnHeadCollision(info);
		});

	m_pBodyCollider = m_pCollisionCom->CreateCollider(this, m_szBodyColliderName);
	if (!m_pBodyCollider) return E_FAIL;

	m_pCollisionCom->SetMainCollider(m_szBodyColliderName);
	m_pBodyCollider->Set_RelativePos(_vec3(0, -2.5f, 0));
	m_pBodyCollider->Set_Scale(_vec3(4,10,4));
	m_pBodyCollider->BindFuncToCollision([&](CollisionInfo info)
		{
			OnBodyCollision(info);
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

	return iExit;
}

void CWhiteMan::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CMonster::LateUpdate_GameObject(fTimeDelta);
	////현재 상태에 맞는 애니메이션으로 자동 전환
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

	// Animation
	pComponent = m_pAnimationCom = dynamic_cast<Engine::CAnimation*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_WhiteManAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });
	return S_OK;
}

void CWhiteMan::ChangeState(_uint nextStateID)
{
	//m_fTime은 맨 위로 고정! ChangeState에서 실행되는 함수(Begin,End)에서 fTime을 바꿀수도있음
	m_fTime = 0.f;
	//현재 상태에 맞는 애니메이션으로 자동 전환
	m_pAnimationCom->Update_State(nextStateID);
	//템플릿 멤버함수! 주의 ! 
	m_pStateCom->ChangeState<CWhiteMan>(nextStateID);

}


void CWhiteMan::OnHeadCollision(CollisionInfo info)
{
	if (info.eTag == TAG_KICK || info.eTag == TAG_SLIDE)
		return;

	m_fHP = 0.f;
	if (m_pHeadCollider) m_pHeadCollider->OffCollision();
	if (m_pBodyCollider) m_pBodyCollider->OffCollision();
	if (info.eTag == TAG_KATANA) ChangeState(ID_SLICE_DEAD);
	else ChangeState(ID_HEAD_DEAD);

	/*m_fHP -= info.fDamage;
	if (m_fHP <= 0.f)
	{
		if (m_pHeadCollider) m_pHeadCollider->OffCollision();
		if (m_pBodyCollider) m_pBodyCollider->OffCollision();
		if (info.eTag == TAG_KATANA) ChangeState(ID_SLICE_DEAD);
		else ChangeState(ID_HEAD_DEAD);
	}
	else ChangeState(MS_HIT);*/
}

void CWhiteMan::OnBodyCollision(CollisionInfo info)
{
	if (info.eDir != CDIR_NONE)
	{
		Move_ByCollision(info.eDir, info.vDiff);
		//지형충돌 
		if (m_pStateCom->GetCurrentStateID() == MS_FLYBACK )
		{
			ChangeState(ID_FLYBACK_END_WALL);
			return;
		}
	}

	if (info.eTag == TAG_SLIDE)
	{
		if (m_pHeadCollider) m_pHeadCollider->OffCollision();
		if (m_pBodyCollider) m_pBodyCollider->OffCollision();
		m_fHP = 0.f;
		ChangeState(MS_FLYBACK);
		return;
	}

	m_fHP -= info.fDamage;
	if (m_fHP <= 0.f)
	{
		if (m_pHeadCollider) m_pHeadCollider->OffCollision();
		if (m_pBodyCollider) m_pBodyCollider->OffCollision();
		if (info.eTag == TAG_KATANA) ChangeState(ID_SLICE_DEAD);
		else ChangeState(MS_DEAD);
	}
	else if (info.eTag == TAG_KICK)
	{
		SetLaunched();
	}
	else
		ChangeState(MS_HIT);
}



void CWhiteMan::Idle()
{

}


void CWhiteMan::Begin_Attack()
{
	_uint prevState = m_pStateCom->GetPrevStateID();
	if (prevState == MS_HIT || prevState == MS_LAUNCH)
	{
		m_fTime = m_fAttackDelayTime * 0.9f;
	}
	else m_fTime = m_fAttackDelayTime;
}

void CWhiteMan::Idle_Attack()
{
	//attack begin 애니메이션이 플레이 중이면 대기
	if (m_pAnimationCom->Get_State()!= MS_ATTACK_IDLE || m_pAnimationCom->GetSubState() == SUB_BEGIN) return;
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

	CBullet* pBullet = CPoolMgr::GetInstance()->Get_Object<CBullet>();
	if (!pBullet) return;

	_vec3 myPos = *m_pTransformCom->Get_Info(INFO_POS);
	//TODO : 수치 테스트 후 상수 + 함수로 수정하기 
	myPos.y += 6.f;
	_vec3 otherPos = { 0.f,0.f,0.f };
	//if (GetPlayerTransform()) otherPos = *GetPlayerTransform()->Get_Info(INFO_POS);
	if (GetCameraTransform()) otherPos = *GetCameraTransform()->Get_Info(INFO_POS);
	otherPos.y -= 1.0f;
	pBullet->SetPos(myPos);

	_vec3 dir = otherPos - myPos;
	D3DXVec3Normalize(&dir, &dir);
	pBullet->SetDirection(dir);

	CLayer* layer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");
	if (!layer) pBullet->ReturnToPool();
	else layer->Add_GameObject(pBullet);

}

void CWhiteMan::Hit()
{
	if (m_pAnimationCom->CanEnd())
	{
		ChangeState(MS_ATTACK_IDLE);
		// MS_ATTACK_IDLE의 BEGIN용 애니메이션을 건너뛰기 
		m_pAnimationCom->PlayNextAnim();
	}
}

void CWhiteMan::Launch()
{
	CTransform* playerTransform = GetPlayerTransform();
	if (!playerTransform) return;

	if (m_fTime >= m_fLaunchTime)
	{
		if (m_bLaunchEnd)
		{
			if (m_pAnimationCom->IsEnd())
			{
				ChangeState(MS_ATTACK_IDLE);
				m_pAnimationCom->PlayNextAnim();
				m_bLaunchEnd = false;
			}
		}
		else
		{
			m_pAnimationCom->Play();
			m_bLaunchEnd = true;
		}
		return;
	}
	else
	{
		m_pAnimationCom->Pause();
	}
	// 플레이어가 몬스터를 바라보는 방향으로 밀기 
	//_vec3 dir = *m_pTransformCom->Get_Info(INFO_POS) - *playerTransform->Get_Info(INFO_POS);
	_vec3 dir = *playerTransform->Get_Info(INFO_LOOK);
	dir.y = 0.f;
	D3DXVec3Normalize(&dir, &dir);

	float totalSpeed = easeOutQuint(m_fTime/m_fLaunchTime) * m_fLaunchSpeed;
	m_pTransformCom->Move_Pos(&dir, 1, totalSpeed);

}

void CWhiteMan::Dead()
{
	if (m_pAnimationCom->IsEnd())
	{
		SetDead();
	}
}
void CWhiteMan::Elect()
{
}
void CWhiteMan::Slice()
{
	if (m_pAnimationCom->IsEnd())
	{
		SetDead();
	}
}
void CWhiteMan::Bomb()
{
}
void CWhiteMan::FlyBack_Begin()
{
	CTransform* pCamTransform = GetCameraTransform();
	if (!pCamTransform) return;
	_vec3 vDir = *pCamTransform->Get_Info(INFO_LOOK);
	vDir.y = 0.f;
	D3DXVec3Normalize(&m_FlyDir, &vDir);


}
void CWhiteMan::FlyBack()
{
	if (m_fTime > m_fFlyBackTime)
	{
		ChangeState(ID_FLYBACK_END_GROUND);
	}

	float totalSpeed = easeOutQuint(m_fTime / m_fFlyBackTime) * m_fFlyBackSpeed;
	m_pTransformCom->Move_Pos(&m_FlyDir, 1, totalSpeed);

}
void CWhiteMan::Fly_BlockWall()
{
	if (m_pAnimationCom->IsEnd())
	{
		SetDead();
	}
}
void CWhiteMan::Fly_FallGournd()
{
	if (m_pAnimationCom->IsEnd())
	{
		SetDead();
	}
}
// _animAspect = cutSize.x / cutSize.y 한 종횡비 
// 애니메이션마다 크기가 다를경우 오브젝트의 scale을 조정하기위함
void CWhiteMan::OnAnimationChange(_float _animAspect)
{
	_vec3 scale = m_pTransformCom->Get_Scale();
	scale.x = scale.y * _animAspect;
	m_pTransformCom->Set_Scale(scale.x, scale.y, scale.z);
}

void CWhiteMan::Activate()
{
	CMonster::Activate();
	ChangeState(MS_IDLE);
}


void CWhiteMan::Free()
{
	CMonster::Free();
}