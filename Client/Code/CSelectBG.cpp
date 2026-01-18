#include "pch.h"
#include "CSelectBG.h"
#include "CProtoMgr.h"
#include "CRenderer.h"


TextureSource CSelectBG::m_textureSource =
{
	0, L"../Bin/Resource/Texture/UI/Select_Item.dds"
};

CSelectBG::CSelectBG(LPDIRECT3DDEVICE9 pGraphicDev)
	: CBaseUI(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
{
}

CSelectBG::CSelectBG(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY)
	: CBaseUI(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
{
	m_fX = fX;
	m_fY = fY;
}

CSelectBG::CSelectBG(const CSelectBG& rhs)
	: CBaseUI(rhs)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
{
}

CSelectBG::~CSelectBG()
{
}

CSelectBG* CSelectBG::Create(PDIRECT3DDEVICE9 pGraphicDev)
{
	CSelectBG* pBG = new CSelectBG(pGraphicDev);
	if (!pBG) return nullptr;

	if (FAILED(pBG->Ready_GameObject()))
	{
		Safe_Release(pBG);
		MSG_BOX("Select BackGround Create Failed");
		return nullptr;
	}
	return pBG;
}

CSelectBG* CSelectBG::Create(PDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY)
{
	CSelectBG* pBG = new CSelectBG(pGraphicDev,fX,fY);
	if (!pBG) return nullptr;

	if (FAILED(pBG->Ready_GameObject()))
	{
		Safe_Release(pBG);
		MSG_BOX("Select BackGround Create Failed");
		return nullptr;
	}
	return pBG;
}

HRESULT CSelectBG::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SelectBGTexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CSelectBG::Free()
{
	CGameObject::Free();
}

HRESULT CSelectBG::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_fSizeX = 115.f;
	m_fSizeY = 225.f;

	SetScale(m_fSizeX, m_fSizeY);
	SetPos({ m_fX, m_fY, 0.f });

	m_pTextureCom->Change_Texture(0);
	return S_OK;
}

_int CSelectBG::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);

	return iExit;
}

void CSelectBG::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CSelectBG::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pTextureCom->Render_Texture();
	m_pBufferCom->Render_Buffer();
}

void CSelectBG::Rotate(ROTATION eType, const _float& fAngle)
{
	m_pTransformCom->Rotation(eType, fAngle);
}

void CSelectBG::SetPos(_vec3 _pos)
{
	m_pTransformCom->Set_Pos(_pos.x - WINCX * 0.5f, -_pos.y + WINCY * 0.5f, 0.f);
}

void CSelectBG::SetScale(_float fCX, _float fCY)
{
	m_pTransformCom->Set_Scale(fCX * 0.5f, fCY * 0.5f, 1.f);
}
