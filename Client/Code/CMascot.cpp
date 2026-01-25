#include "pch.h"
#include "CMascot.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CNoise.h"
#include "CSoundMgr.h"

vector<TextureSource> CMascot::m_vTextureSource =
{
	{ CENTER,  L"../Bin/Resource/Texture/UI/Mascot_Center.dds" },
	{ LEFT,  L"../Bin/Resource/Texture/UI/Mascot_Left.dds" },
	{ RIGHT,  L"../Bin/Resource/Texture/UI/Mascot_Right.dds" }

};

vector<AnimationSource>  CMascot::m_vAnimSource =
{
	{ CENTER,1,7,3, true, 0.12f},
	{ LEFT,1,7,3, true, 0.12f},
	{ RIGHT,1,7,3, true, 0.12f},

};

CMascot::CMascot(LPDIRECT3DDEVICE9 pGraphicDev)
	: CBaseUI(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pAnimationCom(nullptr), m_pStateCom(nullptr), m_iNumber(0)
	, m_fTime(0.f), m_pNoise(nullptr)
{
	m_iOrder = 4;
}

CMascot::CMascot(const CMascot& rhs)
	: CBaseUI(rhs)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pAnimationCom(nullptr), m_pStateCom(nullptr), m_iNumber(0)
	, m_fTime(0.f), m_pNoise(nullptr)
{
	m_iOrder = 4;
}

CMascot::~CMascot()
{
}

CMascot* CMascot::Create(PDIRECT3DDEVICE9 pGraphicDev)
{
	CMascot* pMascot = new CMascot(pGraphicDev);

	if (FAILED(pMascot->Ready_GameObject()))
	{
		Safe_Release(pMascot);
		MSG_BOX("Mascot Create Failed");
		return nullptr;
	}

	return pMascot;
}

void CMascot::CreateStateData()
{
	auto Mgr = CDataMgr<CMascot>::GetInstance();
	if (Mgr->IsStateEmpty() == false) return;

	CState<CMascot>* State = new CState<CMascot>(&CMascot::Begin_Idle, &CMascot::Idle, nullptr);
	Mgr->AddState(CENTER, State);
	State = new CState<CMascot>(&CMascot::Begin_Idle, &CMascot::Idle, nullptr);
	Mgr->AddState(LEFT, State);
	State = new CState<CMascot>(&CMascot::Begin_Idle, &CMascot::Idle, nullptr);
	Mgr->AddState(RIGHT, State);
}

HRESULT CMascot::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_MascotAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });
	return S_OK;
}

void CMascot::Free()
{
	Safe_Release(m_pNoise);
	CDataMgr<CMascot>::DestroyInstance();
	CBaseUI::Free();
}

HRESULT CMascot::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pNoise = CNoise::Create(m_pGraphicDev, WINCX - 130.f, 180.f, 240.f, 400.f);

	if (nullptr == m_pNoise)
		E_FAIL;

	CreateStateData();

	return S_OK;
}

_int CMascot::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CBaseUI::Update_GameObject(fTimeDelta);
	m_pNoise->Update_GameObject(fTimeDelta);
	m_fTime += fTimeDelta;
	

	return iExit;
}

void CMascot::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CBaseUI::LateUpdate_GameObject(fTimeDelta);
	m_pNoise->LateUpdate_GameObject(fTimeDelta);
	m_pAnimationCom->Update_State(m_pStateCom->GetCurrentStateID());
}

void CMascot::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pAnimationCom->Render_Animation();
	m_pBufferCom->Render_Buffer();
}

void CMascot::Rotate(ROTATION eType, const _float& fAngle)
{
	m_pTransformCom->Rotation(eType, fAngle);
}

void CMascot::SetPos(_vec3 _pos)
{
	m_pTransformCom->Set_Pos(_pos.x - WINCX * 0.5f, -_pos.y + WINCY * 0.5f, 0.f);
}

void CMascot::SetScale(_float fCX, _float fCY)
{
	m_pTransformCom->Set_Scale(fCX * 0.5f, fCY * 0.5f, 1.f);
}

void CMascot::ChangeState(_uint nextStateID)
{
	m_fTime = 0.f;
	//현재 상태에 맞는 애니메이션으로 자동 전환
	m_pAnimationCom->Update_State(nextStateID);
	m_pStateCom->ChangeState<CMascot>(nextStateID);
}


void CMascot::Begin_Idle()
{
	m_fSizeX = 240.f;
	m_fSizeY = 480.f;
	m_fX = WINCX - 130.f;
	m_fY = 220.f;

	SetScale(m_fSizeX, m_fSizeY);
	SetPos({ m_fX, m_fY, 0.f });
}

void CMascot::Idle()
{
	if (m_fTime > 2.f)
	{
		Get_Number();
		ChangeState(m_iNumber);			

		return;	
	}
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);	
}
