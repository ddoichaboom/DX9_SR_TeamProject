#include "pch.h"
#include "CSlotUI.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

#include "CHudUI.h"


TextureSource CSlotUI::m_textureSource =
{
	0, L"../Bin/Resource/Texture/UI/Slot_TakeDown.dds"
};

CSlotUI::CSlotUI(LPDIRECT3DDEVICE9 pGraphicDev)
	: CBaseUI(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_fTime(0.f), m_pHudUI(nullptr)
{
	
}

CSlotUI::CSlotUI(const CSlotUI& rhs)
	: CBaseUI(rhs)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_fTime(0.f), m_pHudUI(nullptr)
{
	
}

CSlotUI::~CSlotUI()
{
}

CSlotUI* CSlotUI::Create(PDIRECT3DDEVICE9 pGraphicDev)
{
	CSlotUI* pSlotUI = new CSlotUI(pGraphicDev);
	if (!pSlotUI) return nullptr;

	if (FAILED(pSlotUI->Ready_GameObject()))
	{
		Safe_Release(pSlotUI);
		MSG_BOX("Slot UI Create Failed");
		return nullptr;
	}
	return pSlotUI;
}

HRESULT CSlotUI::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SlotUITexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CSlotUI::Free()
{
	Safe_Release(m_pHudUI);
	CGameObject::Free();
}

HRESULT CSlotUI::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_fX = WINCX * 0.5f;
	m_fY = WINCY - 128.f;

	m_fSizeX = 220.f;
	m_fSizeY = 220.f;

	SetScale(m_fSizeX, m_fSizeY);
	SetPos({ m_fX, m_fY, 0.f });

	m_pTextureCom->Change_Texture(0);


	m_pHudUI = CHudUI::Create(m_pGraphicDev, m_fX+5.f, m_fY - 40.f);
	if (nullptr == m_pHudUI)
		return E_FAIL;

	return S_OK;
}

_int CSlotUI::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	m_pHudUI->Update_GameObject(fTimeDelta);

	return iExit;
}

void CSlotUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);

	m_pHudUI->LateUpdate_GameObject(fTimeDelta);
}

void CSlotUI::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pTextureCom->Render_Texture();
	m_pBufferCom->Render_Buffer();
}

void CSlotUI::Activate()
{
	CGameObject::Activate();
	m_fTime = 0.f;
	m_pTextureCom->Change_Texture(0);
	m_pHudUI->Activate();
}

void CSlotUI::Deactivate()
{
	CGameObject::Deactivate();
}

void CSlotUI::Rotate(ROTATION eType, const _float& fAngle)
{
	m_pTransformCom->Rotation(eType, fAngle);
}

void CSlotUI::SetPos(_vec3 _pos)
{
	m_pTransformCom->Set_Pos(_pos.x - WINCX * 0.5f, -_pos.y + WINCY * 0.5f, 0.f);
}

void CSlotUI::SetScale(_float fCX, _float fCY)
{
	m_pTransformCom->Set_Scale(fCX * 0.5f, fCY * 0.5f, 1.f);
}
