#include "pch.h"
#include "CLeftPart.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"

vector<TextureSource> CLeftPart::m_vTextureSource =
{
	{ IDLE, L"../Bin/Resource/Texture/Player/Left_Hand_Idle.png" },
	{ GetStateID(RELOAD,SW_PISTOL), L"../Bin/Resource/Texture/Player/Left_Hand_Reload_P.png"},
	{ GetStateID(RELOAD,SW_SHOTGUN), L"../Bin/Resource/Texture/Player/Left_Hand_Reload_S.png" }
};

vector<AnimationSource>  CLeftPart::m_vAnimSource =
{
	{ IDLE,1,3,3, true, 0.11f},
	{ GetStateID(RELOAD,SW_PISTOL),0,3,3, false, 0.11f, 0.f},
	{ GetStateID(RELOAD,SW_SHOTGUN),0,3,3, false, 0.11f}
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
	ChangeState(IDLE);

	m_pTransformCom->m_vScale = { 256.f, 256.f, 1.f };
	m_vStartPos = { WINCX * -0.5f + 200.f, WINCY * -0.5f + 150.f, 0.f };
	m_vEndPos = { WINCX * 0.5f - 200.f, WINCY * -0.5f + 150.f, 0.f };

	m_pTransformCom->Set_Pos(m_vStartPos);

	return S_OK;
}

_int CLeftPart::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CPlayerPart::Update_GameObject(fTimeDelta);

	m_fTime += fTimeDelta;

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
	m_pTransformCom->Set_Pos(m_vStartPos);
}

void CLeftPart::Idle()
{
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
}

void CLeftPart::Begin_Reload()
{
	m_bReload = true;
}

void CLeftPart::Reload()
{
	_vec3 vPos;
	_float fTime;
	fTime = m_fTime * 2.f;
	D3DXVec3Lerp(&vPos, &m_vStartPos, &m_vEndPos, fTime);
	m_pTransformCom->Set_Pos(vPos);

	if (fTime > 1.f)
	{
		m_pTransformCom->Set_Pos(m_vEndPos);
		if (m_pAnimationCom->CanEnd())
		{
			ChangeState(IDLE);
			return;
		}
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
}

void CLeftPart::End_Reload()
{

}

