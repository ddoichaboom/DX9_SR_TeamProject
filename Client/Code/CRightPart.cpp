#include "pch.h"
#include "CRightPart.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"

#include "CPlayer.h"

vector<TextureSource> CRightPart::m_vTextureSource =
{
	{ GetStateID(IDLE,SW_PISTOL),	L"../Bin/Resource/Texture/Player/Right_Hand_Idle_P.dds" },
	{ GetStateID(ATTACK,SW_PISTOL), L"../Bin/Resource/Texture/Player/Right_Hand_Shot_P.dds" },
	{ GetStateID(RELOAD,SW_PISTOL), L"../Bin/Resource/Texture/Player/Right_Hand_Reload_P.dds"},
	{ GetStateID(INTRO,SW_KATANA), L"../Bin/Resource/Texture/Player/Right_Hand_Intro_Katana.dds"}

};

vector<AnimationSource>  CRightPart::m_vAnimSource =
{
	{ GetStateID(IDLE,SW_PISTOL),0,3,3, true, 0.11f},
	{ GetStateID(ATTACK,SW_PISTOL),0,5,5, false, 0.02f,	0.9f},
	{ GetStateID(RELOAD,SW_PISTOL),1,6,6, false, 0.04f, 0.9f},
	{ GetStateID(INTRO,SW_KATANA),1,0,0, true, 0.04f},
};



CRightPart::CRightPart(LPDIRECT3DDEVICE9 pGraphicDev)
	: CPlayerPart(pGraphicDev), m_bReload(false), m_bAttack(false)
{
}

CRightPart::CRightPart(const CRightPart& rhs)
	: CPlayerPart(rhs), m_bReload(false), m_bAttack(false)
{
}

CRightPart::~CRightPart()
{
}

void CRightPart::CreateStateData()
{
	auto Mgr = CDataMgr<CRightPart>::GetInstance();
	if (Mgr->IsStateEmpty() == false) return;

	CState<CRightPart>* State = new CState<CRightPart>(&CRightPart::Begin_Idle, &CRightPart::Idle, nullptr);
	Mgr->AddState(GetStateID(IDLE, SW_PISTOL), State);

	State = new CState<CRightPart>(&CRightPart::Begin_Attack, &CRightPart::Attack, &CRightPart::End_Attack);
	Mgr->AddState(GetStateID(ATTACK, SW_PISTOL), State);

	State = new CState<CRightPart>(&CRightPart::Begin_Reload, &CRightPart::Reload, &CRightPart::End_Reload);
	Mgr->AddState(GetStateID(RELOAD, SW_PISTOL), State);

}

CRightPart* CRightPart::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRightPart* pRightPart = new CRightPart(pGraphicDev);

	if (FAILED(pRightPart->Ready_GameObject()))
	{
		Safe_Release(pRightPart);
		MSG_BOX("Right Part Create Failed");
		return nullptr;
	}

	return pRightPart;
}

_bool CRightPart::Get_ActionAble()
{
	return m_pAnimationCom->CanEnd();
}

HRESULT CRightPart::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CreateStateData();
	ChangeState(GetStateID(IDLE, SW_PISTOL));

	m_pTransformCom->m_vScale = { 256.f, 256.f, 1.f };
	m_vStartPos = { WINCX * 0.5f - 200.f, WINCY * -0.5f + 150.f, 0.f };


	m_pTransformCom->Set_Pos(m_vStartPos);

	return S_OK;
}

_int CRightPart::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CPlayerPart::Update_GameObject(fTimeDelta);
	return iExit;
}

void CRightPart::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CPlayerPart::LateUpdate_GameObject(fTimeDelta);
	m_pAnimationCom->Update_State(m_pStateCom->GetCurrentStateID());
}

void CRightPart::Render_GameObject()
{
	CPlayerPart::Render_GameObject();
}

HRESULT CRightPart::Add_Component()
{
	if (FAILED(CPlayerPart::Add_Component()))
		return E_FAIL;

	Engine::CComponent* pComponent = nullptr;

	pComponent = m_pAnimationCom = dynamic_cast<Engine::CAnimation*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RightAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });
	return S_OK;
}

void CRightPart::Free()
{
	CPlayerPart::Free();

}

void CRightPart::ChangeState(_uint nextStateID)
{
	//m_fTime은 맨 위로 고정! ChangeState에서 실행되는 함수(Begin,End)에서 fTime을 바꿀수도있음
	m_fTime = 0.f;
	//현재 상태에 맞는 애니메이션으로 자동 전환
	m_pAnimationCom->Update_State(nextStateID);
	m_pStateCom->ChangeState<CRightPart>(nextStateID);
}

void CRightPart::Begin_Idle()
{
	m_bAttack = false;
	m_bReload = false;
}

void CRightPart::Idle()
{
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
}

void CRightPart::Begin_Attack()
{

}

void CRightPart::Attack()
{
	if (m_pAnimationCom->CanEnd())
	{
		m_pPlayer->Fire();
		ChangeState(GetStateID(IDLE, m_pPlayer->Get_WeaponState()));
	}
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
}

void CRightPart::End_Attack()
{

}

void CRightPart::Begin_Reload()
{

}

void CRightPart::Reload()
{
	if (m_pAnimationCom->CanEnd())
	{
		m_pPlayer->Reload();
		ChangeState(GetStateID(IDLE, m_pPlayer->Get_WeaponState()));
	}
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);

}

void CRightPart::End_Reload()
{

}

