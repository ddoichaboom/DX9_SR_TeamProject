#include "pch.h"
#include "CLoadingBar.h"
#include "CRcColUp.h"
#include "CTransform.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CLoadingBar::CLoadingBar(LPDIRECT3DDEVICE9 pGraphicDev, D3DXCOLOR _color)
	: CGameObject(pGraphicDev), m_pBufferCom(nullptr), m_pTransformCom(nullptr),m_color(_color)
{
	m_eOBJ_ID = OBJ_EFFECT;
	m_iID = Make_ID();
}

CLoadingBar::~CLoadingBar()
{
}

HRESULT CLoadingBar::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;

	m_pTransformCom->Set_Scale(m_vScale);
	m_pTransformCom->Set_Pos(-WINCX*0.25f, -WINCY * 0.4f, 0.f);
	m_pTransformCom->Update_Component(1.f);
	return S_OK;
}

_int CLoadingBar::Update_GameObject(const _float& fTimeDelta)
{
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	if (m_bLerp)
	{
		m_fCurPerecent += fTimeDelta;
		if (m_fCurPerecent>= m_fDestPerecent)
		{
			m_fCurPerecent = m_fDestPerecent;
			m_bLerp = false;
		}

		m_vScale.x = m_fLength * m_fCurPerecent;
		m_pTransformCom->Set_Scale(m_vScale);
		m_pTransformCom->Update_Component(1.f);
	}
	return 0;
}


void CLoadingBar::Render_GameObject()
{
	m_pGraphicDev->SetTexture(0, nullptr);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pBufferCom->Render_Buffer();

	if (m_bLerpRender && !m_bLerp) m_bLerpRender = false;
}


HRESULT CLoadingBar::Add_Component()
{
	//VIBuffer
	//pComponent = m_pBufferCom = dynamic_cast<Engine::CRcColSide*>
	//	(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcColUp"));
	
	//색 지정을 위해서 Proto 복제 하지 않음 
	m_pBufferCom = CRcColSide::Create(m_pGraphicDev, m_color);
	if (nullptr == m_pBufferCom) return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", m_pBufferCom });

	// Transform
	Engine::CComponent*  pComponent = m_pTransformCom = static_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Transform", pComponent });
	return S_OK;
}

void CLoadingBar::SetPercent(_float _percent)
{
	if (_percent >= 1.f) _percent = 1.f;

	m_fDestPerecent = _percent;
	m_bLerp = true;
	m_bLerpRender = true;
}

bool CLoadingBar::IsBarEnd()
{
	return (m_fCurPerecent >= 1.f && !m_bLerp && !m_bLerpRender);
}

CLoadingBar* CLoadingBar::Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXCOLOR _color)
{
	CLoadingBar* pBar = new CLoadingBar(pGraphicDev, _color);

	if (FAILED(pBar->Ready_GameObject()))
	{
		Safe_Release(pBar);
		MSG_BOX("Loading Bar Create Failed");
		return nullptr;
	}

	return pBar;
}


void CLoadingBar::Free()
{
	CGameObject::Free();
}