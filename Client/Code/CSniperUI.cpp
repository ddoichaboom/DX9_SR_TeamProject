#include "pch.h"
#include "CSniperUI.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

TextureSource CSniperUI::m_textureSource =
{
	0, L"../Bin/Resource/Texture/Sniper/SniperUI.dds"
};


CSniperUI::CSniperUI(LPDIRECT3DDEVICE9 pGraphicDev)
	:CBaseUI(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
{
}

CSniperUI::CSniperUI(const CSniperUI& rhs)
	:CBaseUI(rhs), m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
{
}

CSniperUI::~CSniperUI()
{
}

CSniperUI* CSniperUI::Create(PDIRECT3DDEVICE9 pGraphicDev)
{
	CSniperUI* pSniperUI = new CSniperUI(pGraphicDev);
	if (!pSniperUI) return nullptr;

	if (FAILED(pSniperUI->Ready_GameObject()))
	{
		Safe_Release(pSniperUI);
		MSG_BOX("SniperUI Create Failed");
		return nullptr;
	}
	return pSniperUI;
}

HRESULT CSniperUI::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(WINCX*0.75f, WINCX * 0.75f, 1.f);
	m_pTransformCom->Set_Pos(0.f, 0.f, 0.f);
	m_pTransformCom->Update_Component(1.f);

	m_pTextureCom->Change_Texture(0);
	return S_OK;
}

_int CSniperUI::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CBaseUI::Update_GameObject(fTimeDelta);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	return iExit;
}

void CSniperUI::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pTextureCom->Render_Texture();
	m_pBufferCom->Render_Buffer();
}

HRESULT CSniperUI::Add_Component()
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

	pComponent = m_pTextureCom = static_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SniperUITexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CSniperUI::Free()
{
	CGameObject::Free();
}
