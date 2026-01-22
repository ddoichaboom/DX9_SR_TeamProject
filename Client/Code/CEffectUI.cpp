#include "pch.h"
#include "CEffectUI.h"
#include "CPlusUI.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CFontMgr.h"
#include "CFontUI.h"

#include "CUIManager.h"

TextureSource CEffectUI::m_textureSource =
{
	0, L"../Bin/Resource/Texture/UI/UI_Effect.dds"
};

CEffectUI::CEffectUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CBaseUI(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pEffectText(nullptr)
{

}

CEffectUI::CEffectUI(const CEffectUI& rhs)
	: CBaseUI(rhs)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pEffectText(nullptr)
{

}

CEffectUI::~CEffectUI()
{
}

CEffectUI* CEffectUI::Create(PDIRECT3DDEVICE9 pGraphicDev)
{
	CEffectUI* pTextBG = new CEffectUI(pGraphicDev);
	if (!pTextBG) return nullptr;

	if (FAILED(pTextBG->Ready_GameObject()))
	{
		Safe_Release(pTextBG);
		MSG_BOX("Plus Create Failed");
		return nullptr;
	}
	return pTextBG;
}

void CEffectUI::Set_Text(const wstring& wDeadTime)
{
	m_pEffectText->Set_Text(wDeadTime);
}

void CEffectUI::Init()
{
	m_fTime = 0.f;
}

HRESULT CEffectUI::Add_Component()
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

	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_EffectUITexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CEffectUI::Free()
{	
	Safe_Release(m_pEffectText);
	CGameObject::Free();

}

HRESULT CEffectUI::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_vPos = { WINCX * 0.5f, 200.f, 0.f };
	m_fSizeX = 480.f;
	m_fSizeY = 240.f;

	SetPos(m_vPos);
	SetScale(m_fSizeX, m_fSizeY);
	m_pTextureCom->Change_Texture(0);


	m_pEffectText = CFontUI::Create(FONT_LARGEWORD, { 0.f,0.f,0.f }, { 512.f,256.f,0.f });
	if (nullptr == m_pEffectText)
		return E_FAIL;

	m_pEffectText->Set_Parent(this);
	return S_OK;
}

_int CEffectUI::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	m_fTime += fTimeDelta;

	if (m_fTime > 1.5f)
	{
		CUIManager::GetInstance()->Set_RenderEffect(false);

		return 0;
	}
	else
	{

		float fTime = fmodf(m_fTime, 1.5f);

		float fSection = fTime / 0.5f;   // 0~4
		int idx = (int)fSection;
		float fLocalTime = fSection - idx; // 0~1

		D3DXCOLOR colors[3] =
		{
			D3DXCOLOR(1, 0, 0, 1), 
			D3DXCOLOR(0, 0, 1, 1), 
			D3DXCOLOR(0, 1, 0, 1)
			 
		};

		int nextIdx = (idx + 1) % 3;

		D3DXCOLOR curColor =
			colors[idx] * (1.0f - fLocalTime) +
			colors[nextIdx] * fLocalTime;

		m_pEffectText->Set_Color(curColor);

		m_pEffectText->Update_GameObject(fTimeDelta);
		CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	}

	return iExit;
}

void CEffectUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);	
	m_pEffectText->LateUpdate_GameObject(fTimeDelta);
}

void CEffectUI::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pTextureCom->Render_Texture();
	m_pBufferCom->Render_Buffer();
}

void CEffectUI::Rotate(ROTATION eType, const _float& fAngle)
{
	m_pTransformCom->Rotation(eType, fAngle);
}

void CEffectUI::SetPos(_vec3 _pos)
{
	m_pTransformCom->Set_Pos(_pos.x - WINCX * 0.5f, -_pos.y + WINCY * 0.5f, 0.f);
}

void CEffectUI::SetScale(_float fCX, _float fCY)
{
	m_pTransformCom->Set_Scale(fCX * 0.5f, fCY * 0.5f, 1.f);
}
