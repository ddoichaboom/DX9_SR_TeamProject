#include "pch.h"
#include "CTextUI.h"
#include "CPlusUI.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CFontMgr.h"
#include "CFontUI.h"

TextureSource CTextUI::m_textureSource =
{
	0, L"../Bin/Resource/Texture/UI/Text_BG.dds"
};

CTextUI::CTextUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CBaseUI(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pPlusUI(nullptr), m_pTimeFontUI(nullptr)
{

}

CTextUI::CTextUI(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
	: CBaseUI(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pPlusUI(nullptr), m_pTimeFontUI(nullptr)
{
	m_vPos = vPos;

}

CTextUI::CTextUI(const CTextUI& rhs)
	: CBaseUI(rhs)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pPlusUI(nullptr), m_pTimeFontUI(nullptr)
{

}

CTextUI::~CTextUI()
{
}

CTextUI* CTextUI::Create(PDIRECT3DDEVICE9 pGraphicDev)
{
	CTextUI* pTextBG = new CTextUI(pGraphicDev);
	if (!pTextBG) return nullptr;

	if (FAILED(pTextBG->Ready_GameObject()))
	{
		Safe_Release(pTextBG);
		MSG_BOX("Plus Create Failed");
		return nullptr;
	}
	return pTextBG;
}

CTextUI* CTextUI::Create(PDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
	CTextUI* pTextBG = new CTextUI(pGraphicDev, vPos);
	if (!pTextBG) return nullptr;

	if (FAILED(pTextBG->Ready_GameObject()))
	{
		Safe_Release(pTextBG);
		MSG_BOX("Plus Create Failed");
		return nullptr;
	}
	return pTextBG;
}

void CTextUI::Set_Text(const wstring& wDeadTime)
{
	m_pTimeFontUI->Set_Text(wDeadTime);
}

void CTextUI::Set_StartPos(const _vec3& vPos)
{
	m_vPos = vPos;
	SetPos(m_vPos);
	m_vStartPos = m_vPos;
	m_vEndPos = m_vPos;
	m_vEndPos.y += 30.f;
}

void CTextUI::Init()
{
	m_fTime = 0.f;
	m_pTextureCom->Change_Texture(0);
}

HRESULT CTextUI::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_TextBGTexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CTextUI::Free()
{
	Safe_Release(m_pPlusUI);
	Safe_Release(m_pTimeFontUI);
	CGameObject::Free();

}

HRESULT CTextUI::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_fSizeX = 150.f;
	m_fSizeY = 60.f;


	SetScale(m_fSizeX, m_fSizeY);
	m_pTextureCom->Change_Texture(0);



	m_pPlusUI = CPlusUI::Create(m_pGraphicDev, { -50.f,-5.f,0.f });
	if (nullptr == m_pPlusUI)
		return E_FAIL;

	m_pPlusUI->Set_Parent(this);	
	m_pPlusUI->Set_Projection(true);

	m_pTimeFontUI = CFontUI::Create(FONT_WORD, { 20.f,0.f,0.f }, { 80.f,60.f,0.f });
	if (nullptr == m_pTimeFontUI)
		return E_FAIL;

	m_pTimeFontUI->Set_Parent(this);
	m_pTimeFontUI->Set_Projection(true);

	return S_OK;
}

_int CTextUI::Update_GameObject(const _float& fTimeDelta)
{
	if (IsDead()) return RET_DEAD;
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	m_fTime += fTimeDelta;

	if (m_fTime > 1.f)
	{
		m_bDead = true;
		return RET_NONE;
	}
	else
	{
		D3DXVec3Lerp(&m_vPos, &m_vStartPos, &m_vEndPos, m_fTime);
		_matrix matView, matProj, matWorld;
		D3DVIEWPORT9 viewport;

		m_pGraphicDev->GetViewport(&viewport);
		m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
		m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
		D3DXMatrixIdentity(&matWorld);

		D3DXVec3Project(&m_vScreenPos,
			&m_vPos,
			&viewport,
			&matProj,
			&matView,
			&matWorld);

		SetPos(m_vScreenPos);

		m_pPlusUI->Update_GameObject(fTimeDelta);
		m_pTimeFontUI->Update_GameObject(fTimeDelta);
		CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	}

	return iExit;
}

void CTextUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pPlusUI->LateUpdate_GameObject(fTimeDelta);	
	m_pTimeFontUI->LateUpdate_GameObject(fTimeDelta);
}

void CTextUI::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pTextureCom->Render_Texture();
	m_pBufferCom->Render_Buffer();

	m_pPlusUI->Render_GameObject();
}

void CTextUI::Activate()
{
	CGameObject::Activate();
	Init();
}

void CTextUI::Deactivate()
{
	CGameObject::Deactivate();
}

void CTextUI::Rotate(ROTATION eType, const _float& fAngle)
{
	m_pTransformCom->Rotation(eType, fAngle);
}

void CTextUI::SetPos(_vec3 _pos)
{
	m_pTransformCom->Set_Pos(_pos.x - WINCX * 0.5f, -_pos.y + WINCY * 0.5f, 0.f);
}

void CTextUI::SetScale(_float fCX, _float fCY)
{
	m_pTransformCom->Set_Scale(fCX * 0.5f, fCY * 0.5f, 1.f);
}
