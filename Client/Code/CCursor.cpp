#include "pch.h"
#include "CCursor.h"
#include "CProtoMgr.h"
#include "CRenderer.h"


TextureSource CCursor::m_textureSource =
{
	0, L"../Bin/Resource/Texture/UI/Cursor.dds"
};

CCursor::CCursor(LPDIRECT3DDEVICE9 pGraphicDev)
	: CBaseUI(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
{
	
}

CCursor::CCursor(const CCursor& rhs)
	: CBaseUI(rhs)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
{
	
}

CCursor::~CCursor()
{
}

CCursor* CCursor::Create(PDIRECT3DDEVICE9 pGraphicDev)
{
	CCursor* pBG = new CCursor(pGraphicDev);
	if (!pBG) return nullptr;

	if (FAILED(pBG->Ready_GameObject()))
	{
		Safe_Release(pBG);
		MSG_BOX("Cursor Create Failed");
		return nullptr;
	}
	return pBG;
}

HRESULT CCursor::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CursorTexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CCursor::Free()
{
	CGameObject::Free();
}

HRESULT CCursor::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_fSizeX = 50.f;
	m_fSizeY = 50.f;
	
	SetScale(m_fSizeX, m_fSizeY);	

	m_pTextureCom->Change_Texture(0);

	return S_OK;
}

_int CCursor::Update_GameObject(const _float& fTimeDelta)
{
	if (IsDead()) return RET_DEAD;
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);
	
	return iExit;
}

void CCursor::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	POINT pt{};
	GetCursorPos(&pt); 
	ScreenToClient(g_hWnd, &pt);
	_vec3 vPos = { (_float)pt.x,(_float)pt.y,0.f };

	SetPos(vPos);
	
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
}

void CCursor::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pTextureCom->Render_Texture();
	m_pBufferCom->Render_Buffer();
}

void CCursor::Rotate(ROTATION eType, const _float& fAngle)
{
	m_pTransformCom->Rotation(eType, fAngle);
}

void CCursor::SetPos(_vec3 _pos)
{
	m_pTransformCom->Set_Pos(_pos.x - WINCX * 0.5f, -_pos.y + WINCY * 0.5f, 0.f);
}

void CCursor::SetScale(_float fCX, _float fCY)
{
	m_pTransformCom->Set_Scale(fCX * 0.5f, fCY * 0.5f, 1.f);
}
