#include "pch.h"
#include "CPlayerPart.h"
#include "CAnimation.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"

#include "CPlayer.h"


CPlayerPart::CPlayerPart(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pAnimationCom(nullptr)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pStateCom(nullptr)
	, m_bRendering(true), m_fTime(0.f)
	, m_pPlayer(nullptr)
{

}

CPlayerPart::CPlayerPart(const CPlayerPart& rhs)
	: CGameObject(rhs)
	, m_pAnimationCom(nullptr)
	, m_pBufferCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pStateCom(nullptr)
	, m_bRendering(true), m_fTime(0.f)
	, m_pPlayer(nullptr)
{

}

CPlayerPart::~CPlayerPart()
{

}

HRESULT CPlayerPart::Add_Component()
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

	//StateComponent
	pComponent = m_pStateCom = dynamic_cast<Engine::CStateComponent*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_StateComponent"));

	m_pStateCom->SetOnwer(this);
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_StateComponent", pComponent });
	return S_OK;
}

void CPlayerPart::Free()
{
	CGameObject::Free();
}


HRESULT CPlayerPart::Ready_GameObject()
{
	return E_FAIL;
}

_int CPlayerPart::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CGameObject::Update_GameObject(fTimeDelta);

	return iExit;
}

void CPlayerPart::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);

}

void CPlayerPart::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pAnimationCom->Render_Animation();
	m_pBufferCom->Render_Buffer();
}

void CPlayerPart::SetParent(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
}

void CPlayerPart::SetPos(_vec3 _pos)
{
	if (!m_pTransformCom) return;
	m_pTransformCom->Set_Pos(_pos);
}