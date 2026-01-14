#include "pch.h"
#include "CLeftPart.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"

#include "CPlayer.h"

vector<TextureSource> CLeftPart::m_vTextureSource =
{
	{ IDLE, L"../Bin/Resource/Texture/Player/Left_Hand_Idle.dds" },
	{ GetStateID(RELOAD,SW_PISTOL), L"../Bin/Resource/Texture/Player/Left_Hand_Reload_P.dds"},
	{ GetStateID(RELOAD,SW_SHOTGUN), L"../Bin/Resource/Texture/Player/Left_Hand_Reload_S.dds" },
	{ GetStateID(INTRO,SW_KATANA), L"../Bin/Resource/Texture/Player/Left_Hand_Intro_Katana.dds" }
};

vector<AnimationSource>  CLeftPart::m_vAnimSource =
{
	{ IDLE,1,3,3, true, 0.11f},
	{ GetStateID(RELOAD,SW_PISTOL),0,3,3, false, 0.11f, 1.f},
	{ GetStateID(RELOAD,SW_SHOTGUN),0,3,3, false, 0.11f},
	{ GetStateID(INTRO,SW_KATANA),1,0,0, true, 0.11f}
};



CLeftPart::CLeftPart(LPDIRECT3DDEVICE9 pGraphicDev)
	: CPlayerPart(pGraphicDev), m_bReload(false)
{
}

CLeftPart::CLeftPart(const CLeftPart& rhs)
	: CPlayerPart(rhs), m_bReload(false)
{
}

CLeftPart::~CLeftPart()
{
}

void CLeftPart::CreateStateData()
{
	auto Mgr = CDataMgr<CLeftPart>::GetInstance();
	if (Mgr->IsStateEmpty() == false) return;

	CState<CLeftPart>* State = new CState<CLeftPart>(&CLeftPart::Begin_Idle, &CLeftPart::Idle, nullptr);
	Mgr->AddState(IDLE, State);

	State = new CState<CLeftPart>(&CLeftPart::Begin_Reload, &CLeftPart::Reload, &CLeftPart::End_Reload);
	Mgr->AddState(GetStateID(RELOAD, SW_PISTOL), State);

	State = new CState<CLeftPart>(&CLeftPart::Begin_Reload, &CLeftPart::Reload, &CLeftPart::End_Reload);
	Mgr->AddState(GetStateID(RELOAD, SW_SHOTGUN), State);

	State = new CState<CLeftPart>(&CLeftPart::Begin_Intro, &CLeftPart::Intro, &CLeftPart::End_Intro);
	Mgr->AddState(GetStateID(INTRO, SW_KATANA), State);
}

CLeftPart* CLeftPart::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLeftPart* pPart = new CLeftPart(pGraphicDev);

	if (FAILED(pPart->Ready_GameObject()))
	{
		Safe_Release(pPart);
		MSG_BOX("Left Part Create Failed");
		return nullptr;
	}

	return pPart;
}

_bool CLeftPart::Get_ActionAble()
{
	return !m_bReload;
}

HRESULT CLeftPart::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CreateStateData();
		
	m_vStartPos = { 200.f, WINCY - 200.f, 0.f };
	m_vEndPos	= { WINCX - 200.f, WINCY - 200.f, 0.f };	

	//ChangeState(IDLE);
	return S_OK;
}

_int CLeftPart::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CPlayerPart::Update_GameObject(fTimeDelta);

	m_fTime += fTimeDelta;
	if (m_bDelay)
		m_fDelayTime += fTimeDelta;
	//CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
	return iExit;
}

void CLeftPart::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CPlayerPart::LateUpdate_GameObject(fTimeDelta);

	m_pAnimationCom->Update_State(m_pStateCom->GetCurrentStateID());
}

void CLeftPart::Render_GameObject()
{
	CPlayerPart::Render_GameObject();
}

HRESULT CLeftPart::Add_Component()
{
	if (FAILED(CPlayerPart::Add_Component()))
		return E_FAIL;

	Engine::CComponent* pComponent = nullptr;

	pComponent = m_pAnimationCom = dynamic_cast<Engine::CAnimation*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_LeftAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });

	return S_OK;
}

void CLeftPart::Free()
{
	CGameObject::Free();
}

void CLeftPart::ChangeState(_uint nextStateID)
{
	//m_fTime은 맨 위로 고정! ChangeState에서 실행되는 함수(Begin,End)에서 fTime을 바꿀수도있음
	m_fTime = 0.f;
	//현재 상태에 맞는 애니메이션으로 자동 전환
	m_pAnimationCom->Update_State(nextStateID);
	m_pStateCom->ChangeState<CLeftPart>(nextStateID);	
}

void CLeftPart::Begin_Idle()
{
	m_bReload = false;

	m_vConvertPos = m_vStartPos;
	m_vConvertScale = { 512.f, 512.f, 1.f };
	
		
	m_pTransformCom->Set_Scale(m_vConvertScale * 0.5f);
	m_pTransformCom->Set_Pos(m_vConvertPos.x - WINCX * 0.5f, -m_vConvertPos.y + WINCY * 0.5f, 0.f);
}

void CLeftPart::Idle()
{
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
}

void CLeftPart::Begin_Reload()
{
	m_bReload = true;
	m_fTime = 0.f;
	m_vConvertPos = m_vStartPos;
	m_vConvertScale = { 512.f, 512.f, 1.f };


	m_pTransformCom->Set_Scale(m_vConvertScale * 0.5f);
	m_pTransformCom->Set_Pos(m_vConvertPos.x - WINCX * 0.5f, -m_vConvertPos.y + WINCY * 0.5f, 0.f);
}

void CLeftPart::Reload()
{
	_vec3 vPos;
	_float fTime;
	fTime = m_fTime * 2.f;
	D3DXVec3Lerp(&vPos, &m_vStartPos, &m_vEndPos, fTime);
	//m_pTransformCom->Set_Pos(vPos);
	m_pTransformCom->Set_Pos(vPos.x - WINCX * 0.5f, -vPos.y + WINCY * 0.5f, 0.f);
	if (fTime > 1.f)
	{
		m_pTransformCom->Set_Pos(m_vEndPos.x - WINCX * 0.5f, -m_vEndPos.y + WINCY * 0.5f, 0.f);
		if (m_pAnimationCom->CanEnd())
		{
			//ChangeState(IDLE);
			m_pPlayer->Reload_Func();
			m_pPlayer->Change_State(IDLE);
			return;
		}
	}
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
}

void CLeftPart::End_Reload()
{

}

void CLeftPart::Begin_Intro()
{

	m_vStartPos = { 250.f, WINCY - 200.f, 0.f };
	m_vEndPos = { -700.f, WINCY - 200.f, 0.f };

	m_vConvertScale = { 1024.f, 512.f, 1.f };


	m_pTransformCom->Set_Scale(m_vConvertScale * 0.5f);
	m_pTransformCom->Set_Pos(m_vStartPos.x - WINCX * 0.5f, -m_vStartPos.y + WINCY * 0.5f, 0.f);
	m_fTime = 0.f;
	m_fDelayTime = 0.f;
	m_bDelay = true;
}

void CLeftPart::Intro()
{
	_vec3 vPos;
	_float fTime;
	if (m_fDelayTime < 1.f)
	{		
		CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
		return;
	}
	else if(m_bDelay)
	{
		m_fTime = 0.f;
		m_bDelay = false;
	}
	
	fTime = m_fTime * 1.5f;
	D3DXVec3Lerp(&vPos, &m_vStartPos, &m_vEndPos, fTime);
	
	m_pTransformCom->Set_Pos(vPos.x - WINCX * 0.5f, -vPos.y + WINCY * 0.5f, 0.f);
	if (fTime > 1.f)
	{
		m_pTransformCom->Set_Pos(m_vEndPos.x - WINCX * 0.5f, -m_vEndPos.y + WINCY * 0.5f, 0.f);
		m_pPlayer->Change_State(IDLE);
	}
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
}

void CLeftPart::End_Intro()
{
	m_vStartPos = { 200.f, WINCY - 200.f, 0.f };
	m_vEndPos = { WINCX - 200.f, WINCY - 200.f, 0.f };
	m_fDelayTime = 0.f;
	m_bDelay = false;
}

