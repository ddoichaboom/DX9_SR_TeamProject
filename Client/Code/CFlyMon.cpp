#include "pch.h"
#include "CFlyMon.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CManagement.h"


//-------------------------------------------------------------------------
// Texture , Animation Data
//-------------------------------------------------------------------------
vector<TextureSource> CFlyMon::m_vTextureSource =
{
	 { MS_IDLE,		L"../Bin/Resource/Texture/Monster/FlyMon/FlyMon_Ready_1024.dds" }
	 ,{CStateComponent::MakeStateID(MS_ATTACK_IDLE, SUB_BEGIN),
			L"../Bin/Resource/Texture/Monster/FlyMon/FlyMon_Attack_Begin_1024.dds"}
	,{ MS_ATTACK_IDLE, L"../Bin/Resource/Texture/Monster/FlyMon/FlyMon_Attack_Idle_1024.dds"}
	,{ MS_ATTACK,	L"../Bin/Resource/Texture/Monster/FlyMon/FlyMon_Attack_1024.dds" }
};
//Loop 인 애니메이션은 Ratio 세팅 금지(디폴트로 두기) . Ratio먹이면 다음 애니메이션이 안나옴 
vector<AnimationSource> CFlyMon::m_vAnimSource =
{
	 {  MS_IDLE ,1,2,2, true}
	,{ CStateComponent::MakeStateID(MS_ATTACK_IDLE, SUB_BEGIN),0,2,2, false, 0.08f, 1.f}
	,{ MS_ATTACK_IDLE,0,2,2, true, 0.11f}
	,{ MS_ATTACK,1,1,1, false, 0.05f,1.f}
};

CFlyMon::CFlyMon(LPDIRECT3DDEVICE9 pGraphicDev)
	:CMonster(pGraphicDev), m_pBodyCollider(nullptr), m_fNear(false)
{
}

CFlyMon::CFlyMon(const CFlyMon& rhs)
	:CMonster(rhs), m_pBodyCollider(nullptr), m_fNear(false)
{
}

CFlyMon::~CFlyMon()
{
}

void CFlyMon::CreateStateData()
{
	auto Mgr = CDataMgr<CFlyMon>::GetInstance();
	//클래스 당 한번만 실행되게
	if (Mgr->IsStateEmpty() == false) return;

	//Idle State
	CState<CFlyMon>* State = new CState<CFlyMon>(nullptr, &CFlyMon::Idle, nullptr);
	Mgr->AddState(MS_IDLE, State);

	//Attack Idle State
	State = new CState<CFlyMon>(nullptr, &CFlyMon::Attack_Idle, nullptr);
	Mgr->AddState(MS_ATTACK_IDLE, State);

	//Attack State
	State = new CState<CFlyMon>(nullptr, &CFlyMon::Attack, nullptr);
	Mgr->AddState(MS_ATTACK, State);

	//Launch State
	State = new CState<CFlyMon>(nullptr, &CFlyMon::Launch, nullptr);
	Mgr->AddState(MS_LAUNCH, State);

	//Dead State
	State = new CState<CFlyMon>(nullptr, &CFlyMon::Dead, nullptr);
	Mgr->AddState(MS_DEAD, State);
}

CFlyMon* CFlyMon::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CFlyMon* pFly = new CFlyMon(pGraphicDev);

	if (FAILED(pFly->Ready_GameObject()))
	{
		Safe_Release(pFly);
		MSG_BOX("FlyMon Create Failed");
		return nullptr;
	}

	return pFly;
}

HRESULT CFlyMon::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	m_pTransformCom->m_vScale = { 11.f, 9.f  ,1.f };
	m_pAnimationCom->Bind_OnChangedFunc([&](_float _aspect) { OnAnimationChange(_aspect); });

	CreateStateData();
	ChangeState(MS_IDLE);

	m_pBodyCollider = m_pCollisionCom->CreateCollider(this, m_szBodyColliderName);
	if (!m_pBodyCollider) return E_FAIL;
	m_pBodyCollider->Set_Scale(_vec3(4, 4, 4));
	m_pBodyCollider->BindFuncToCollision([&](CollisionInfo info)
		{
			OnBodyCollision(info);
		});

	m_fLaunchSpeed = 4.f;
	return S_OK;
}

_int CFlyMon::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CMonster::Update_GameObject(fTimeDelta);
	if (iExit == RET_DEAD) return iExit;

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	m_fTime += fTimeDelta;

	_vec3 pos;
	m_pTransformCom->Get_Info(INFO_POS, &pos);
	Compute_ViewZ(&pos);

	return RET_NONE;
}

void CFlyMon::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CMonster::LateUpdate_GameObject(fTimeDelta);
	if (m_pStateCom->GetCurrentStateID() == MS_ATTACK_IDLE)
	{
		TracePlayer(fTimeDelta);
	}
	m_pAnimationCom->Update_State(m_pStateCom->GetCurrentStateID());
}

void CFlyMon::Render_GameObject()
{
	CMonster::Render_GameObject();
}

void CFlyMon::ChangeState(_uint nextStateID)
{
	m_fTime = 0.f;
	m_pAnimationCom->Update_State(nextStateID);
	m_pStateCom->ChangeState<CFlyMon>(nextStateID);
}

HRESULT CFlyMon::Add_Component()
{
	if (FAILED(CMonster::Add_Component())) return E_FAIL;
	Engine::CComponent* pComponent = nullptr;

	// Animation
	pComponent = m_pAnimationCom = dynamic_cast<Engine::CAnimation*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_FlyMonAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });
	return S_OK;
}

void CFlyMon::OnAnimationChange(_float _animAspect)
{

}

void CFlyMon::OnBodyCollision(CollisionInfo info)
{
	m_pBodyCollider->OffCollision();
	ChangeState(MS_DEAD);
}

void CFlyMon::TracePlayer(const _float& fTimeDelta)
{
	_vec3 dir;
	//_vec3 playerPos = *GetPlayerTransform()->Get_Info(INFO_POS);
	_vec3 cameraPos = *GetCameraTransform()->Get_Info(INFO_POS);
	_vec3 myPos = *m_pTransformCom->Get_Info(INFO_POS);

	dir = cameraPos - myPos;
	D3DXVec3Normalize(&dir, &dir);
	m_pTransformCom->Move_Pos(&dir, fTimeDelta, m_fTraceSpeed);
}


void CFlyMon::Idle()
{

}

void CFlyMon::Attack_Idle()
{
	_vec3 vDist;
	//_vec3 playerPos = *GetPlayerTransform()->Get_Info(INFO_POS);
	_vec3 cameraPos = *GetCameraTransform()->Get_Info(INFO_POS);
	_vec3 myPos = *m_pTransformCom->Get_Info(INFO_POS);
	vDist = cameraPos - myPos;
	if (D3DXVec3Length(&vDist) <= m_fAttackDist)
	{
		ChangeState(MS_ATTACK);
	}
}

void CFlyMon::Attack()
{
	if (m_pAnimationCom->IsEnd())
	{
		ChangeState(MS_LAUNCH);
		m_pAnimationCom->Pause();
	}
}

void CFlyMon::Launch()
{
	//CTransform* playerTransform = GetPlayerTransform();
	CTransform* cameraTransform = GetCameraTransform();
	if (!cameraTransform) return;

	if (m_fTime >= m_fLaunchTime)
	{
		ChangeState(MS_ATTACK_IDLE);
		m_pAnimationCom->PlayFromStart();
		return;
	}
	// 플레이어가 몬스터를 바라보는 방향으로 밀기 
	_vec3 dir = *m_pTransformCom->Get_Info(INFO_POS) - *cameraTransform->Get_Info(INFO_POS);
	dir.y = 0.f;
	D3DXVec3Normalize(&dir, &dir);

	float totalSpeed = easeOutQuint(m_fTime / m_fLaunchTime) * m_fLaunchSpeed;
	m_pTransformCom->Move_Pos(&dir, 1, totalSpeed);
}

void CFlyMon::Dead()
{
	SetDead();
}

void CFlyMon::Activate()
{
	CMonster::Activate();
	ChangeState(MS_IDLE);
}

void CFlyMon::Free()
{
	CMonster::Free();
}
