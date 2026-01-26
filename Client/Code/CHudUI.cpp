#include "pch.h"
#include "CHudUI.h"
#include "CProtoMgr.h"
#include "CRenderer.h"


TextureSource CHudUI::m_textureSource =
{
	0, L"../Bin/Resource/Texture/UI/Hud_Axe.dds"
};

CHudUI::CHudUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CBaseUI(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_fTime(0.f), m_fInterval(0.45f), m_bSizeLerp(false)
{
	
}

CHudUI::CHudUI(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY)
	: CBaseUI(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_fTime(0.f), m_fInterval(0.45f), m_bSizeLerp(false)
{
	m_fX = fX;
	m_fY = fY;

}

CHudUI::CHudUI(const CHudUI& rhs)
	: CBaseUI(rhs)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_fTime(0.f), m_fInterval(0.45f), m_bSizeLerp(false)
{
	
}

CHudUI::~CHudUI()
{
}

CHudUI* CHudUI::Create(PDIRECT3DDEVICE9 pGraphicDev)
{
	CHudUI* pItem = new CHudUI(pGraphicDev);
	if (!pItem) return nullptr;

	if (FAILED(pItem->Ready_GameObject()))
	{
		Safe_Release(pItem);
		MSG_BOX("HUD Item Create Failed");
		return nullptr;
	}
	return pItem;
}

CHudUI* CHudUI::Create(PDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY)
{
	CHudUI* pItem = new CHudUI(pGraphicDev, fX, fY);
	if (!pItem) return nullptr;

	if (FAILED(pItem->Ready_GameObject()))
	{
		Safe_Release(pItem);
		MSG_BOX("HUD Item Create Failed");
		return nullptr;
	}
	return pItem;
}

HRESULT CHudUI::Add_Component()
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

	pComponent = m_pTextureCom = static_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_HudUITexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CHudUI::Free()
{
	CGameObject::Free();
}

HRESULT CHudUI::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_fSizeX = 40.f;
	m_fSizeY = 20.f;

	m_vStartScale = { m_fSizeX * 1.5f, m_fSizeY * 1.5f , 1.f };
	m_vEndScale = { m_fSizeX, m_fSizeY , 1.f };


	SetScale(m_vStartScale);
	SetPos({ m_fX, m_fY, 0.f });

	m_pTextureCom->Change_Texture(0);
	return S_OK;
}

_int CHudUI::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	_vec3 vPos;
	_vec3 vScale;
	_float fTime;
	m_fTime += fTimeDelta;	

	if (m_bSizeLerp == false)
	{
		fTime = m_fTime * 2.f;
		if (fTime < 1.f)
		{		
			D3DXVec3Lerp(&vScale, &m_vStartScale, &m_vEndScale, fTime);
			SetScale(vScale);	
		}
		else
		{
			m_bSizeLerp = true;
			m_fTime = 0.f;
			SetScale(m_vEndScale);
		}
		CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
		return iExit;
	}
	else 
	{
		if (m_fTime < m_fInterval)
		{
			CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
		}
		else if (m_fTime > m_fInterval *2.f)
		{
			m_fTime = 0.f;
			
		}

	}

	

	return iExit;
}

void CHudUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CHudUI::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pTextureCom->Render_Texture();
	m_pBufferCom->Render_Buffer();
}

void CHudUI::Activate()
{
	CGameObject::Activate();
	m_fTime = 0.f;
	//m_fInterval = 0.f;
	m_bSizeLerp = false;

	SetScale(m_vStartScale);
	SetPos({ m_fX, m_fY, 0.f });
	m_pTextureCom->Change_Texture(0);
}

void CHudUI::Deactivate()
{
	CGameObject::Deactivate();
}

void CHudUI::Rotate(ROTATION eType, const _float& fAngle)
{
	m_pTransformCom->Rotation(eType, fAngle);
}

void CHudUI::SetPos(_vec3 _pos)
{
	m_pTransformCom->Set_Pos(_pos.x - WINCX * 0.5f, -_pos.y + WINCY * 0.5f, 0.f);
}

void CHudUI::SetScale(_float fCX, _float fCY)
{
	m_pTransformCom->Set_Scale(fCX * 0.5f, fCY * 0.5f, 1.f);
}

void CHudUI::SetScale(_vec3 vScale)
{	
	m_pTransformCom->Set_Scale(vScale.x , vScale.y , 1.f);
}
