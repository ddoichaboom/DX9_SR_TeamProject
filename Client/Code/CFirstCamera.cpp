#include "pch.h"
#include "CFirstCamera.h"
#include "CDInputMgr.h"
#include "CProtoMgr.h"

#include "CManagement.h"
#include "CTransform.h"

CFirstCamera::CFirstCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCamera(pGraphicDev), m_bFix(true), m_bCheck(true)
{
}

CFirstCamera::CFirstCamera(const CFirstCamera& rhs)
	: CCamera(rhs), m_bFix(true), m_bCheck(true)
{
}

CFirstCamera::~CFirstCamera()
{
}

HRESULT CFirstCamera::Set_Transform(INFO eInfo, _vec3* pVector)
{
	if (eInfo >= INFO_END || eInfo < 0 || m_pTransformCom == nullptr)
		return E_FAIL;

	m_pTransformCom->m_vInfo[eInfo] = *pVector;



	return S_OK;
}

void CFirstCamera::Set_CamSetting()
{
	
	
}

HRESULT CFirstCamera::Ready_GameObject(const _vec3* pEye,
	const _vec3* pAt,
	const _vec3* pUp,
	const _float& fFov,
	const _float& fAspect,
	const _float& fNear,
	const _float& fFar)
{
	m_vEye = *pEye;
	m_vAt = *pAt;
	m_vUp = *pUp;

	m_fFov = fFov;
	m_fAspect = fAspect;
	m_fNear = fNear;
	m_fFar = fFar;
	m_fSpeed = 10.f;

	if (FAILED(Add_Component()))
		return E_FAIL;

	if (FAILED(CCamera::Ready_GameObject()))
		return E_FAIL;

	return S_OK;
}

_int CFirstCamera::Update_GameObject(const _float& fTimeDelta)
{
	// Transform -> Matrix
	for (auto& pComponent : m_mapComponent[ID_DYNAMIC])
		pComponent.second->Update_Component(fTimeDelta);

	_int iExit = CCamera::Update_GameObject(fTimeDelta);

	Engine::CTransform* pPlayerTransform = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()->
		Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));
	
	_vec3 vPos, vLook;
	pPlayerTransform->Get_Info(INFO_POS, &vPos);
	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

	m_pTransformCom->Get_Info(INFO_POS, &m_vEye);
	m_pTransformCom->Get_Info(INFO_LOOK, &vLook);
	m_vAt = m_vEye + vLook;
	return 0;
}

void CFirstCamera::LateUpdate_GameObject(const _float& fTimeDelta)
{	
	CCamera::LateUpdate_GameObject(fTimeDelta);

	
	Key_Input(fTimeDelta);

	if (m_bFix)
	{
		Mouse_Fix();
		Mouse_Move();
	}

}

HRESULT CFirstCamera::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	return S_OK;
}

void CFirstCamera::Key_Input(const _float& fTimeDelta)
{
	if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_TAB) & 0x80)
	{
		if (m_bCheck)
			return;

		m_bCheck = true;

		if (m_bFix)
			m_bFix = false;

		else
			m_bFix = true;
	}

	else
	{
		m_bCheck = false;
	}


	if (false == m_bFix)
		return;


}

void CFirstCamera::Mouse_Move()
{
	_long	dwMouseMove(0);

	if (dwMouseMove = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_Y))
	{
		m_pTransformCom->Rotation(ROT_X, D3DXToRadian(dwMouseMove * 5.f));	
	}

	if (dwMouseMove = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_X))
	{		
		m_pTransformCom->Rotation(ROT_Y, D3DXToRadian(dwMouseMove * 5.f));		
	}
}

void CFirstCamera::Mouse_Fix()
{
	POINT		ptMouse{ WINCX >> 1, WINCY >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);

}

CFirstCamera* CFirstCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev,
	const _vec3* pEye, const _vec3* pAt, const _vec3* pUp,
	const _float& fFov, const _float& fAspect,
	const _float& fNear, const _float& fFar)
{
	CFirstCamera* pCamera = new CFirstCamera(pGraphicDev);

	if (FAILED(pCamera->Ready_GameObject(pEye, pAt, pUp, fFov, fAspect, fNear, fFar)))
	{
		Safe_Release(pCamera);
		MSG_BOX("DynamicCamera Create Failed");
		return nullptr;
	}

	return pCamera;
}

void CFirstCamera::Free()
{
	CCamera::Free();
}
