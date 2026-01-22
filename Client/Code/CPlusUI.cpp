#include "pch.h"
#include "CPlusUI.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

TextureSource CPlusUI::m_textureSource =
{
    0, L"../Bin/Resource/Texture/UI/UI_Heal.dds"
};

CPlusUI::CPlusUI(LPDIRECT3DDEVICE9 pGraphicDev)
    : CBaseUI(pGraphicDev)
    , m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr), m_pParentUI(nullptr)
	, m_bProject(false)
{
	
}

CPlusUI::CPlusUI(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
	: CBaseUI(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr), m_pParentUI(nullptr)
	, m_vLocalPos(vPos), m_bProject(false)
{
	
}

CPlusUI::CPlusUI(const CPlusUI& rhs)
    : CBaseUI(rhs)
    , m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr), m_pParentUI(nullptr)
	, m_bProject(false)
{
}

CPlusUI::~CPlusUI()
{
}

CPlusUI* CPlusUI::Create(PDIRECT3DDEVICE9 pGraphicDev)
{
    CPlusUI* pPlus = new CPlusUI(pGraphicDev);
    if (!pPlus) return nullptr;

    if (FAILED(pPlus->Ready_GameObject()))
    {
        Safe_Release(pPlus);
        MSG_BOX("Plus Create Failed");
        return nullptr;
    }
    return pPlus;
}

CPlusUI* CPlusUI::Create(PDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
	CPlusUI* pPlus = new CPlusUI(pGraphicDev, vPos);
	if (!pPlus) return nullptr;

	if (FAILED(pPlus->Ready_GameObject()))
	{
		Safe_Release(pPlus);
		MSG_BOX("Plus Create Failed");
		return nullptr;
	}
	return pPlus;
}

void CPlusUI::Set_Parent(CBaseUI* pParent)
{
	m_pParentUI = pParent;
}



HRESULT CPlusUI::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_PlusTexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CPlusUI::Free()
{
	CGameObject::Free();
}

HRESULT CPlusUI::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_fSizeX = 40.f;
	m_fSizeY = 40.f;
	SetScale(m_fSizeX, m_fSizeY);
	m_pTextureCom->Change_Texture(0);
	return S_OK;
}

_int CPlusUI::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);
	

	return iExit;
}

void CPlusUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);

	if (m_pParentUI == nullptr)
		return;

	if (false == m_bProject)
	{
		_vec3 vParentPos = m_pParentUI->Get_Pos();
		vParentPos += m_vLocalPos;
		SetPos(vParentPos);
	}
	else
	{
		_vec3 vParentPos = m_pParentUI->Get_ScreenPos();
		vParentPos += m_vLocalPos;
		SetPos(vParentPos);
	}

	
}

void CPlusUI::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pTextureCom->Render_Texture();
	m_pBufferCom->Render_Buffer();
}

void CPlusUI::Rotate(ROTATION eType, const _float& fAngle)
{
	m_pTransformCom->Rotation(eType, fAngle);
}

void CPlusUI::SetPos(_vec3 _pos)
{
	m_pTransformCom->Set_Pos(_pos.x - WINCX * 0.5f, -_pos.y + WINCY * 0.5f, 0.f);
}

void CPlusUI::SetScale(_float fCX, _float fCY)
{
	m_pTransformCom->Set_Scale(fCX * 0.5f, fCY * 0.5f, 1.f);
}
