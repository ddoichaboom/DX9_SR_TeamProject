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
	{ GetStateID(INTRO,WEAPON_PISTOL),  L"../Bin/Resource/Texture/Player/Player_Intro_Begin.dds"},

	{ INTRO,  L"../Bin/Resource/Texture/Player/Player_Intro.dds" },
	{ SHOP,  L"../Bin/Resource/Texture/Player/Player_Shop.dds" },

};

vector<AnimationSource>  CMiddlePart::m_vAnimSource =
{
	{ IDLE,0,3,3, true, 0.08f},
	{ KICK,0,3,3, false, 0.08f, 1.f},
	{ DRINK,0,6,6, false, 0.1f, 0.8f},
	{ SLIDE,1,0,0, true, 0.08f},
	{ GetStateID(INTRO,WEAPON_PISTOL),1,2,2, false, 0.12f, 1.f},
	{ INTRO,1,2,2, false, 0.12f, 1.f},
	{ SHOP,1,0,0, true, 0.12f, 1.f},
};

CMiddlePart::CMiddlePart(LPDIRECT3DDEVICE9 pGraphicDev)
	: CPlayerPart(pGraphicDev), m_iLoopTime(3)
	, m_fX(0.f), m_fY(0.f), m_fSizeX(0.f), m_fSizeY(0.f)
	, m_fDelayTime(0.f), m_bDelay(false), m_bStateStop(false)
{	
}

CMiddlePart::CMiddlePart(const CMiddlePart& rhs)
	: CPlayerPart(rhs), m_iLoopTime(3)
	, m_fX(0.f), m_fY(0.f), m_fSizeX(0.f), m_fSizeY(0.f)
	, m_fDelayTime(0.f), m_bDelay(false), m_bStateStop(false)
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
	Mgr->AddState(GetStateID(INTRO, WEAPON_PISTOL), State);

	State = new CState<CMiddlePart>(&CMiddlePart::Begin_Intro2, &CMiddlePart::Intro2, nullptr);
	Mgr->AddState(INTRO, State);

	State = new CState<CMiddlePart>(&CMiddlePart::Begin_Shop, &CMiddlePart::Shopping, &CMiddlePart::End_Shop);
	Mgr->AddState(SHOP, State);

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
	return S_OK;
}

_int CMiddlePart::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CPlayerPart::Update_GameObject(fTimeDelta);	
	m_fTime += fTimeDelta;

	if (m_bDelay)
		m_fDelayTime += fTimeDelta;

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
	m_fTime = 0.f;
	m_fSizeX = 512.f;
	m_fSizeY = 512.f;
	m_fX = WINCX * 0.5f;
	m_fY = WINCY - m_fSizeY * 0.5f;
	m_vStartPos = { m_fX, m_fY, 0.f };
	m_vEndPos = { m_fX, m_fY, 0.f };
	m_pTransformCom->Set_Scale(m_fSizeX * 0.5f, m_fSizeY * 0.5f, 1.f);
	m_pTransformCom->Set_Pos(m_fX - WINCX * 0.5f, -m_fY + WINCY * 0.5f, 0.f);
}

void CMiddlePart::Idle()
{

}

void CMiddlePart::Begin_Kick()
{
	m_fSizeX = 512.f;
	m_fSizeY = 512.f;
	m_fX = WINCX * 0.5f;
	m_fY = WINCY - m_fSizeY * 0.5f;
	m_vStartPos = { m_fX, m_fY, 0.f };
	m_vEndPos = { m_fX, m_fY, 0.f };
	m_pTransformCom->Set_Scale(m_fSizeX * 0.5f, m_fSizeY * 0.5f, 1.f);
	m_pTransformCom->Set_Pos(m_fX - WINCX * 0.5f, -m_fY + WINCY * 0.5f, 0.f);
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
	m_fSizeX = 700.f;
	m_fSizeY = 700.f;
	m_fX = WINCX * 0.5f;
	m_fY = WINCY -100.f;
	m_vStartPos = { m_fX, m_fY, 0.f };
	m_vEndPos = { m_fX, m_fY, 0.f };
	m_pTransformCom->Set_Scale(m_fSizeX * 0.5f, m_fSizeY * 0.5f, 1.f);
	m_pTransformCom->Set_Pos(m_fX - WINCX * 0.5f, -m_fY + WINCY * 0.5f, 0.f);
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
	m_fSizeX = 1024.f;
	m_fSizeY = 512.f;
	m_fX = WINCX * 0.5f;
	m_fY = WINCY - m_fSizeY * 0.5f;
	m_vStartPos = { m_fX, m_fY, 0.f };
	m_vEndPos = { m_fX, m_fY, 0.f };
	m_pTransformCom->Set_Scale(m_fSizeX * 0.5f, m_fSizeY * 0.5f, 1.f);
	m_pTransformCom->Set_Pos(m_fX - WINCX * 0.5f, -m_fY + WINCY * 0.5f, 0.f);
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
	m_fSizeX = 1024.f;
	m_fSizeY = 1024.f;
	m_fX = WINCX * 0.5f;
	m_fY = WINCY - 100.f;
	m_vStartPos = { m_fX, m_fY, 0.f };
	m_vEndPos = { m_fX, m_fY, 0.f };
	m_pTransformCom->Set_Scale(m_fSizeX * 0.5f, m_fSizeY * 0.5f, 1.f);
	m_pTransformCom->Set_Pos(m_fX - WINCX * 0.5f, -m_fY + WINCY * 0.5f, 0.f);
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
	m_fSizeX = 1024.f;
	m_fSizeY = 512.f;
	m_fX = WINCX * 0.5f;
	m_fY = WINCY - 50.f;
	m_vStartPos = { m_fX, m_fY, 0.f };
	m_vEndPos = { m_fX, m_fY, 0.f };
	m_pTransformCom->Set_Scale(m_fSizeX * 0.5f, m_fSizeY * 0.5f, 1.f);
	m_pTransformCom->Set_Pos(m_fX - WINCX * 0.5f, -m_fY + WINCY * 0.5f, 0.f);
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

void CMiddlePart::Begin_Shop()
{
	m_fSizeX = 800.f;
	m_fSizeY = 400.f;
	m_fX = 400.f;
	m_fY = WINCY;
	m_vStartPos = { m_fX, m_fY, 0.f };
	m_vEndPos = { m_fX, m_fY - 150.f , 0.f };

	m_vStartScale = { m_fSizeX * 0.5f, m_fSizeY * 0.5f, 1.f };
	m_vEndScale = { m_fSizeX * 0.7f, m_fSizeY * 0.7f, 1.f };
	m_pTransformCom->Set_Scale(m_vStartScale);
	m_pTransformCom->Set_Pos(m_fX - WINCX * 0.5f, -m_fY + WINCY * 0.5f, 0.f);

	m_bDelay = true;
	m_fDelayTime = 0.f;
	m_bStateStop = false;
}

void CMiddlePart::Shopping()
{
	if (m_bStateStop)
	{
		CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
		return;
	}

	_vec3 vPos;
	_vec3 vScale;
	_float fTime;

	if (m_bDelay)
	{
		if (m_fDelayTime < 1.f)
		{
			fTime = m_fTime * 3.f;
			D3DXVec3Lerp(&vPos, &m_vStartPos, &m_vEndPos, fTime);
			m_pTransformCom->Set_Pos(vPos.x - WINCX * 0.5f, -vPos.y + WINCY * 0.5f, 0.f);
			CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
			return;
		}
		else
		{
			m_bDelay = false;
			m_fTime = 0.f;
		}
	}	
	
	fTime = m_fTime * 3.f;
	D3DXVec3Lerp(&vScale, &m_vStartScale, &m_vEndScale, fTime);

	if (fTime < 1.f)
	{
		m_pTransformCom->Set_Scale(vScale);
	}	
	if (fTime > 1.f)
	{
		m_pTransformCom->Set_Scale(vScale);


		m_bStateStop = true;
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
}

void CMiddlePart::End_Shop()
{
	m_bStateStop = false;
}
	
