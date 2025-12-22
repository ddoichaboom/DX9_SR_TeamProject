#include "pch.h"
#include "CMiddlePart.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"


CMiddlePart::CMiddlePart(LPDIRECT3DDEVICE9 pGraphicDev)
	:CCharacter(pGraphicDev)
{
}

CMiddlePart::CMiddlePart(const CMiddlePart& rhs)
	:CCharacter(rhs)
{
}

CMiddlePart::~CMiddlePart()
{
}

void CMiddlePart::Set_Animation()
{
	m_pAnimationCom->ChangeNextAnimation();
	m_pAnimationCom->PlayFromStart();
}

HRESULT CMiddlePart::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	m_pTransformCom->m_vScale = { 256.f, 256.f, 1.f };
	m_pAnimationCom->Change_Animation(1);

	return S_OK;
}

_int CMiddlePart::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CCharacter::Update_GameObject(fTimeDelta);


	// 위치 설정 
	m_pTransformCom->Set_Pos(0.f, _float(-WINCY / 2) + 200.f, 0.f);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
	return iExit;
}

void CMiddlePart::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CCharacter::LateUpdate_GameObject(fTimeDelta);
}

void CMiddlePart::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	
	m_pAnimationCom->Render_Animation();
	m_pBufferCom->Render_Buffer();
	m_pAnimationCom->LateRender_Animation();
}

HRESULT CMiddlePart::Add_Component()
{
	if (FAILED(CCharacter::Add_Component()))
		return E_FAIL;

	Engine::CComponent* pComponent = nullptr;

	// Animation
	pComponent = m_pAnimationCom = dynamic_cast<Engine::CAnimation*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_MiddleAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });

	return S_OK;
}

CMiddlePart* CMiddlePart::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMiddlePart* pTest = new CMiddlePart(pGraphicDev);

	if (FAILED(pTest->Ready_GameObject()))
	{
		Safe_Release(pTest);
		MSG_BOX("Middle Part Create Failed");
		return nullptr;
	}

	return pTest;
}

void CMiddlePart::Free()
{
	CCharacter::Free();
}
