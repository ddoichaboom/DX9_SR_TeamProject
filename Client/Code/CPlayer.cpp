#include "pch.h"
#include "CPlayer.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"

#include "CManagement.h"
#include "CLeftHand.h"
#include "CRightHand.h"
#include "CMiddlePart.h"


CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCharacter(pGraphicDev)
	, m_pLeftHand(nullptr), m_pRightHand(nullptr), m_pMiddlePart(nullptr)
	, m_bCheck(false)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CCharacter(rhs)
	, m_pLeftHand(nullptr), m_pRightHand(nullptr), m_pMiddlePart(nullptr)
	, m_bCheck(false)
{
}

CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	if (FAILED(Add_PlayerPart()))
		return E_FAIL;

	m_pTransformCom->m_vScale = { 6.f, 6.f, 1.f };

	return S_OK;
}

_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CCharacter::Update_GameObject(fTimeDelta);


	m_pLeftHand->Update_GameObject(fTimeDelta);
	m_pRightHand->Update_GameObject(fTimeDelta);
	m_pMiddlePart->Update_GameObject(fTimeDelta);

	return iExit;
}

void CPlayer::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Key_Input(fTimeDelta);

	CCharacter::LateUpdate_GameObject(fTimeDelta);


	m_pLeftHand->LateUpdate_GameObject(fTimeDelta);
	m_pRightHand->LateUpdate_GameObject(fTimeDelta);
	m_pMiddlePart->LateUpdate_GameObject(fTimeDelta);

}

void CPlayer::Render_GameObject()
{

}

HRESULT CPlayer::Add_Component()
{
	if (FAILED(CCharacter::Add_Component())) return E_FAIL;
	return S_OK;
}

void CPlayer::Key_Input(const _float& fTimeDelta)
{

	Engine::CTransform* pTransform = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()->
		Get_Component(ID_DYNAMIC, L"Environment_Layer", L"Camera", L"Com_Transform"));

	_vec3 vLook, vRight;
	pTransform->Get_Info(INFO_LOOK, &vLook);

	_vec3 vLookExCludeY = vLook;
	vLookExCludeY.y = 0;

	pTransform->Get_Info(INFO_RIGHT, &vRight);

	// 앞으로 이동
	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_W) & 0x80)
	{
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLookExCludeY, &vLookExCludeY), fTimeDelta, 10.f);

	}

	// 왼쪽 이동
	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_A) & 0x80)
	{
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, -10.f);

	}

	// 뒤로 이동
	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_S) & 0x80)
	{
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLookExCludeY, &vLookExCludeY), fTimeDelta, -10.f);
	}

	// 오른쪽 이동
	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_D) & 0x80)
	{
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, 10.f);
	}


	// 공격 
	if (CDInputMgr::GetInstance()->Get_DIMouseState(DIM_LB) & 0x80)
	{

	}


	// 대쉬 
	if (CDInputMgr::GetInstance()->Get_DIMouseState(DIM_LB) & 0x80)
	{

	}

}

HRESULT CPlayer::Add_PlayerPart()
{
	//Left
	m_pLeftHand = CLeftHand::Create(m_pGraphicDev);

	if (nullptr == m_pLeftHand)
		return E_FAIL;

	//Right

	m_pRightHand = CRightHand::Create(m_pGraphicDev);

	if (nullptr == m_pRightHand)
		return E_FAIL;


	//Middle

	m_pMiddlePart = CMiddlePart::Create(m_pGraphicDev);

	if (nullptr == m_pMiddlePart)
		return E_FAIL;


	return S_OK;
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayer* pPlayer = new CPlayer(pGraphicDev);

	if (FAILED(pPlayer->Ready_GameObject()))
	{
		Safe_Release(pPlayer);
		MSG_BOX("pPlayer Create Failed");
		return nullptr;
	}

	return pPlayer;
}

void CPlayer::Free()
{
	Safe_Release(m_pLeftHand);
	Safe_Release(m_pRightHand);
	Safe_Release(m_pMiddlePart);

	CCharacter::Free();
}