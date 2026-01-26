#include "pch.h"
#include "CTakeDown.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CTakeDownBlood.h"
vector<TextureSource> CTakeDown::m_vTextureSource =
{
	{ 0,  L"../Bin/Resource/Texture/TakeDown/TakeDown_1024.dds" }
};

vector<AnimationSource>  CTakeDown::m_vAnimSource =
{
	{ 0,5,3,1, false, 0.03f},
};

CTakeDown::CTakeDown(LPDIRECT3DDEVICE9 pGraphicDev)
	: CBaseUI(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pAnimationCom(nullptr), m_pBloodEffect(nullptr)
{
}

CTakeDown::CTakeDown(const CTakeDown& rhs)
	: CBaseUI(rhs)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pAnimationCom(nullptr), m_pBloodEffect(nullptr)
{
	m_iOrder = 0;
}

CTakeDown::~CTakeDown()
{
	m_iOrder = 0;
}

CTakeDown* CTakeDown::Create(PDIRECT3DDEVICE9 pGraphicDev)
{
	CTakeDown* pTakeDown = new CTakeDown(pGraphicDev);

	if (FAILED(pTakeDown->Ready_GameObject()))
	{
		Safe_Release(pTakeDown);
		MSG_BOX("TakeDown Create Failed");
		return nullptr;
	}

	return pTakeDown;
}

HRESULT	CTakeDown::Add_Component()
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

	m_mapComponent[ID_STATIC].insert({ L"Com_Transform", pComponent });

	//Animation
	pComponent = m_pAnimationCom = static_cast<Engine::CAnimation*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_TakeDownAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });

	return S_OK;
}

HRESULT CTakeDown::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale({ WINCX * 0.5f ,WINCY * 0.5f ,1.f });
	m_pTransformCom->Set_Pos({ 0, 0, 0 });
	m_pTransformCom->Update_Component(1);

	m_pAnimationCom->Update_State(0);

	m_pBloodEffect = CTakeDownBlood::Create(m_pGraphicDev);

	return S_OK;
}

_int CTakeDown::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CBaseUI::Update_GameObject(fTimeDelta);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	m_pBloodEffect->Update_GameObject(fTimeDelta);

	if (m_pAnimationCom->IsEnd()) return RET_DEAD;
	return iExit;
}

void CTakeDown::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CBaseUI::LateUpdate_GameObject(fTimeDelta);
	m_pBloodEffect->LateUpdate_GameObject(fTimeDelta);
}

void CTakeDown::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pAnimationCom->Render_Animation();
	m_pBufferCom->Render_Buffer();
}

void CTakeDown::Set_On()
{
	m_pAnimationCom->PlayFromStart();
	m_pBloodEffect->Reset();
}

void CTakeDown::Free()
{
	Safe_Release(m_pBloodEffect);
	CBaseUI::Free();
}