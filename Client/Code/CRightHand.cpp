#include "pch.h"
#include "CRightHand.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"


CRightHand::CRightHand(LPDIRECT3DDEVICE9 pGraphicDev)
	:CCharacter(pGraphicDev)
{
}

CRightHand::CRightHand(const CRightHand& rhs)
	:CCharacter(rhs)
{
}

CRightHand::~CRightHand()
{
}


void CRightHand::Set_Animation()
{
	m_pAnimationCom->ChangeNextAnimation();
	m_pAnimationCom->PlayFromStart();
}

HRESULT CRightHand::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	m_pTransformCom->m_vScale = { 256.f, 256.f, 1.f };
	m_pAnimationCom->Change_Animation(2);

	m_pTransformCom->Set_Pos(_float(WINCX / 2) - 200.f, _float(-WINCY / 2) + 200.f, 0.f);

	return S_OK;
}

_int CRightHand::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CCharacter::Update_GameObject(fTimeDelta);


	// 위치 설정 
	

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
	return iExit;
}

void CRightHand::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CCharacter::LateUpdate_GameObject(fTimeDelta);
}

void CRightHand::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pAnimationCom->Render_Animation();
	m_pBufferCom->Render_Buffer();
	m_pAnimationCom->LateRender_Animation();

}

HRESULT CRightHand::Add_Component()
{
	if (FAILED(CCharacter::Add_Component()))
		return E_FAIL;

	Engine::CComponent* pComponent = nullptr;

	// Animation
	pComponent = m_pAnimationCom = dynamic_cast<Engine::CAnimation*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RightAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });

	return S_OK;
}

CRightHand* CRightHand::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRightHand* pTest = new CRightHand(pGraphicDev);

	if (FAILED(pTest->Ready_GameObject()))
	{
		Safe_Release(pTest);
		MSG_BOX("Right Hand Create Failed");
		return nullptr;
	}

	return pTest;
}

void CRightHand::Free()
{
	CCharacter::Free();
}
