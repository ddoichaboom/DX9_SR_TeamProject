#include "pch.h"
#include "CDashUI.h"
#include "CPlusUI.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CFontMgr.h"
#include "CFontUI.h"

vector<TextureSource> CDashUI::m_vTextureSource =
{
	{ DASH_IDLE,  L"../Bin/Resource/Texture/UI/Dash_Effect.dds" },

};

vector<AnimationSource>  CDashUI::m_vAnimSource =
{
	{ DASH_IDLE,1,3,1, true, 0.4f},

};

CDashUI::CDashUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CBaseUI(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pAnimationCom(nullptr), m_pStateCom(nullptr), m_fTime(0.f)
{

}
CDashUI::CDashUI(const CDashUI& rhs)
	: CBaseUI(rhs)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pAnimationCom(nullptr), m_pStateCom(nullptr), m_fTime(0.f)
{

}

CDashUI::~CDashUI()
{
}

CDashUI* CDashUI::Create(PDIRECT3DDEVICE9 pGraphicDev)
{
	CDashUI* pDash = new CDashUI(pGraphicDev);
	if (!pDash) return nullptr;

	if (FAILED(pDash->Ready_GameObject()))
	{
		Safe_Release(pDash);
		MSG_BOX("DashUI Create Failed");
		return nullptr;
	}
	return pDash;
}

void CDashUI::CreateStateData()
{
	auto Mgr = CDataMgr<CDashUI>::GetInstance();
	if (Mgr->IsStateEmpty() == false) return;

	CState<CDashUI>* State = new CState<CDashUI>(&CDashUI::Begin_Idle, &CDashUI::Idle, nullptr);
	Mgr->AddState(DASH_IDLE, State);
}

HRESULT CDashUI::Add_Component()
{
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

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	//StateComponent
	pComponent = m_pStateCom = dynamic_cast<Engine::CStateComponent*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_StateComponent"));

	m_pStateCom->SetOnwer(this);
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_StateComponent", pComponent });

	//Animation
	pComponent = m_pAnimationCom = dynamic_cast<Engine::CAnimation*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_DashAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });
	return S_OK;
}

void CDashUI::Free()
{
	CDataMgr<CDashUI>::DestroyInstance();
	CGameObject::Free();

}

HRESULT CDashUI::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CreateStateData();


	m_vPos = { WINCX * 0.5f, WINCY * 0.5f, 0.f };
	m_fSizeX = WINCX;
	m_fSizeY = WINCY + 500.f;

	SetPos(m_vPos);
	SetScale(m_fSizeX, m_fSizeY);
	
	Set_On();

	return S_OK;
}

_int CDashUI::Update_GameObject(const _float& fTimeDelta)
{
	if (IsDead()) return RET_DEAD;
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	m_fTime += fTimeDelta;

	if (m_fTime > 0.4f)
	{
		m_bDead = true;		
		return RET_NONE;
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);

	return iExit;
}

void CDashUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pAnimationCom->Update_State(m_pStateCom->GetCurrentStateID());
}

void CDashUI::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pAnimationCom->Render_Animation();
	m_pBufferCom->Render_Buffer();
}

void CDashUI::Activate()
{
	CGameObject::Activate();
	m_fTime = 0.f;	
	Set_On();
}

void CDashUI::Deactivate()
{
	CGameObject::Deactivate();
}

void CDashUI::Rotate(ROTATION eType, const _float& fAngle)
{
	m_pTransformCom->Rotation(eType, fAngle);
}

void CDashUI::SetPos(_vec3 _pos)
{
	m_pTransformCom->Set_Pos(_pos.x - WINCX * 0.5f, -_pos.y + WINCY * 0.5f, 0.f);
}

void CDashUI::SetScale(_float fCX, _float fCY)
{
	m_pTransformCom->Set_Scale(fCX * 0.5f, fCY * 0.5f, 1.f);
}

void CDashUI::ChangeState(_uint nextStateID)
{
	//현재 상태에 맞는 애니메이션으로 자동 전환	
	m_pAnimationCom->Update_State(nextStateID);
	m_pStateCom->ChangeState<CDashUI>(nextStateID);
}

void CDashUI::Begin_Idle()
{
	m_fTime = 0.f;
}

void CDashUI::Idle()
{

	
}
