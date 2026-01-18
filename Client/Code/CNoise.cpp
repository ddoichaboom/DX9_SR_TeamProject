#include "pch.h"
#include "CNoise.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

vector<TextureSource> CNoise::m_vTextureSource =
{
	{ NOISE_IDLE,  L"../Bin/Resource/Texture/UI/UI_Noise.dds" },

};

vector<AnimationSource>  CNoise::m_vAnimSource =
{
	{ NOISE_IDLE,1,3,0, true, 0.15f},

};

CNoise::CNoise(LPDIRECT3DDEVICE9 pGraphicDev)
	: CBaseUI(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pAnimationCom(nullptr), m_pStateCom(nullptr)
{
	m_iOrder = 7;
}

CNoise::CNoise(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _float fSizeX, _float fSizeY)
	: CBaseUI(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pAnimationCom(nullptr), m_pStateCom(nullptr)
{
	m_fX = fX;
	m_fY = fY;
	m_fSizeX = fSizeX;
	m_fSizeY = fSizeY;
	m_iOrder = 7;
}

CNoise::CNoise(const CNoise& rhs)
	: CBaseUI(rhs)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pAnimationCom(nullptr), m_pStateCom(nullptr)
{
	m_iOrder = 7;
}

CNoise::~CNoise()
{
}

CNoise* CNoise::Create(PDIRECT3DDEVICE9 pGraphicDev)
{
	CNoise* pNoise = new CNoise(pGraphicDev);

	if (FAILED(pNoise->Ready_GameObject()))
	{
		Safe_Release(pNoise);
		MSG_BOX("Chat Create Failed");
		return nullptr;
	}

	return pNoise;
}

CNoise* CNoise::Create(PDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CNoise* pNoise = new CNoise(pGraphicDev, fX, fY, fSizeX, fSizeY);

	if (FAILED(pNoise->Ready_GameObject()))
	{
		Safe_Release(pNoise);
		MSG_BOX("Chat Create Failed");
		return nullptr;
	}

	return pNoise;
}

void CNoise::CreateStateData()
{
	auto Mgr = CDataMgr<CNoise>::GetInstance();
	if (Mgr->IsStateEmpty() == false) return;

	CState<CNoise>* State = new CState<CNoise>(&CNoise::Begin_Idle, &CNoise::Idle, nullptr);
	Mgr->AddState(NOISE_IDLE, State);
}

HRESULT CNoise::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_NoiseAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });
	return S_OK;
}

void CNoise::Free()
{
	CDataMgr<CNoise>::DestroyInstance();
	CBaseUI::Free();
}

HRESULT CNoise::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CreateStateData();


	SetScale(m_fSizeX, m_fSizeY);
	SetPos({ m_fX, m_fY, 0.f });

	Set_On();

	return S_OK;
}

_int CNoise::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CBaseUI::Update_GameObject(fTimeDelta);
	return iExit;
}

void CNoise::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CBaseUI::LateUpdate_GameObject(fTimeDelta);
	m_pAnimationCom->Update_State(m_pStateCom->GetCurrentStateID());
}

void CNoise::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pAnimationCom->Render_Animation();
	m_pBufferCom->Render_Buffer();
}

void CNoise::Rotate(ROTATION eType, const _float& fAngle)
{
	m_pTransformCom->Rotation(eType, fAngle);
}

void CNoise::SetPos(_vec3 _pos)
{
	m_pTransformCom->Set_Pos(_pos.x - WINCX * 0.5f, -_pos.y + WINCY * 0.5f, 0.f);
}

void CNoise::SetScale(_float fCX, _float fCY)
{
	m_pTransformCom->Set_Scale(fCX * 0.5f, fCY * 0.5f, 1.f);
}

void CNoise::ChangeState(_uint nextStateID)
{
	//현재 상태에 맞는 애니메이션으로 자동 전환
	m_pAnimationCom->Update_State(nextStateID);
	m_pStateCom->ChangeState<CNoise>(nextStateID);
}

void CNoise::Begin_Idle()
{

}

void CNoise::Idle()
{
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
}
