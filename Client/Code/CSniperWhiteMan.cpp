#include "pch.h"
#include "CSniperWhiteMan.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CManagement.h"
#include "CBullet.h"
#include "CPoolMgr.h"
#include "CBlood.h"
#include "CExplosion.h"
#include "CSoundMgr.h"
#include "CBeam.h"

wstring CSniperWhiteMan::szWhiteManDead = L"Monster_Dead_SFX.wav";
wstring CSniperWhiteMan::szWhiteManBody = L"Monster_Pistol_Shoot_SFX.wav";
wstring CSniperWhiteMan::szWhiteManHead = L"Monster_HeadShot_SFX.wav";
wstring CSniperWhiteMan::szWhiteManShot = L"Monster_Shot_SFX.wav";

//-------------------------------------------------------------------------
// Texture , Animation Data
//-------------------------------------------------------------------------
vector<TextureSource> CSniperWhiteMan::m_vTextureSource =
{
	 { MS_IDLE,		L"../Bin/Resource/Texture/Monster/WhiteMan/white_Idle_1024.dds" }
	,{ CStateComponent::MakeStateID(MS_ATTACK_IDLE, SUB_BEGIN),
		L"../Bin/Resource/Texture/Monster/WhiteMan/white_Aiming_1024.dds"}
	,{ MS_ATTACK_IDLE, L"../Bin/Resource/Texture/Monster/WhiteMan/white_AttackIdle_1024.dds"}
	,{ MS_DEAD, L"../Bin/Resource/Texture/Monster/WhiteMan/headDead_512.dds" }
};

//Loop 인 애니메이션은 Ratio 세팅 금지(디폴트로 두기) . Ratio먹이면 다음 애니메이션이 안나옴 
vector<AnimationSource> CSniperWhiteMan::m_vAnimSource =
{
	{  MS_IDLE ,1,5,5, true, 0.13f}						//IDLE
	,{ CStateComponent::MakeStateID(MS_ATTACK_IDLE, SUB_BEGIN),1,4,3, false, 0.08f, 1.f}				//Aiming
	,{ MS_ATTACK_IDLE,1,3,2, true, 0.11f}				//Attack_Idle
	,{ MS_DEAD,5,3,1, false, 0.10f, 1.f, true}		//Head Dead
};

CSniperWhiteMan::CSniperWhiteMan(LPDIRECT3DDEVICE9 pGraphicDev)
	:CMonster(pGraphicDev), m_pBodyCollider(nullptr), m_pBeam(nullptr)
{
}

CSniperWhiteMan::CSniperWhiteMan(const CSniperWhiteMan& rhs)
	:CMonster(rhs), m_pBodyCollider(nullptr), m_pBeam(nullptr)
{
}

CSniperWhiteMan::~CSniperWhiteMan()
{
}

void CSniperWhiteMan::CreateStateData()
{
	auto Mgr = CDataMgr<CSniperWhiteMan>::GetInstance();
	if (Mgr->IsStateEmpty() == false) return;

	//IdleState
	CState<CSniperWhiteMan>* State = new CState<CSniperWhiteMan>(nullptr, &CSniperWhiteMan::Idle, nullptr);
	Mgr->AddState(MS_IDLE, State);

	//Attack Idle State
	State = new CState<CSniperWhiteMan>(&CSniperWhiteMan::Begin_Attack, &CSniperWhiteMan::Idle_Attack, nullptr);
	Mgr->AddState(MS_ATTACK_IDLE, State);

	//Dead State
	State = new CState<CSniperWhiteMan>(nullptr, &CSniperWhiteMan::Dead, nullptr);
	Mgr->AddState(MS_DEAD, State);
}

CSniperWhiteMan* CSniperWhiteMan::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSniperWhiteMan* pWhite = new CSniperWhiteMan(pGraphicDev);

	if (FAILED(pWhite->Ready_GameObject()))
	{
		Safe_Release(pWhite);
		MSG_BOX("Sniper White Man Create Failed");
		return nullptr;
	}

	return pWhite;
}


CSniperWhiteMan* CSniperWhiteMan::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
	CSniperWhiteMan* pWhite = new CSniperWhiteMan(pGraphicDev);

	if (FAILED(pWhite->Ready_GameObject()))
	{
		Safe_Release(pWhite);
		MSG_BOX("Sniper White Man Create Failed");
		return nullptr;
	}

	CTransform* pTransform = static_cast<Engine::CTransform*>(
		pWhite->Get_Component(ID_DYNAMIC, L"Com_Transform"));
	pTransform->Set_Pos(vPos);
	pTransform->Update_Component(0.f);

	return pWhite;
}


HRESULT CSniperWhiteMan::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	CreateStateData();
	ChangeState(MS_ATTACK_IDLE);

	m_pAnimationCom->Bind_OnChangedFunc([&](_float _aspect) { OnAnimationChange(_aspect); });
	m_pTransformCom->m_vScale = { 5.f, 13.f  ,1.f };

	//Collider 생성 
	m_pBodyCollider = m_pCollisionCom->CreateCollider(this, m_szBodyColliderName);
	if (!m_pBodyCollider) return E_FAIL;

	m_pCollisionCom->SetMainCollider(m_szBodyColliderName);
	m_pBodyCollider->Set_RelativePos(_vec3(0, 0, 0));
	m_pBodyCollider->Set_Scale(_vec3(4, 21, 4));
	m_pBodyCollider->BindFuncToCollision([&](CollisionInfo info)
		{
			OnBodyCollision(info);
		});

	m_fAttackDamage = 5.f;

	m_vHitPos = { 0,10,0 };


	m_pBeam = CBeam::Create(m_pGraphicDev);
	m_pBeam->SetScale(ROT_X, 0.2f);
	m_pBeam->SetScale(ROT_Y, m_fBeamLen);

	return S_OK;
}

_int CSniperWhiteMan::Update_GameObject(const _float& fTimeDelta)
{
	if (IsDead()) return RET_DEAD;

	int iExit = CMonster::Update_GameObject(fTimeDelta);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA_QUALITY, this);
	//빔 갱신 후 첫 프레임엔 무시함
	if (!m_bFirstFrame) m_pBeam->Update_GameObject(fTimeDelta);

	m_fTime += fTimeDelta;

	_vec3 info;
	m_pTransformCom->Get_Info(INFO_POS, &info);
	Compute_ViewZ(&info);

	if (m_bTrace || m_bTargeting || m_bShoot)
	{
		if (m_bShoot)
		{
			if (m_fTime > m_fLerpTime)
			{
				Shoot();
				m_bShoot = false;
			}
		}
		else if (m_bTargeting)
		{
			_vec3 vBeamPos{};
			D3DXVec3Lerp(&vBeamPos, &m_vBeamDestPos, &m_vPlayerPos, m_fTime / m_fLerpTime);
			m_vBeamDir = vBeamPos - m_vMyPos;
			D3DXVec3Normalize(&m_vBeamDir, &m_vBeamDir);

			if (m_fTime > m_fLerpTime)
			{
				m_bTargeting = false;
				m_bShoot = true;
				m_fTime = 0.f;
			}
		}
		else
		{
			UpdateBeam(fTimeDelta);
		}
		m_pBeam->SetShootDir(m_vBeamDir);
		if(m_bFirstFrame) m_bFirstFrame = false;

	}

	return iExit;
}

void CSniperWhiteMan::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CMonster::LateUpdate_GameObject(fTimeDelta);
	m_pAnimationCom->Update_State(m_pStateCom->GetCurrentStateID());
	if (m_bTrace || m_bTargeting || m_bShoot) m_pBeam->LateUpdate_GameObject(fTimeDelta);
}

void CSniperWhiteMan::Render_GameObject()
{
	CMonster::Render_GameObject();
}

void CSniperWhiteMan::ChangeState(_uint nextStateID)
{	
	m_fTime = 0.f;
	m_pAnimationCom->Update_State(nextStateID);
	m_pStateCom->ChangeState<CSniperWhiteMan>(nextStateID);
}

HRESULT CSniperWhiteMan::Add_Component()
{
	if (FAILED(CMonster::Add_Component())) return E_FAIL;
	Engine::CComponent* pComponent = nullptr;

	// Animation
	pComponent = m_pAnimationCom = static_cast<Engine::CAnimation*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SniperManAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });
	return S_OK;
}

void CSniperWhiteMan::UpdateBeam(const _float& fTimeDelta)
{ 
	if(!m_bAngleReverse) m_fRotSpeed += fTimeDelta;
	else m_fRotSpeed -= fTimeDelta;

	if (m_fBeamAngle >= m_fEndAngle) m_bAngleReverse = true;

	
	m_fBeamAngle += m_fRotSpeed * fTimeDelta;

	_float offsetX = m_fBeamDist * cosf(m_fBeamAngle);
	_float offsetY = m_fBeamDist * sinf(m_fBeamAngle);

	_vec3 vTargetPos = m_vPlayerPos;
	vTargetPos.x += offsetX;
	vTargetPos.y += offsetY;
	vTargetPos.z = m_vPlayerPos.z;

	m_vBeamDir = vTargetPos - m_vMyPos ;
	D3DXVec3Normalize(&m_vBeamDir, &m_vBeamDir);

	if (m_fBeamAngle <= m_fStartAngle)
	{
		m_vBeamDestPos = vTargetPos;
		m_fTime = 0.f;
		m_bTrace = false;
		m_bTargeting = true;
	}

}


void CSniperWhiteMan::OnBodyCollision(CollisionInfo info)
{
	CBlood* blood = nullptr;

	_vec3 pos = *m_pTransformCom->Get_Info(INFO_POS);
	pos += m_vHitPos;
	m_fHP = 0.f;

	if (m_pBodyCollider) m_pBodyCollider->OffCollision();

	blood = CPoolMgr::GetInstance()->Get_Object<CBlood>();
	blood->ChangeState(1);
	blood->SetPos(pos);
	blood->Reset();
	blood = CPoolMgr::GetInstance()->Get_Object<CBlood>();
	CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer")->Add_GameObject(blood);

	ChangeState(MS_DEAD);

	CSoundMgr::GetInstance()->PlayMonsterSound(szWhiteManDead.c_str(), 0.3f);
	CSoundMgr::GetInstance()->PlayMonsterSound(szWhiteManBody.c_str(), 1.f);
}

void CSniperWhiteMan::Idle()
{
}

void CSniperWhiteMan::Begin_Attack()
{
}

void CSniperWhiteMan::Idle_Attack()
{
	if (m_pAnimationCom->CanEnd() && m_pAnimationCom->GetSubState() == SUB_BEGIN)
	{
		m_bTrace = true;

		m_vMyPos = *m_pTransformCom->Get_Info(INFO_POS);
		
		m_vPlayerPos = *GetPlayerTransform()->Get_Info(INFO_POS);
		m_vPlayerPos.y -= GetPlayerTransform()->Get_Scale().y *0.5f;

		CTransform* pBeamTransform = static_cast<CTransform*>(m_pBeam->Get_Component(ID_DYNAMIC, L"Com_Transform"));
		m_vPlayerPos.x += pBeamTransform->Get_Scale().x * 0.5f;
		m_pBeam->SetPos(m_vMyPos + m_vHandPos);

		m_fStartAngle = D3DXToRadian((rand() % 180));
		m_fEndAngle = m_fStartAngle + D3DX_PI * 0.5f;
		m_fBeamAngle = m_fStartAngle;
		m_bFirstFrame = true;
	}
}

void CSniperWhiteMan::Shoot()
{
	if (GetPlayerCollision())
		GetPlayerCollision()->GetCollider()->Collision({ this,{0,0,0},m_fAttackDamage });
	SetDead();
}

void CSniperWhiteMan::Dead()
{
	if (m_pAnimationCom->IsEnd())
	{
		SetDead();
	}
}


void CSniperWhiteMan::OnAnimationChange(_float _animAspect)
{
	_vec3 scale = m_pTransformCom->Get_Scale();
	scale.x = scale.y * _animAspect;
	m_pTransformCom->Set_Scale(scale.x, scale.y, scale.z);
}

void CSniperWhiteMan::Activate()
{
	CMonster::Activate();
	ChangeState(MS_IDLE);
}

void CSniperWhiteMan::Free()
{
	Safe_Release(m_pBeam);
	CMonster::Free();
}