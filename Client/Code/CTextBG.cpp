#include "pch.h"
#include "CTextBG.h"
#include "CPlusUI.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CFontMgr.h"
#include "CFontUI.h"

TextureSource CTextBG::m_textureSource =
{
	0, L"../Bin/Resource/Texture/UI/Text_BG.dds"
};

CTextBG::CTextBG(LPDIRECT3DDEVICE9 pGraphicDev)
	: CBaseUI(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pPlusUI(nullptr) , m_pDeadFontUI(nullptr) , m_pTimeFontUI(nullptr)	 
{
	
}

CTextBG::CTextBG(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
	: CBaseUI(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pPlusUI(nullptr), m_pDeadFontUI(nullptr), m_pTimeFontUI(nullptr)
{
	m_vPos = vPos;
	
}

CTextBG::CTextBG(const CTextBG& rhs)
	: CBaseUI(rhs)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pPlusUI(nullptr), m_pDeadFontUI(nullptr), m_pTimeFontUI(nullptr)
{
	
}

CTextBG::~CTextBG()
{
}

CTextBG* CTextBG::Create(PDIRECT3DDEVICE9 pGraphicDev)
{
	CTextBG* pTextBG = new CTextBG(pGraphicDev);
	if (!pTextBG) return nullptr;

	if (FAILED(pTextBG->Ready_GameObject()))
	{
		Safe_Release(pTextBG);
		MSG_BOX("Plus Create Failed");
		return nullptr;
	}
	return pTextBG;
}

CTextBG* CTextBG::Create(PDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
	CTextBG* pTextBG = new CTextBG(pGraphicDev,vPos);
	if (!pTextBG) return nullptr;

	if (FAILED(pTextBG->Ready_GameObject()))
	{
		Safe_Release(pTextBG);
		MSG_BOX("Plus Create Failed");
		return nullptr;
	}
	return pTextBG;
}

void CTextBG::Set_Text(const wstring& wDeadText, const wstring& wDeadTime)
{
	m_pDeadFontUI->Set_Text(wDeadText);
	m_pTimeFontUI->Set_Text(wDeadTime);
}

void CTextBG::Init()
{
	m_vPos = m_vStartPos;
	m_fTime = 0.f;
}

HRESULT CTextBG::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_TextBGTexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CTextBG::Free()
{	
	Safe_Release(m_pPlusUI);
	Safe_Release(m_pDeadFontUI);
	Safe_Release(m_pTimeFontUI);
	CGameObject::Free();
	
}

HRESULT CTextBG::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_fSizeX = 280.f;
	m_fSizeY = 60.f;
	
	SetPos(m_vPos);
	SetScale(m_fSizeX, m_fSizeY);
	m_pTextureCom->Change_Texture(0);

	m_vStartPos = m_vPos;
	m_vEndPos = m_vPos;
	m_vEndPos.y += 200.f;
		
	_vec3 vZero = {0.f,0.f,0.f};
	_vec3 vLeft = {-10.f,0.f,0.f};
	_vec3 vRight = {50.f,0.f,0.f};

	m_pPlusUI = CPlusUI::Create(m_pGraphicDev, vLeft);
	if (nullptr == m_pPlusUI)
		return E_FAIL;

	m_pPlusUI->Set_Parent(this);

	m_pDeadFontUI = CFontUI::Create(FONT_DEFAULT, { -150.f,0.f,0.f }, vZero);
	if (nullptr == m_pDeadFontUI)
		return E_FAIL;

	m_pDeadFontUI->Set_Parent(this);

	m_pTimeFontUI = CFontUI::Create(FONT_DEFAULT, { 40.f,0.f,0.f }, vZero);
	if (nullptr == m_pTimeFontUI)
		return E_FAIL;

	m_pTimeFontUI->Set_Parent(this);

	return S_OK;
}

_int CTextBG::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);
	
	m_fTime += fTimeDelta;

	
	if (m_fTime > 1.f)
	{
		m_fTime = 0.f;
		return -1;
	}
	else
	{
		D3DXVec3Lerp(&m_vPos, &m_vStartPos, &m_vEndPos, m_fTime);
		SetPos(m_vPos);
		CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	}

	m_pPlusUI->Update_GameObject(fTimeDelta);
	m_pDeadFontUI->Update_GameObject(fTimeDelta);
	m_pTimeFontUI->Update_GameObject(fTimeDelta);

	return iExit;
}

void CTextBG::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pPlusUI->LateUpdate_GameObject(fTimeDelta);
	m_pDeadFontUI->LateUpdate_GameObject(fTimeDelta);
	m_pTimeFontUI->LateUpdate_GameObject(fTimeDelta);
}

void CTextBG::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pTextureCom->Render_Texture();
	m_pBufferCom->Render_Buffer();

	m_pPlusUI->Render_GameObject();
}

void CTextBG::Rotate(ROTATION eType, const _float& fAngle)
{
	m_pTransformCom->Rotation(eType, fAngle);
}

void CTextBG::SetPos(_vec3 _pos)
{
	m_pTransformCom->Set_Pos(_pos.x - WINCX * 0.5f, -_pos.y + WINCY * 0.5f, 0.f);
}

void CTextBG::SetScale(_float fCX, _float fCY)
{
	m_pTransformCom->Set_Scale(fCX * 0.5f, fCY * 0.5f, 1.f);
}
