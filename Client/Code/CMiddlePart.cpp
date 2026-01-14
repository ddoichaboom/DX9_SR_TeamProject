#include "pch.h"
#include "CMiddlePart.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"

#include "CPlayer.h"

vector<TextureSource> CMiddlePart::m_vTextureSource =
{
	{ IDLE, L"../Bin/Resource/Texture/Player/Middle_Kick.dds"},
	{ KICK, L"../Bin/Resource/Texture/Player/Middle_Kick.dds"},
	{ DRINK,  L"../Bin/Resource/Texture/Player/Middle_Soda.dds" },
	{ SLIDE,  L"../Bin/Resource/Texture/Player/Middle_Slide.dds" },
	{ GetStateID(INTRO,SW_PISTOL),  L"../Bin/Resource/Texture/Player/Player_Intro_Begin.dds"},

	{ INTRO,  L"../Bin/Resource/Texture/Player/Player_Intro.dds" },
};

vector<AnimationSource>  CMiddlePart::m_vAnimSource =
{
	{ IDLE,0,3,3, true, 0.08f},
	{ KICK,0,3,3, false, 0.08f, 1.f},
	{ DRINK,0,6,6, false, 0.08f, 1.f},
	{ SLIDE,1,0,0, true, 0.08f},
	{ GetStateID(INTRO,SW_PISTOL),1,2,2, false, 0.12f, 1.f},
	{ INTRO,1,2,2, false, 0.12f, 1.f}
};

CMiddlePart::CMiddlePart(LPDIRECT3DDEVICE9 pGraphicDev)
	: CPlayerPart(pGraphicDev), m_iLoopTime(3)
{
}

CMiddlePart::CMiddlePart(const CMiddlePart& rhs)
	: CPlayerPart(rhs), m_iLoopTime(3)
{
}

CMiddlePart::~CMiddlePart()
{
}

void CMiddlePart::CreateStateData()
{
	auto Mgr = CDataMgr<CMiddlePart>::GetInstance();
	if (Mgr->IsStateEmpty() == false) return;

	CState<CMiddlePart>* State = new CState<CMiddlePart>(&CMiddlePart::Begin_Drink, &CMiddlePart::Drink, &CMiddlePart::End_Drink);
	Mgr->AddState(DRINK, State);

	State = new CState<CMiddlePart>(&CMiddlePart::Begin_Kick, &CMiddlePart::Kick, &CMiddlePart::End_Kick);
	Mgr->AddState(KICK, State);

	State = new CState<CMiddlePart>(&CMiddlePart::Begin_Idle, &CMiddlePart::Idle, nullptr);
	Mgr->AddState(IDLE, State);

	State = new CState<CMiddlePart>(&CMiddlePart::Begin_Slide, &CMiddlePart::Slide, &CMiddlePart::End_Slide);
	Mgr->AddState(SLIDE, State);

	State = new CState<CMiddlePart>(&CMiddlePart::Begin_Intro, &CMiddlePart::Intro, nullptr);
	Mgr->AddState(GetStateID(INTRO, SW_PISTOL), State);

	State = new CState<CMiddlePart>(&CMiddlePart::Begin_Intro2, &CMiddlePart::Intro2, nullptr);
	Mgr->AddState(INTRO, State);

}

CMiddlePart* CMiddlePart::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMiddlePart* pMiddlePart = new CMiddlePart(pGraphicDev);

	if (FAILED(pMiddlePart->Ready_GameObject()))
	{
		Safe_Release(pMiddlePart);
		MSG_BOX("MiddlePart Part Create Failed");
		return nullptr;
	}

	return pMiddlePart;
}

_bool CMiddlePart::Get_ActionAble()
{
	return true;
}

HRESULT CMiddlePart::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CreateStateData();
	m_pTransformCom->m_vScale = { 256.f, 256.f, 1.f };
	m_vStartPos = { 0.f, WINCY * -0.5f + 200.f, 0.f };
	m_pTransformCom->Set_Pos(m_vStartPos);

	m_bRendering = false;

	return S_OK;
}

_int CMiddlePart::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CPlayerPart::Update_GameObject(fTimeDelta);	

	return iExit;
}

void CMiddlePart::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CPlayerPart::LateUpdate_GameObject(fTimeDelta);
	m_pAnimationCom->Update_State(m_pStateCom->GetCurrentStateID());
}

void CMiddlePart::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pAnimationCom->Render_Animation();
	m_pBufferCom->Render_Buffer();
}

HRESULT CMiddlePart::Add_Component()
{
	if (FAILED(CPlayerPart::Add_Component()))
		return E_FAIL;

	Engine::CComponent* pComponent = nullptr;

	pComponent = m_pAnimationCom = dynamic_cast<Engine::CAnimation*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_MiddleAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });

	return S_OK;
}

void CMiddlePart::Free()
{
	CPlayerPart::Free();
}

void CMiddlePart::ChangeState(_uint nextStateID)
{
	//m_fTime은 맨 위로 고정! ChangeState에서 실행되는 함수(Begin,End)에서 fTime을 바꿀수도있음
	m_fTime = 0.f;
	//현재 상태에 맞는 애니메이션으로 자동 전환
	m_pAnimationCom->Update_State(nextStateID);
	m_pStateCom->ChangeState<CMiddlePart>(nextStateID);
}

void CMiddlePart::Begin_Idle()
{
	m_pTransformCom->m_vScale = { 256.f, 256.f, 1.f };
	m_pTransformCom->Set_Pos(m_vStartPos);
}

void CMiddlePart::Idle()
{

}

void CMiddlePart::Begin_Kick()
{
	m_pTransformCom->m_vScale = { 256.f, 256.f, 1.f };
	m_pTransformCom->Set_Pos(m_vStartPos);
}

void CMiddlePart::Kick()
{
	if (m_pAnimationCom->CanEnd())
	{
		m_pPlayer->Kick_Func();
		//ChangeState(IDLE);
		m_pPlayer->Change_State(IDLE);
		return;
	}
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
}

void CMiddlePart::End_Kick()
{

}

void CMiddlePart::Begin_Drink()
{
	// 체력 회복 
	m_pTransformCom->m_vScale = { 350.f, 350.f, 1.f };	
	m_pTransformCom->Set_Pos({ 0.f, WINCY * -0.5f + 100.f, 0.f });

}

void CMiddlePart::Drink()
{

	if (m_pAnimationCom->CanEnd())
	{
		//ChangeState(IDLE);
		m_pPlayer->Change_State(IDLE);
		return;
	}
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
}

void CMiddlePart::End_Drink()
{
	m_pTransformCom->Set_Pos(m_vStartPos);
}

void CMiddlePart::Begin_Slide()
{
	m_pTransformCom->m_vScale = { 512.f, 256.f, 1.f };
	m_pTransformCom->Set_Pos({ 0.f, WINCY * -0.5f + 225.f, 0.f });
}

void CMiddlePart::Slide()
{
	m_pPlayer->Slide_Func();
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
}

void CMiddlePart::End_Slide()
{

}

void CMiddlePart::Begin_Intro()
{
	m_pTransformCom->m_vScale = { 512.f, 512.f, 1.f };
	m_pTransformCom->Set_Pos({ 0.f, WINCY * -0.5f, 0.f });
}

void CMiddlePart::Intro()
{
	if (m_pAnimationCom->CanEnd())
	{		
		ChangeState(INTRO);		
		return;
	}
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
}

void CMiddlePart::Begin_Intro2()
{
	m_pTransformCom->m_vScale = { 512.f, 256.f, 1.f };
	m_pTransformCom->Set_Pos({ 0.f, WINCY * -0.5f + 125.f, 0.f });
	m_fTime = 0.f;
}

void CMiddlePart::Intro2()
{
	if (m_pAnimationCom->CanEnd())
	{
		//ChangeState(IDLE);
		m_pPlayer->Change_State(IDLE);
		return;
	}
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
}
	
