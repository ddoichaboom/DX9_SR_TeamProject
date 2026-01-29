#include "pch.h"
#include "CTargetUI.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CSniperPlayer.h"
#include "CManagement.h"


TextureSource CTargetUI::m_textureSource =
{
	0, L"../Bin/Resource/Texture/Sniper/SNIPER_ICON_256.dds"
};

CTargetUI::CTargetUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CBaseUI(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr), m_pPlayer(nullptr)
	, m_fTime(0.f), m_bInit(false)
{
	
}

CTargetUI::CTargetUI(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY)
	: CBaseUI(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr), m_pPlayer(nullptr)
	, m_fTime(0.f), m_bInit(false)
{
	m_fX = fX;
	m_fY = fY;
	
}

CTargetUI::CTargetUI(const CTargetUI& rhs)
	: CBaseUI(rhs)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr), m_pPlayer(nullptr)
	, m_fTime(0.f), m_bInit(false)
{
	
}

CTargetUI::~CTargetUI()
{
}

CTargetUI* CTargetUI::Create(PDIRECT3DDEVICE9 pGraphicDev)
{
	CTargetUI* pTarget = new CTargetUI(pGraphicDev);
	if (!pTarget) return nullptr;

	if (FAILED(pTarget->Ready_GameObject()))
	{
		Safe_Release(pTarget);
		MSG_BOX("Target UI Create Failed");
		return nullptr;
	}
	return pTarget;
}

CTargetUI* CTargetUI::Create(PDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY)
{
	CTargetUI* pTarget = new CTargetUI(pGraphicDev, fX, fY);
	if (!pTarget) return nullptr;

	if (FAILED(pTarget->Ready_GameObject()))
	{
		Safe_Release(pTarget);
		MSG_BOX("Target UI Create Failed");
		return nullptr;
	}
	return pTarget;
}

HRESULT CTargetUI::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_TargetUITexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CTargetUI::Free()
{
	CGameObject::Free();
}

HRESULT CTargetUI::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_fSizeX = 80.f;
	m_fSizeY = 80.f;

	m_vScale = { m_fSizeX, m_fSizeY, 1.f };
	m_vEndScale = m_vScale;
	m_vStartScale = m_vEndScale * 2.5f;

	
	SetPos({ m_fX, m_fY, 0.f });

	Activate();
	return S_OK;
}

_int CTargetUI::Update_GameObject(const _float& fTimeDelta)
{
	if (m_pPlayer == nullptr)
	{
		CGameObject* pGameObj = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer")->Get_Object(OBJ_PLAYER);
		if (pGameObj != nullptr)
		{
			m_pPlayer = static_cast<CSniperPlayer*>(pGameObj);
		}					
	}

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);	

	if (m_bInit)
	{
		m_fTime += fTimeDelta;

		_float fTime = m_fTime * 0.5f;
		_vec3 vScale;

		if (fTime > 1.f)
		{
			m_bInit = false;
			SetScale(m_vScale);
		}
		else
		{
			D3DXVec3Lerp(&vScale, &m_vStartScale, &m_vEndScale, fTime);
			SetScale(vScale);
		}
	}

	if (m_pPlayer->Get_IsAimState() == false)
	{
		CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	}

	return iExit;
}

void CTargetUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CTargetUI::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pTextureCom->Render_Texture();
	m_pBufferCom->Render_Buffer();
}

void CTargetUI::Activate()
{
	CGameObject::Activate();
	m_fTime = 0.f;
	m_bInit = true;
	m_vScale = m_vEndScale;
	m_pTextureCom->Change_Texture(0);
}

void CTargetUI::Deactivate()
{
	CGameObject::Deactivate();
}

void CTargetUI::Rotate(ROTATION eType, const _float& fAngle)
{
	m_pTransformCom->Rotation(eType, fAngle);
}

void CTargetUI::SetPos(_vec3 _pos)
{
	m_pTransformCom->Set_Pos(_pos.x - WINCX * 0.5f, -_pos.y + WINCY * 0.5f, 0.f);
}

void CTargetUI::SetScale(_float fCX, _float fCY)
{
	m_pTransformCom->Set_Scale(fCX * 0.5f, fCY * 0.5f, 1.f);
}

void CTargetUI::SetScale(_vec3 vScale)
{
	m_pTransformCom->Set_Scale(vScale.x * 0.5f, vScale.y * 0.5f, 1.f);
}
