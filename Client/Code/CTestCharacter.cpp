#include "pch.h"
#include "CTestCharacter.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"


CTestCharacter::CTestCharacter(LPDIRECT3DDEVICE9 pGraphicDev)
	:CCharacter(pGraphicDev)
{
}

CTestCharacter::CTestCharacter(const CTestCharacter& rhs)
	:CCharacter(rhs)
{
}

CTestCharacter::~CTestCharacter()
{
}

HRESULT CTestCharacter::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;

	m_pAnimationCom->Change_Animation(0);
	m_pAnimationCom->PlayFromStart();

	return S_OK;
}

_int CTestCharacter::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CCharacter::Update_GameObject(fTimeDelta);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	return iExit;
}

void CTestCharacter::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Key_Input(fTimeDelta);
	CCharacter::LateUpdate_GameObject(fTimeDelta);
}

void CTestCharacter::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pAnimationCom->Render_Animation();
	m_pBufferCom->Render_Buffer();

	m_pAnimationCom->LateRender_Animation();
}

HRESULT CTestCharacter::Add_Component()
{
	if (FAILED(CCharacter::Add_Component())) return E_FAIL;
	Engine::CComponent* pComponent = nullptr;

	// Animation
	pComponent = m_pAnimationCom = dynamic_cast<Engine::CAnimation*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_TestAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });

	return S_OK;
}

CTestCharacter* CTestCharacter::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTestCharacter* pTest = new CTestCharacter(pGraphicDev);

	if (FAILED(pTest->Ready_GameObject()))
	{
		Safe_Release(pTest);
		MSG_BOX("Test Character Create Failed");
		return nullptr;
	}

	return pTest;
}

void CTestCharacter::Key_Input(const _float& fTimeDelta)
{
	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_RETURN) & 0x80)
	{
		m_pAnimationCom->ChangeNextAnimation();
	}
}

void CTestCharacter::Free()
{
	CCharacter::Free();
}
