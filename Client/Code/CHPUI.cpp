#include "pch.h"
#include "CHPUI.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CFontMgr.h"
#include "CManagement.h"

#include "CPlayer.h"
#include "CFontUI.h"

TextureSource CHPUI::m_textureSource =
{
	0, L"../Bin/Resource/Texture/UI/HP_BG.dds"
};

CHPUI::CHPUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CBaseUI(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pPlayer(nullptr), m_pFontUI(nullptr), m_bBossStage(true)
{
}

CHPUI::CHPUI(const CHPUI& rhs)
	: CBaseUI(rhs)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pPlayer(nullptr), m_pFontUI(nullptr), m_bBossStage(true)
{

}

CHPUI::~CHPUI()
{
}

CHPUI* CHPUI::Create(PDIRECT3DDEVICE9 pGraphicDev)
{
	CHPUI* pBG = new CHPUI(pGraphicDev);
	if (!pBG) return nullptr;

	if (FAILED(pBG->Ready_GameObject()))
	{
		Safe_Release(pBG);
		MSG_BOX("Select BackGround Create Failed");
		return nullptr;
	}
	return pBG;
}

HRESULT CHPUI::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_HPUITexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CHPUI::Free()
{
	Safe_Release(m_pFontUI);
	CGameObject::Free();
}

HRESULT CHPUI::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_fX = 310.f;
	m_fY = WINCY - 150.f;
	m_fSizeX = 120.f;
	m_fSizeY = 120.f;

	m_vPos = { 310.f, WINCY - 150.f ,0 };
	m_vSize = { m_fSizeX, m_fSizeY, 0.f };
	
	SetScale(m_fSizeX, m_fSizeY);
	SetPos(m_vPos);



	m_pTextureCom->Change_Texture(0);

	m_pFontUI = CFontUI::Create(FONT_NUMBER, { 0.f, 0.f, 0.f }, m_vSize);
	if (nullptr == m_pFontUI)
		return E_FAIL;

	m_pFontUI->Set_Parent(this);
	m_pFontUI->Set_Color(D3DXCOLOR(0.f, 0.f, 0.f, 1.f));
	
	return S_OK;
}

_int CHPUI::Update_GameObject(const _float& fTimeDelta)
{
	if (m_pPlayer == nullptr)
	{
		CGameObject* player = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer")->Get_Object(OBJ_PLAYER);
		m_pPlayer = static_cast<CPlayer*>(player);
		Check_Stage();
	}

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	wstring wHP;
	
	if (m_bBossStage)
	{
		wHP = m_pPlayer->Get_HPPercent();
	}
	else
	{
		wHP = m_pPlayer->Get_HPText();		
	}
	
	m_pFontUI->Set_Text(wHP);
	
	return iExit;
}

void CHPUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pFontUI->LateUpdate_GameObject(fTimeDelta);
}

void CHPUI::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pTextureCom->Render_Texture();
	m_pBufferCom->Render_Buffer();
}

void CHPUI::Rotate(ROTATION eType, const _float& fAngle)
{
	m_pTransformCom->Rotation(eType, fAngle);
}

void CHPUI::SetPos(_vec3 _pos)
{
	m_pTransformCom->Set_Pos(_pos.x - WINCX * 0.5f, -_pos.y + WINCY * 0.5f, 0.f);
}

void CHPUI::SetScale(_float fCX, _float fCY)
{
	m_pTransformCom->Set_Scale(fCX * 0.5f, fCY * 0.5f, 1.f);
}

void CHPUI::Check_Stage()
{
	if (SCENE_BOSS == CManagement::GetInstance()->Get_CurrSceneType())
	{
		m_bBossStage = true;		
		m_pFontUI->Set_FontType(FONT_SMALLNUMBER);
	}		
	else
	{
		m_bBossStage = false;
		m_pFontUI->Set_FontType(FONT_NUMBER);
	}
	m_pPlayer->Set_BossStage(m_bBossStage);
}
