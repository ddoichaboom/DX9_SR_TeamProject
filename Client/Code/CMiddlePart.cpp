#include "pch.h"
#include "CMiddlePart.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"

#include "CPlayer.h"

vector<TextureSource> CMiddlePart::m_vTextureSource =
{
	{ IDLE, L"../Bin/Resource/Texture/Player/Middle_Kick.png"},
	{ KICK, L"../Bin/Resource/Texture/Player/Middle_Kick.png"},
	{ DRINK,  L"../Bin/Resource/Texture/Player/Middle_Soda.png" }
};

vector<AnimationSource>  CMiddlePart::m_vAnimSource =
{
	{ IDLE,0,3,3, false, 0.08f},
	{ KICK,0,3,3, false, 0.08f, 1.f},
	{ DRINK,0,6,6, false, 0.08f, 1.f}
};

CMiddlePart::CMiddlePart(LPDIRECT3DDEVICE9 pGraphicDev)
	: CPlayerPart(pGraphicDev)
{
}

CMiddlePart::CMiddlePart(const CMiddlePart& rhs)
	: CPlayerPart(rhs)
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
}

void CMiddlePart::Idle()
{

}

void CMiddlePart::Begin_Kick()
{
	m_pTransformCom->m_vScale = { 256.f, 256.f, 1.f };
}

void CMiddlePart::Kick()
{
	if (m_pAnimationCom->CanEnd())
	{
		m_pPlayer->Kick();
		ChangeState(IDLE);
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
		ChangeState(IDLE);
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

}

void CMiddlePart::Slide()
{
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
}

void CMiddlePart::End_Slide()
{

}
