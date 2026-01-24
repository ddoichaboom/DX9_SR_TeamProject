#include "pch.h"
#include "CBoss.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CManagement.h"
#include "CPoolMgr.h"
#include "CBossBullet.h"
#include "CBeam.h"
#include "CRocket.h"
#include "CBossTrail.h"
#include "CExplosion.h"
#include "CEventMgr.h"

//-------------------------------------------------------------------------
// Texture , Animation Data
//-------------------------------------------------------------------------
vector<TextureSource> CBoss::m_vTextureSource =
{
	{ MS_IDLE,	L"../Bin/Resource/Texture/BOSS/Boss_Idle_256.dds" }
	,{ MS_WALK,	L"../Bin/Resource/Texture/BOSS/Boss_Idle_256.dds" }
	,{ CStateComponent::MakeStateID(MS_ATTACK, SUB_BEGIN),
		L"../Bin/Resource/Texture/BOSS/Boss_Shoot_Idle_256.dds"}
	,{ MS_ATTACK, L"../Bin/Resource/Texture/BOSS/Boss_Shoot_256.dds"}

	,{ CStateComponent::MakeStateID(MS_ATTACK2, SUB_BEGIN),
		L"../Bin/Resource/Texture/BOSS/Boss_Rocket_Idle_256.dds"}
	,{ MS_ATTACK2,	L"../Bin/Resource/Texture/BOSS/Boss_Rocket_Attack_256.dds" }
	
	,{ MS_ATTACK3,	L"../Bin/Resource/Texture/BOSS/Boss_Idle_256.dds" }

	,{ MS_GUARD, L"../Bin/Resource/Texture/BOSS/Boss_Shield_Begin_256.dds" }
};

vector<AnimationSource> CBoss::m_vAnimSource =
{
	{  MS_IDLE ,1,2,2, true, 0.12f}					//IDLE
	,{  MS_WALK ,1,2,2, true, 0.12f}				//DASH
	, { CStateComponent::MakeStateID(MS_ATTACK, SUB_BEGIN),1,2,2, false, 0.03f, 1.f, true }
	,{ MS_ATTACK,1,2,2, true, 0.09f}	//Bullet Shoot

	,{ CStateComponent::MakeStateID(MS_ATTACK2, SUB_BEGIN),2,3,1, false, 0.04f, 1.f, true}
	,{ MS_ATTACK2,1,2,2, true, 0.08f} // Rocket 
	,{ MS_ATTACK3,1,2,2, true, 0.07f} // Beam 
	,{ MS_GUARD,2,2,2, false, 0.11f, 1.f, true}
	
};

_vec2	CBoss::m_vRandomRange = { 0.f, 10.f };

CBoss::CBoss(LPDIRECT3DDEVICE9 pGraphicDev)
	:CMonster(pGraphicDev), m_pBodyCollider(nullptr), m_fMapRadius(0.f), m_fDirOffset(1.f)
	, m_vScale({ 50.f,50.f,1.f }), gen(rd())
	, dis((_int)m_vRandomRange.x, (_int)m_vRandomRange.y)
	, floatDis(m_vAngleABSRange.x, m_vAngleABSRange.y)
{
	fill(m_pBeam, m_pBeam + MON_END_HAND, nullptr);
	ZeroMemory(m_vBeamStartPos, sizeof(_vec3) * MON_END_HAND);
	ZeroMemory(m_vBeamEndPos, sizeof(_vec3) * MON_END_HAND);
	ZeroMemory(m_vHandPos, sizeof(_vec3) * MON_END_HAND);
}

CBoss::CBoss(const CBoss& rhs)
	:CMonster(rhs), m_pBodyCollider(nullptr), m_fMapRadius(0.f), m_fDirOffset(1.f)
	, m_vScale({ 50.f,50.f,1.f }), gen(rd())
	, dis((_int)m_vRandomRange.x, (_int)m_vRandomRange.y)
	, floatDis(m_vAngleABSRange.x, m_vAngleABSRange.y)
{
	fill(m_pBeam, m_pBeam + MON_END_HAND, nullptr);
	ZeroMemory(m_vBeamStartPos, sizeof(_vec3) * MON_END_HAND);
	ZeroMemory(m_vBeamEndPos, sizeof(_vec3) * MON_END_HAND);
	ZeroMemory(m_vHandPos, sizeof(_vec3) * MON_END_HAND);
}

CBoss::~CBoss()
{
}

void CBoss::CreateStateData()
{
	auto Mgr = CDataMgr<CBoss>::GetInstance();
	if (Mgr->IsStateEmpty() == false) return;

	//IdleState
	CState<CBoss>* State = new CState<CBoss>(&CBoss::Idle_Begin, &CBoss::Idle, nullptr);
	Mgr->AddState(MS_IDLE, State);

	//Walk == Dash
	State = new CState<CBoss>(&CBoss::Dash_Begin, &CBoss::Dash, nullptr);
	Mgr->AddState(MS_WALK, State);

	//Attack_Idle
	State = new CState<CBoss>(nullptr, &CBoss::Attack_Idle, nullptr);
	Mgr->AddState(MS_ATTACK_IDLE, State);

	//Attack Bullet
	State = new CState<CBoss>(nullptr, &CBoss::Attack_Bullet, nullptr);
	Mgr->AddState(MS_ATTACK, State);

	//Attack_Rocket
	State = new CState<CBoss>(&CBoss::Attack_Rocket_Begin, &CBoss::Attack_Rocket, nullptr);
	Mgr->AddState(MS_ATTACK2, State);

	//Attack_Beam
	State = new CState<CBoss>(&CBoss::Reset_Beam, &CBoss::Attack_Beam, nullptr);
	Mgr->AddState(MS_ATTACK3, State);

	//Guard
	State = new CState<CBoss>(nullptr, &CBoss::Guard, nullptr);
	Mgr->AddState(MS_GUARD, State);
	
	//Dead
	State = new CState<CBoss>(nullptr, &CBoss::Dead, nullptr);
	Mgr->AddState(MS_DEAD, State);
}

HRESULT CBoss::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;

	m_pBeam[MON_LEFT_HAND] = CBeam::Create(m_pGraphicDev);
	if (!m_pBeam[MON_LEFT_HAND]) return E_FAIL;
	m_pBeam[MON_LEFT_HAND]->SetScale(ROT_X, 0.5f);
	//m_pBeam[MON_LEFT_HAND]->SetPrevTranslation({ 0,0,0 });

	m_pBeam[MON_RIGHT_HAND] = CBeam::Create(m_pGraphicDev);
	if (!m_pBeam[MON_RIGHT_HAND]) return E_FAIL;
	m_pBeam[MON_RIGHT_HAND]->SetScale(ROT_X, 0.5f);
	//m_pBeam[MON_RIGHT_HAND]->SetPrevTranslation({ 0,0,0 });

	//손 소켓 위치 
	m_vHandPos[MON_LEFT_HAND] = { -0.53f, 0.2f,0.f };
	m_vHandPos[MON_RIGHT_HAND] = { 0.57f, 0.2f,0.f };

	//로켓 손 위치
	m_vRocektPos[MON_LEFT_HAND] = { -0.95f, 0.6f,0.f };
	m_vRocektPos[MON_RIGHT_HAND] = { 1.f, 0.6f,0.f };

	CreateStateData();
	ChangeState(MS_IDLE);

	m_pAnimationCom->Bind_OnChangedFunc([&](_float _aspect) { OnAnimationChange(_aspect); });

	m_pBodyCollider = m_pCollisionCom->CreateCollider(this, m_szBodyColliderName);
	if (!m_pBodyCollider) return E_FAIL;

	m_pBodyCollider->Set_RotToPrt();
	m_pBodyCollider->Set_Scale(_vec3(35,35,1.f));
	m_pBodyCollider->BindFuncToCollision([&](CollisionInfo info)
		{
			OnBodyCollision(info);
		});

	//맵 반지름 값 
	//m_fMapRadius = 400.f;
	m_fMapRadius = 500.f;
	m_pTransformCom->m_vScale = m_vScale;
	//m_pTransformCom->Set_Pos({ 0.f, 100.f ,m_fMapRadius});			// CMapLoader에서 스포너 발견시 생성
	
	GetHandWorldPos(MON_LEFT_HAND);
	GetHandWorldPos(MON_RIGHT_HAND);

	//Effect
	m_pBossTrail = CBossTrail::Create(m_pGraphicDev);
	m_pBossTrail->SetOwnerTransform(m_pTransformCom);

	m_fAttackDamage = 5.f;
	//m_fSpeed = m_fBaseSpeed;
	//m_fHP = 100.f;
	m_fHP = 20.f;
	return S_OK;
}

CBoss* CBoss::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBoss* pBoss = new CBoss(pGraphicDev);

	if (FAILED(pBoss->Ready_GameObject()))
	{
		Safe_Release(pBoss);
		MSG_BOX("BOSS Create Failed");
		return nullptr;
	}

	return pBoss;
}

CBoss* CBoss::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
	CBoss* pBoss = new CBoss(pGraphicDev);

	if (FAILED(pBoss->Ready_GameObject()))
	{
		Safe_Release(pBoss);
		MSG_BOX("BOSS Create Failed");
		return nullptr;
	}

	pBoss->SetPos(vPos);

	return pBoss;
}

_int CBoss::Update_GameObject(const _float& fTimeDelta)
{
	if (IsDead()) return RET_DEAD;
	int iExit = CCharacter::Update_GameObject(fTimeDelta);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA_QUALITY, this);

	Move(fTimeDelta, m_fDirAngle, m_fStateRatio);

	if (m_pStateCom->GetCurrentStateID() == MS_WALK)
	{
		m_pBossTrail->Update_GameObject(fTimeDelta);
	}
	else if (m_pStateCom->GetCurrentStateID() == MS_ATTACK3)
	{
		m_pBeam[MON_LEFT_HAND]->Update_GameObject(fTimeDelta);
		m_pBeam[MON_RIGHT_HAND]->Update_GameObject(fTimeDelta);
	}
	m_fTime += fTimeDelta;
	m_fSubTime += fTimeDelta;

	_vec3 info;
	m_pTransformCom->Get_Info(INFO_POS, &info);
	Compute_ViewZ(&info);

	return iExit;
}

void CBoss::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CMonster::LateUpdate_GameObject(fTimeDelta);
	////현재 상태에 맞는 애니메이션으로 자동 전환
	m_pAnimationCom->Update_State(m_pStateCom->GetCurrentStateID());
	
	if (m_pStateCom->GetCurrentStateID() == MS_ATTACK2)
	{
		GetRocketWorldPos(MON_LEFT_HAND);
		GetRocketWorldPos(MON_RIGHT_HAND);
	}
	else if (m_pStateCom->GetCurrentStateID() == MS_ATTACK3)
	{
		m_pBeam[MON_LEFT_HAND]->LateUpdate_GameObject(fTimeDelta);
		m_pBeam[MON_RIGHT_HAND]->LateUpdate_GameObject(fTimeDelta);
		GetHandWorldPos(MON_LEFT_HAND);
		GetHandWorldPos(MON_RIGHT_HAND);
	}

}

void CBoss::Render_GameObject()
{
	CMonster::Render_GameObject();
	if (m_pStateCom->GetCurrentStateID() == MS_ATTACK3)
	{
		m_pBeam[MON_LEFT_HAND]->Render_GameObject();
		m_pBeam[MON_RIGHT_HAND]->Render_GameObject();
	}

}

void CBoss::ChangeState(_uint nextStateID)
{
	m_fTime = 0.f;
	m_fSubTime = 0.f;
	m_pAnimationCom->Update_State(nextStateID);
	m_pStateCom->ChangeState<CBoss>(nextStateID);
}

HRESULT CBoss::Add_Component()
{
	//if (FAILED(CMonster::Add_Component())) return E_FAIL;
	Engine::CComponent* pComponent = nullptr;

	//VIBuffer
	pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Transform", pComponent });

	//Collision
	pComponent = m_pCollisionCom = dynamic_cast<Engine::CCollision*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collision"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collision", pComponent });

	//StateComponent
	pComponent = m_pStateCom = dynamic_cast<Engine::CStateComponent*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_StateComponent"));

	//Owner 지정해주기!! 
	m_pStateCom->SetOnwer(this);

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_StateComponent", pComponent });


	//CComponent* pComponent = nullptr;

	// Animation
	pComponent = m_pAnimationCom = dynamic_cast<CAnimation*>
		(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BossAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });
	return S_OK;
}

void CBoss::Collision_Beam()
{
	if (m_bBeamCollision) return;

	CCollision* playerCollision = GetPlayerCollision();
	if (!playerCollision) return;

	CCollider* collider = playerCollision->GetCollider();
	if (!collider) return;

	for (int i = 0; i < MON_END_HAND; i++)
	{
		bool bCollision = m_pBeam[i]->CheckCollision(collider);
		if (bCollision)
		{
			m_bBeamCollision = bCollision;
			collider->Collision({ this,_vec3(),m_fAttackDamage });
		}
		return;
	}
}



void CBoss::OnBodyCollision(CollisionInfo info)
{
	m_fHP -= info.fDamage;
	CExplosion* exp = CPoolMgr::GetInstance()->Get_Object<CExplosion>();
	if (exp)
	{
		exp->SetPos(*m_pTransformCom->Get_Info(INFO_POS));
		exp->Reset();
		CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer")->Add_GameObject(exp);
	}
	if (m_fHP <= 0.f)
	{
		if (m_pBodyCollider) m_pBodyCollider->OffCollision();
		ChangeState(MS_DEAD);
		CEventMgr::GetInstance()->Broadcast(EVENT_ENDING, nullptr);
	}
}

void CBoss::OnAnimationChange(_float _animAspect)
{
	_float angle = floatDis(gen);
	SetAngle(angle);
}

void CBoss::Move(const _float& fTimeDelta, _float& _dirAngle, _float ratio)
{
	_vec3 dir;
	_vec3 pos = { 0,0,0 };
	_vec3 vUp = { 0,1,0 };
	//맵 원점 바라보는 방향. 원점 - pos 
	_vec3 vLook = (*m_pTransformCom->Get_Info(INFO_POS)) * -1.f; 
	_vec3 vRight;
	D3DXVec3Cross(&vRight, &vLook, &vUp);
	//맵을 바라봤을 때의 Right 벡터 
	D3DXVec3Normalize(&vRight, &vRight);
	dir = vRight;
	
	//각도 조절
	_matrix mat;
	//맵 원점을 바라보지않고 전방을 바라보게 다시 외적
	D3DXVec3Cross(&vLook, &vUp, &vRight);
	D3DXVec3Normalize(&vLook, &vLook);

	//vLook을 기준으로 Right를 회전 = 전방 기준 회전 
	D3DXMatrixRotationAxis(&mat, &vLook, D3DXToRadian(_dirAngle));
	D3DXVec3TransformNormal(&dir, &vRight, &mat);


	pos = *m_pTransformCom->Get_Info(INFO_POS);
	_float value = (ratio>= 1.f? 1.f : easeOutQuint(ratio));


	pos += dir * fTimeDelta * m_fSpeed * value * m_fDirOffset;

	if (pos.y >= m_vHeightRange.y) pos.y = m_vHeightRange.y;
	else if (pos.y <= m_vHeightRange.x) pos.y = m_vHeightRange.x;
	//원점에서 MapRadius 길이만큼 떨어진 위치로 보정 
	_vec3 originDir;
	D3DXVec3Normalize(&originDir, &pos);
	pos = originDir * m_fMapRadius;

	m_pTransformCom->Set_Pos(pos);
}


void CBoss::Idle_Begin()
{
	m_fStateRatio = 0.f;
	m_fSpeed = m_fIdleSpeed;
}

void CBoss::Idle()
{
	m_fStateRatio = m_fTime / m_fIdle_Time;
	if (m_fTime >= m_fIdle_Time)
	{
		m_fSpeed = m_fBaseSpeed;
		int CanDash = rand() % 3;
		if (CanDash >= 1) ChangeState(MS_WALK); // Dash
		else ChangeState(MS_ATTACK_IDLE);
		m_fStateRatio = 1.f;
	}
}

void CBoss::Dash_Begin()
{
	m_fStateRatio = 0.f;
	m_fSpeed = m_fDashSpeed;
}

void CBoss::Dash()
{
	m_fStateRatio = m_fTime / m_fDash_Time;
	if (m_fTime >= m_fDash_Time)
	{
		m_fSpeed = m_fBaseSpeed;
		ChangeState(MS_ATTACK_IDLE);
		m_fStateRatio = 1.f;
		m_pBossTrail->Reset();
	}
}

void CBoss::Attack_Idle()
{
	_int nextAttack = rand() % 3;
	switch (nextAttack)
	{
	case 0:
		ChangeState(MS_ATTACK);
		break;
	case 1 :
		ChangeState(MS_ATTACK2);
		break;
	case 2:
		ChangeState(MS_ATTACK3);
		break;
	default:
		break;
	}
}

void CBoss::Attack_Bullet()
{
	if (m_fSubTime >= m_fShoot_time)
	{
		m_fSubTime = 0.f;
		CBossBullet* bullet = CPoolMgr::GetInstance()->Get_Object<CBossBullet>();
		if (bullet)
		{
			CLayer* layer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");
			CTransform* playerTransform = GetPlayerTransform();
			if (layer && playerTransform)
			{
				_vec3 vPos = GetHandWorldPos(MON_RIGHT_HAND);
				_vec3 vDest= *playerTransform->Get_Info(INFO_POS);

				vDest.x += (dis(gen) - m_vRandomRange.y * 0.5f);
				vDest.z += (dis(gen) - m_vRandomRange.y * 0.5f);
				vDest.y += (dis(gen) - m_vRandomRange.y * 0.5f);

				_vec3 vLook = vDest - vPos;

				D3DXVec3Normalize(&vLook, &vLook);
				bullet->SetDirection(vLook);
				bullet->SetPos(vPos);
				layer->Add_GameObject(bullet);
			}
			else bullet->ReturnToPool();
		}
	}

	if (m_fTime >= m_fAttack_Bullet_Time)
	{
		ChangeState(MS_IDLE);
	}
}

void CBoss::Attack_Beam()
{
	Run_Beam(m_fTime / m_fAttack_Beam_Time);
	if (m_fTime >= m_fAttack_Beam_Time)
	{
		ChangeState(MS_IDLE);
	}
}

//TODO : 타일에서 시작 지점, 종료 지점을 가져와서 변경하기 
void CBoss::Reset_Beam()
{
	for (int i = 0; i < MON_END_HAND; i++)
	{
		_vec3 pos = m_vWorldHandPos[i];
		m_pBeam[i]->SetPos(pos);
		m_vBeamStartPos[i] = pos;
		m_vBeamStartPos[i].y = 0.f;

		_vec3 playerPos = *GetPlayerTransform()->Get_Info(INFO_POS);
		playerPos.y = 0.f;

		_vec3 dir = playerPos - m_vBeamStartPos[i];
		D3DXVec3Normalize(&dir, &dir);
		m_vBeamEndPos[i] = m_vBeamStartPos[i] + dir * m_fMapRadius * 2.f;
		dir = { 0,-1,0};
		m_pBeam[i]->SetShootDir(dir);
	}
	m_fTime = 0.f;
	m_bBeamCollision = false;
}

void CBoss::Run_Beam(_float _ratio)
{
	for (int i = 0; i < MON_END_HAND; i++)
	{
		_vec3 pos = m_vWorldHandPos[i];
		m_pBeam[i]->SetPos(pos);
		
		_vec3 destPos, shootDir;
		D3DXVec3Lerp(&destPos, &m_vBeamStartPos[i], &m_vBeamEndPos[i], _ratio);
		shootDir = destPos - pos;
		D3DXVec3Normalize(&shootDir, &shootDir);
		m_pBeam[i]->SetShootDir(shootDir);
	}
	Collision_Beam();
}

_vec3 CBoss::GetHandWorldPos(MON_HAND _eHand)
{
	D3DXVec3TransformCoord(&m_vWorldHandPos[_eHand], &m_vHandPos[_eHand], m_pTransformCom->Get_World());
	return m_vWorldHandPos[_eHand];
}

_vec3 CBoss::GetRocketWorldPos(MON_HAND _eHand)
{
	D3DXVec3TransformCoord(&m_vWorldRocketPos[_eHand], &m_vRocektPos[_eHand], m_pTransformCom->Get_World());
	return m_vWorldRocketPos[_eHand];
}


void CBoss::Attack_Rocket_Begin()
{
	ReverseDir();
}

void CBoss::Attack_Rocket()
{
	if (m_fSubTime >= m_fRocketShoot_time)
	{
		m_fSubTime = 0.f;
		_vec3 vRight{}, vUp{}, vPos{};
		_float randX{}, randY{};

		memcpy(&vRight, &m_pTransformCom->Get_World()->m[INFO_RIGHT], sizeof(_vec3));
		memcpy(&vUp, &m_pTransformCom->Get_World()->m[INFO_UP], sizeof(_vec3));
		memcpy(&vPos, &m_pTransformCom->Get_World()->m[INFO_POS], sizeof(_vec3));
		D3DXVec3Normalize(&vRight, &vRight);
		D3DXVec3Normalize(&vUp, &vUp);
		for (int i = 0; i < 3; i++)
		{
			CRocket* pRocket = CPoolMgr::GetInstance()->Get_Object<CRocket>();
			if (pRocket)
			{
				CLayer* layer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");
				CTransform* playerTransform = GetPlayerTransform();
				if (layer && playerTransform)
				{
					randX = GetRandomFloat(-100.f, 100.f);
					randY = GetRandomFloat(-30.f, 80.f);
					
					_vec3 vRocketPos = vPos + vRight * randX + vUp * randY;
					_vec3 vDest = *playerTransform->Get_Info(INFO_POS);

					vDest.x += (dis(gen) - m_vRandomRange.x * 0.5f);
					vDest.y += (dis(gen) - m_vRandomRange.y * 0.5f);

					_vec3 vLook = vDest - vPos;

					D3DXVec3Normalize(&vLook, &vLook);
					pRocket->SetDirection(vLook);
					pRocket->SetPos(vRocketPos);
					layer->Add_GameObject(pRocket);
				}
				else
				{
					pRocket->ReturnToPool();
					break;
				}
			}
		}
	}

	if (m_fTime >= m_fAttack_Rocket_Time)
	{
		ChangeState(MS_IDLE);
	}
}

void CBoss::Guard()
{
}

void CBoss::Dead()
{
	if (m_pAnimationCom->IsEnd())
	{
		SetDead();
	}
}

void CBoss::Activate()
{
	CMonster::Activate();
	m_pBossTrail->Reset();
	ChangeState(MS_IDLE);
}

void CBoss::Free()
{
	Safe_Release(m_pBossTrail);
	Safe_Release(m_pBeam[MON_LEFT_HAND]);
	Safe_Release(m_pBeam[MON_RIGHT_HAND]);
	CMonster::Free();
}