#include "pch.h"
#include "CSRightHand.h"
#include "CProtoMgr.h"
#include "CRenderer.h"


//-------------------------------------------------------------------------
// Texture , Animation Data
//-------------------------------------------------------------------------
vector<TextureSource> CSRightHand::m_vTextureSource =
{
	 { CStateComponent::MakeStateID(LS_INTRO,SUB_BEGIN),		
		L"../Bin/Resource/Texture/Sniper/Reload_Total1_1024.dds"},
	 { LS_INTRO,		L"../Bin/Resource/Texture/Sniper/Reload_Total2_1024.dds" },
	 { LS_IDLE,		L"../Bin/Resource/Texture/Sniper/GunIdle_1024.dds" },
	 { LS_ATTACK, L"../Bin/Resource/Texture/Sniper/ZoomIn_1024.dds" },
	 { CStateComponent::MakeStateID(LS_ATTACK,SUB_END),
			L"../Bin/Resource/Texture/Sniper/ZoomOut_1024.dds" },
	 { LS_RELOAD,		L"../Bin/Resource/Texture/Sniper/ShootEnd_Total_1024.dds" },

};

vector<AnimationSource> CSRightHand::m_vAnimSource =
{
	{  CStateComponent::MakeStateID(LS_INTRO,SUB_BEGIN) ,3,3,3, false, 0.06f,1.f},
	{  LS_INTRO ,1,2,1, false, 0.07f, 1.f},
	{  LS_IDLE ,0,1,1, true},
	{ LS_ATTACK,1,1,1, false, 0.03f, 1.f,true},
	{  CStateComponent::MakeStateID(LS_ATTACK, SUB_END), 1,1,1, false, 0.03f, 1.f,true},
	{  LS_RELOAD ,2,3,2, false,0.07f, 1.f},
};


CSRightHand::CSRightHand(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev)
{
	m_eOBJ_ID = OBJ_PLAYER;
	m_iID = Make_ID();
}

CSRightHand::~CSRightHand()
{
	m_eOBJ_ID = OBJ_PLAYER;
	m_iID = Make_ID();
}

void CSRightHand::CreateStateData()
{
	auto Mgr = CDataMgr<CSRightHand>::GetInstance();
	if (Mgr->IsStateEmpty() == false) return;

	//Intro
	CState<CSRightHand>* State = new CState<CSRightHand>(&CSRightHand::Intro_Begin, &CSRightHand::Intro, nullptr);
	Mgr->AddState(LS_INTRO, State);

	//Idle
	State = new CState<CSRightHand>(nullptr, &CSRightHand::Idle, nullptr);
	Mgr->AddState(LS_IDLE, State);

	//Attack
	State = new CState<CSRightHand>(&CSRightHand::ZoomIn, &CSRightHand::Attack, &CSRightHand::ZoomOut);
	Mgr->AddState(LS_ATTACK, State);

	//Reload
	State = new CState<CSRightHand>(nullptr, &CSRightHand::Reload, nullptr);
	Mgr->AddState(LS_RELOAD, State);
}

HRESULT CSRightHand::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	m_pTransformCom->Set_Scale(m_vScale);
	m_pTransformCom->Set_Pos(m_vIntoPos);

	CreateStateData();
	ChangeState(LS_NONE);

	return S_OK;
}

_int CSRightHand::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CGameObject::Update_GameObject(fTimeDelta);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);

	m_fTime += fTimeDelta;

	return iExit;
}

void CSRightHand::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pAnimationCom->Update_State(m_pStateCom->GetCurrentStateID());
}

void CSRightHand::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pAnimationCom->Render_Animation();
	m_pBufferCom->Render_Buffer();
}

HRESULT CSRightHand::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;
	//VIBuffer
	pComponent = m_pBufferCom = static_cast<Engine::CRcTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom = static_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	//StateComponent
	pComponent = m_pStateCom = static_cast<Engine::CStateComponent*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_StateComponent"));

	//Owner 지정해주기!! 
	m_pStateCom->SetOnwer(this);

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_StateComponent", pComponent });

	// Animation
	pComponent = m_pAnimationCom = static_cast<Engine::CAnimation*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SniperRightAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });
	return S_OK;
}

void CSRightHand::Change_State(_uint eState)
{
	m_fTime = 0.f;
	m_pAnimationCom->Update_State(eState);
	m_pStateCom->ChangeState<CSRightHand>(eState);
}

bool CSRightHand::IsAnimationEnd()
{
	return m_pAnimationCom->GetSubState() == SUB_NONE && m_pAnimationCom->IsEnd();
}

bool CSRightHand::CanAnimationEnd()
{
	return m_pAnimationCom->GetSubState() == SUB_NONE && m_pAnimationCom->CanEnd();
}

bool CSRightHand::CheckState(LEFT_STATE _state)
{
	if (!m_pAnimationCom) return false;
	return m_pAnimationCom->Get_State() == _state;
}

void CSRightHand::ChangeState(_uint nextStateID)
{
	m_pTransformCom->Set_Pos(m_vPos);
	m_fTime = 0.f;
	m_pAnimationCom->Update_State(nextStateID);
	m_pStateCom->ChangeState<CSRightHand>(nextStateID);
}

void CSRightHand::Intro_Begin()
{
}

void CSRightHand::Intro()
{
	if (m_pAnimationCom->GetSubState() != SUB_BEGIN )
	{
		if (m_pAnimationCom->IsEnd())
		{
			ChangeState(LS_IDLE);
		}
		m_pTransformCom->Set_Pos(m_vPos);
	}
}

void CSRightHand::Idle()
{
	_vec3 pos = m_vPos;
	pos.y = m_vPos.y + sinf(m_fTime*2.f) * m_fHeight;
	m_pTransformCom->Set_Pos(pos);
}


void CSRightHand::ZoomIn()
{
}

void CSRightHand::Attack()
{
}


void CSRightHand::ZoomOut()
{
}


void CSRightHand::Reload()
{
	if (m_pAnimationCom->IsEnd())
		Change_State(LS_IDLE);
}



CSRightHand* CSRightHand::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSRightHand* pRight = new CSRightHand(pGraphicDev);

	if (FAILED(pRight->Ready_GameObject()))
	{
		Safe_Release(pRight);
		MSG_BOX("Snifer Right Hand Create Failed");
		return nullptr;
	}
	return pRight;
}

void CSRightHand::Free()
{
	CGameObject::Free();
}
