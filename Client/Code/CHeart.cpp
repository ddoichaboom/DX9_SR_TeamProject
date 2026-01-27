#include "pch.h"
#include "CHeart.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

vector<TextureSource> CHeart::m_vTextureSource =
{
	{ HEART_IDLE,  L"../Bin/Resource/Texture/UI/UI_Heart.dds" },

};

vector<AnimationSource>  CHeart::m_vAnimSource =
{
	{ HEART_IDLE,1,3,0, true, 0.12f},

};

CHeart::CHeart(LPDIRECT3DDEVICE9 pGraphicDev)
	: CBaseUI(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pAnimationCom(nullptr), m_pStateCom(nullptr)
{
	m_iOrder = 6;
}

CHeart::CHeart(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY)
	: CBaseUI(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pAnimationCom(nullptr), m_pStateCom(nullptr)
{
	m_fX = fX;
	m_fY = fY;
	m_iOrder = 6;
}

CHeart::CHeart(const CHeart& rhs)
	: CBaseUI(rhs)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pAnimationCom(nullptr), m_pStateCom(nullptr)
{
	m_iOrder = 6;
}

CHeart::~CHeart()
{
}

CHeart* CHeart::Create(PDIRECT3DDEVICE9 pGraphicDev)
{
	CHeart* pHeart = new CHeart(pGraphicDev);

	if (FAILED(pHeart->Ready_GameObject()))
	{
		Safe_Release(pHeart);
		MSG_BOX("Chat Create Failed");
		return nullptr;
	}

	return pHeart;
}

CHeart* CHeart::Create(PDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY)
{
	CHeart* pHeart = new CHeart(pGraphicDev,fX, fY);

	if (FAILED(pHeart->Ready_GameObject()))
	{
		Safe_Release(pHeart);
		MSG_BOX("Chat Create Failed");
		return nullptr;
	}

	return pHeart;
}

void CHeart::CreateStateData()
{
	auto Mgr = CDataMgr<CHeart>::GetInstance();
	if (Mgr->IsStateEmpty() == false) return;

	CState<CHeart>* State = new CState<CHeart>(&CHeart::Begin_Idle, &CHeart::Idle, nullptr);
	Mgr->AddState(HEART_IDLE, State);
}

HRESULT CHeart::Add_Component()
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

	m_pStateCom->SetOnwer(this);
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_StateComponent", pComponent });

	//Animation
	pComponent = m_pAnimationCom = static_cast<Engine::CAnimation*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_HeartAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });
	return S_OK;
}

void CHeart::Free()
{
	CDataMgr<CHeart>::DestroyInstance();
	CBaseUI::Free();
}

HRESULT CHeart::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CreateStateData();

	m_fSizeX = 80.f;
	m_fSizeY = 80.f;

	SetScale(m_fSizeX, m_fSizeY);
	SetPos({ m_fX, m_fY, 0.f });

	return S_OK;
}

_int CHeart::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CBaseUI::Update_GameObject(fTimeDelta);
	return iExit;
}

void CHeart::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CBaseUI::LateUpdate_GameObject(fTimeDelta);
	m_pAnimationCom->Update_State(m_pStateCom->GetCurrentStateID());
}

void CHeart::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pAnimationCom->Render_Animation();
	m_pBufferCom->Render_Buffer();
}

void CHeart::Rotate(ROTATION eType, const _float& fAngle)
{
	m_pTransformCom->Rotation(eType, fAngle);
}

void CHeart::SetPos(_vec3 _pos)
{
	m_pTransformCom->Set_Pos(_pos.x - WINCX * 0.5f, -_pos.y + WINCY * 0.5f, 0.f);
}

void CHeart::SetScale(_float fCX, _float fCY)
{
	m_pTransformCom->Set_Scale(fCX * 0.5f, fCY * 0.5f, 1.f);
}

void CHeart::ChangeState(_uint nextStateID)
{
	//현재 상태에 맞는 애니메이션으로 자동 전환
	m_pAnimationCom->Update_State(nextStateID);
	m_pStateCom->ChangeState<CHeart>(nextStateID);
}

void CHeart::Begin_Idle()
{

}

void CHeart::Idle()
{
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
}
