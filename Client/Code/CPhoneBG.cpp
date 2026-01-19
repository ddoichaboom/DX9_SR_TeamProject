#include "pch.h"
#include "CPhoneBG.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

#include "CPhonePlayer.h"
#include "CHPUI.h"

TextureSource CPhoneBG::m_textureSource =
{
	0, L"../Bin/Resource/Texture/UI/PHONE_BACK.dds"
};

CPhoneBG::CPhoneBG(LPDIRECT3DDEVICE9 pGraphicDev)
    : CBaseUI(pGraphicDev)
	,	m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pPhonePlayer(nullptr), m_pHpUI(nullptr)
{
	m_eOBJ_ID = OBJ_ITEM;
}

CPhoneBG::CPhoneBG(const CPhoneBG& rhs)
    : CBaseUI(rhs)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pPhonePlayer(nullptr), m_pHpUI(nullptr)
{
	m_eOBJ_ID = OBJ_ITEM;
}

CPhoneBG::~CPhoneBG()
{
}

CPhoneBG* CPhoneBG::Create(PDIRECT3DDEVICE9 pGraphicDev)
{
	CPhoneBG* pBG = new CPhoneBG(pGraphicDev);
	if (!pBG) return nullptr;

	if (FAILED(pBG->Ready_GameObject()))
	{
		Safe_Release(pBG);
		MSG_BOX("Phone BackGround Create Failed");
		return nullptr;
	}
	return pBG;
}

HRESULT CPhoneBG::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_PhoneBGTexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

void CPhoneBG::Free()
{
	Safe_Release(m_pPhonePlayer);
	Safe_Release(m_pHpUI);
	CGameObject::Free();
}

HRESULT CPhoneBG::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_fX = 360.f;
	m_fY = WINCY - 190.f;

	m_fSizeX = 230.f;
	m_fSizeY = 220.f;

	//Rotate(ROT_Z, 15.f);
	SetScale(m_fSizeX, m_fSizeY);
	SetPos({ m_fX, m_fY, 0.f });
	
	m_pTextureCom->Change_Texture(0);

	m_pPhonePlayer = CPhonePlayer::Create(m_pGraphicDev);

	if (nullptr == m_pPhonePlayer)
		return E_FAIL;

	m_pHpUI = CHPUI::Create(m_pGraphicDev);

	if (nullptr == m_pHpUI)
		return E_FAIL;

    return S_OK;
}

_int CPhoneBG::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	m_pPhonePlayer->Update_GameObject(fTimeDelta);
	m_pHpUI->Update_GameObject(fTimeDelta);

    return iExit;
}

void CPhoneBG::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);

	m_pPhonePlayer->LateUpdate_GameObject(fTimeDelta);
	m_pHpUI->LateUpdate_GameObject(fTimeDelta);
}

void CPhoneBG::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pTextureCom->Render_Texture();
	m_pBufferCom->Render_Buffer();
}

void CPhoneBG::Rotate(ROTATION eType, const _float& fAngle)
{
	m_pTransformCom->Rotation(eType, fAngle);
}

void CPhoneBG::SetPos(_vec3 _pos)
{
	m_pTransformCom->Set_Pos(_pos.x - WINCX * 0.5f, -_pos.y + WINCY * 0.5f, 0.f);
}

void CPhoneBG::SetScale(_float fCX, _float fCY)
{
	m_pTransformCom->Set_Scale(fCX * 0.5f, fCY * 0.5f, 1.f);
}
