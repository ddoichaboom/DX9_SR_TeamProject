#include "pch.h"
#include "CBeamMon.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CManagement.h"
#include "CPoolMgr.h"
#include "CBeam.h"
#include "CExplosion.h"
#include "CBeamFlare.h"
#include "CBodyEmit.h"

//-------------------------------------------------------------------------
// Texture , Animation Data
//-------------------------------------------------------------------------
vector<TextureSource> CBeamMon::m_vTextureSource =
{
	 { MS_IDLE,		L"../Bin/Resource/Texture/Monster/BeamMon/Beam_Ready_256.dds" }
	 ,{ CStateComponent::MakeStateID(MS_IDLE, SUB_END),
		L"../Bin/Resource/Texture/Monster/BeamMon/Beam_Found_256.dds" }
	,{ MS_ATTACK_IDLE, L"../Bin/Resource/Texture/Monster/BeamMon/Beam_Idle_256.dds"}
	,{ MS_ATTACK,	L"../Bin/Resource/Texture/Monster/BeamMon/Beam_Attack_256.dds" }
};
//Loop 인 애니메이션은 Ratio 세팅 금지(디폴트로 두기) . Ratio먹이면 다음 애니메이션이 안나옴 
vector<AnimationSource> CBeamMon::m_vAnimSource =
{
	{  MS_IDLE ,3,1,1, true, 0.13f}					
	,{ CStateComponent::MakeStateID(MS_IDLE, SUB_END),3,1,1, false, 0.08f, 1.f}	
	,{ MS_ATTACK_IDLE,2,1,1, true, 0.13f}			
	,{ MS_ATTACK,1,3,2, false, 0.03f}					
};


CBeamMon::CBeamMon(LPDIRECT3DDEVICE9 pGraphicDev)
	:CMonster(pGraphicDev), m_pBodyCollider(nullptr), m_pBeam(nullptr)
	, m_vShootDir({0,0,0}), m_vStartDir({0,0,0}), m_vEndDir({0,0,0}), m_bBeamCollision(false)
{
}

CBeamMon::CBeamMon(const CBeamMon& rhs)
	:CMonster(rhs), m_pBodyCollider(nullptr), m_pBeam(nullptr)
	, m_vShootDir({ 0,0,0 }), m_vStartDir({ 0,0,0 }), m_vEndDir({ 0,0,0 }), m_bBeamCollision(false)
{
}

CBeamMon::~CBeamMon()
{
}

void CBeamMon::CreateStateData()
{
	auto Mgr = CDataMgr<CBeamMon>::GetInstance();
	//클래스 당 한번만 실행되게
	if (Mgr->IsStateEmpty() == false) return;

	//Idle State
	CState<CBeamMon>* State = new CState<CBeamMon>(nullptr, &CBeamMon::Idle, nullptr);
	Mgr->AddState(MS_IDLE, State);

	//Attack Idle State
	State = new CState<CBeamMon>(nullptr, &CBeamMon::Attack_Idle, nullptr);
	Mgr->AddState(MS_ATTACK_IDLE, State);

	//Attack State
	State = new CState<CBeamMon>(nullptr, &CBeamMon::Attack, nullptr);
	Mgr->AddState(MS_ATTACK, State);

	State = new CState<CBeamMon>(nullptr, &CBeamMon::Dead, nullptr);
	Mgr->AddState(MS_DEAD, State);

}

CBeamMon* CBeamMon::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBeamMon* pBeam = new CBeamMon(pGraphicDev);

	if (FAILED(pBeam->Ready_GameObject()))
	{
		Safe_Release(pBeam);
		MSG_BOX("BeamMon Create Failed");
		return nullptr;
	}

	return pBeam;
}


HRESULT CBeamMon::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;

	m_fAttackableDist = 80.f;
	m_pTransformCom->m_vScale = { 8.f, 6.f  ,1.f };
	m_pAnimationCom->Bind_OnChangedFunc([&](_float _aspect) { OnAnimationChange(_aspect); });

	//상태 객체 생성
	CreateStateData();
	ChangeState(MS_IDLE);

	m_pBeam = CBeam::Create(m_pGraphicDev);
	if (!m_pBeam) return E_FAIL;

	//m_pBeam->SetPrevTranslation({ 0,-94.f, 0 });

	m_pBodyCollider = m_pCollisionCom->CreateCollider(this, m_szBodyColliderName);
	if (!m_pBodyCollider) return E_FAIL;
	m_pBodyCollider->Set_Scale(_vec3(4, 4, 4));
	m_pBodyCollider->BindFuncToCollision([&](CollisionInfo info)
		{
			OnBodyCollision(info);
		});

	m_fAttackDamage = 5.f;
	return S_OK;
}

_int CBeamMon::Update_GameObject(const _float& fTimeDelta)
{
	if (IsDead()) return RET_DEAD;

	if (CCharacter::Update_GameObject(fTimeDelta) == RET_DEAD) return RET_DEAD;
	if (m_pStateCom->GetCurrentStateID() == MS_IDLE)
	{
		_vec3 vDist;
		if (FAILED(GetDistVecToPlayer(vDist))) return RET_NONE;
		_float distLen = D3DXVec3Length(&vDist);
		D3DXVec3Normalize(&m_vDir, &vDist);

		if (m_fAttackableDist >= distLen) ChangeState(MS_ATTACK);
	}
	if (m_bShooting)
	{
		m_pBeam->Update_GameObject(fTimeDelta);
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);
	m_fTime += fTimeDelta;

	_vec3 pos;
	m_pTransformCom->Get_Info(INFO_POS, &pos);
	Compute_ViewZ(&pos);

	return RET_NONE;
}

void CBeamMon::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CMonster::LateUpdate_GameObject(fTimeDelta);
	if (m_bShooting)
	{
		m_pBeam->LateUpdate_GameObject(fTimeDelta);
		//TODO : 플레이어에 콜라이더 생성되면 주석 풀기 
		//CollisionBeam();

		bool bBeamEnd = RunBeam(fTimeDelta);
		if (bBeamEnd) m_bShooting = false;
	}
	m_pAnimationCom->Update_State(m_pStateCom->GetCurrentStateID());
}

void CBeamMon::Render_GameObject()
{
	CMonster::Render_GameObject();
	if (m_bShooting) m_pBeam->Render_GameObject();
}

void CBeamMon::ChangeState(_uint nextStateID)
{	
	m_fTime = 0.f;
	m_pAnimationCom->Update_State(nextStateID);
	m_pStateCom->ChangeState<CBeamMon>(nextStateID);

	if (MS_DEAD == nextStateID) Make_DeadText(TAG_NONE, 2);
}

HRESULT CBeamMon::Add_Component()
{
	if (FAILED(CMonster::Add_Component())) return E_FAIL;
	Engine::CComponent* pComponent = nullptr;

	// Animation
	pComponent = m_pAnimationCom = dynamic_cast<Engine::CAnimation*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BeamMonAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });
	return S_OK;
}

void CBeamMon::OnAnimationChange(_float _animAspect)
{
	//공격 애니메이션이 시작될 때 
	if (m_pAnimationCom->Get_State() == MS_ATTACK && m_pAnimationCom->GetSubState() != SUB_END)
	{
		m_pTransformCom->m_vScale.y *= m_fYScaleOffset;
		m_bShooting = true;
		ResetBeam();
	}
	_vec3 scale = m_pTransformCom->Get_Scale();
	scale.x = scale.y * _animAspect;
	m_pTransformCom->Set_Scale(scale.x, scale.y, scale.z);
}

//BeamMon은 한 발이면 사망  
void CBeamMon::OnBodyCollision(CollisionInfo info)
{
	m_pBodyCollider->OffCollision();
	ChangeState(MS_DEAD);
}


void CBeamMon::Idle()
{
}

void CBeamMon::Attack_Idle()
{
	if (m_fTime >= m_fAttackDelayTime)
	{
		ChangeState(MS_ATTACK);
	}
}

void CBeamMon::Attack()
{
	if (m_fTime >= m_fAttackResetTime)
	{
		m_pTransformCom->m_vScale.y /= m_fYScaleOffset;
		ChangeState(MS_ATTACK_IDLE);
		m_bShooting = false;
	}
}


void CBeamMon::Dead()
{
	CExplosion * exp = CPoolMgr::GetInstance()->Get_Object<CExplosion>();
	CBodyEmit* bodyEmit = CPoolMgr::GetInstance()->Get_Object<CBodyEmit>();
	if (exp)
	{
		CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer")->Add_GameObject(exp);
		exp->SetPos(*m_pTransformCom->Get_Info(INFO_POS));
		exp->Reset();
	}

	if (bodyEmit)
	{
		CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer")->Add_GameObject(bodyEmit);
		bodyEmit->SetPos(*m_pTransformCom->Get_Info(INFO_POS));
		bodyEmit->Reset();
	}
	SetDead();
}

//빔이 향하는 방향을 세팅 
//시작 방향 -> 끝 방향으로 Lerp하여 현재 방향을 구함 
void CBeamMon::ResetBeam()
{
	_vec3 pos = *m_pTransformCom->Get_Info(INFO_POS);
	pos.y += m_vBeamPosOffset;
	m_pBeam->SetPos(pos);
	m_vStartDir = { 0,-1,0 };

	_vec3 m_vPlayerPos = *GetPlayerTransform()->Get_Info(INFO_POS);
	m_vPlayerPos.y = pos.y; //현재 플레이어의 위치에서 높이값만 몬스터 높이로 변경
	m_vEndDir = m_vPlayerPos - pos;
	D3DXVec3Normalize(&m_vEndDir, &m_vEndDir);

	m_vShootDir = m_vStartDir;
	m_pBeam->SetShootDir(m_vShootDir);
	m_fTime = 0.f;
	m_bBeamCollision = false;

	if (!m_pBeamFlare)
	{
		m_pBeamFlare = CPoolMgr::GetInstance()->Get_Object<CBeamFlare>();
		pos += m_vEndDir;
		m_pBeamFlare->SetPos({ pos.x, pos.y- 1.f, pos.z});
		m_pBeamFlare->Reset();
		CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer")->Add_GameObject(m_pBeamFlare);
	}
}

bool CBeamMon::RunBeam(const _float& fTimeDelta)
{
	if (!m_pBeam) return true;
	if (m_fTime >= m_fBeamTime)
	{
		m_pBeamFlare->SetDead();
		m_pBeamFlare = nullptr;
		return true;
	}

	D3DXVec3Lerp(&m_vShootDir, &m_vStartDir, &m_vEndDir, m_fTime / m_fBeamTime);
	D3DXVec3Normalize(&m_vShootDir, &m_vShootDir);
	m_pBeam->SetShootDir(m_vShootDir);
	CollisionBeam();

	return false;
}

void CBeamMon::CollisionBeam()
{
	if (m_bBeamCollision) return;
	CCollision* playerCollision = GetPlayerCollision();
	if (!playerCollision) return;

	CCollider * collider = playerCollision->GetCollider();
	if (!collider) return;

	bool bCollision = m_pBeam->CheckCollision(collider);
	if (bCollision)
	{
		m_bBeamCollision = bCollision;
		collider->Collision({ this,_vec3(),m_fAttackDamage });
	}

}

void CBeamMon::Activate()
{
	CMonster::Activate();
	m_pTransformCom->m_vScale = { 8.f, 6.f  ,1.f };
	ChangeState(MS_IDLE);
}

void CBeamMon::Deactivate()
{
	CMonster::Deactivate();
	if (m_pBeamFlare)
	{
		m_pBeamFlare->SetDead();
		m_pBeamFlare = nullptr;
	}
}


void CBeamMon::Free()
{
	if (m_pBeamFlare) m_pBeamFlare->SetDead();
	Safe_Release(m_pBeam);
	CMonster::Free();
}