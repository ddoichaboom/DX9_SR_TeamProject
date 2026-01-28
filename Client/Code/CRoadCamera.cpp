#include "pch.h"
#include "CRoadCamera.h"
#include "CDInputMgr.h"
#include "CProtoMgr.h"

#include "CManagement.h"
#include "CTransform.h"


CRoadCamera::CRoadCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCamera(pGraphicDev), m_bFix(true), m_bCheck(true)
	, m_pTransformCom(nullptr), m_fSpeed(0.f)
	, m_fPitch(0.f), m_fYaw(0.f)
	, m_fShakeTime(0.f), m_fShakeSpeed(0.5f), m_fShakePower(1.f)

{
	m_eOBJ_ID = OBJ_CAM;
	m_iID = Make_ID();
}

CRoadCamera::CRoadCamera(const CRoadCamera& rhs)
	: CCamera(rhs), m_bFix(true), m_bCheck(true)
	, m_pTransformCom(nullptr), m_fSpeed(0.f)
	, m_fPitch(0.f), m_fYaw(0.f)
	, m_fShakeTime(0.f), m_fShakeSpeed(0.5f), m_fShakePower(1.f)
{
	m_eOBJ_ID = OBJ_CAM;
	m_iID = Make_ID();
}

CRoadCamera::~CRoadCamera()
{
}

HRESULT CRoadCamera::Set_Transform(INFO eInfo, _vec3* pVector)
{
	if (eInfo >= INFO_END || eInfo < 0 || m_pTransformCom == nullptr)
		return E_FAIL;

	m_pTransformCom->m_vInfo[eInfo] = *pVector;



	return S_OK;
}

HRESULT CRoadCamera::Ready_GameObject(const _vec3* pEye,
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

_int CRoadCamera::Update_GameObject(const _float& fTimeDelta)
{
	// Transform -> Matrix
	_int iExit = CCamera::Update_GameObject(fTimeDelta);

	for (auto& pComponent : m_mapComponent[ID_DYNAMIC])
		pComponent.second->Update_Component(fTimeDelta);

	Engine::CTransform* pPlayerTransform = static_cast<CTransform*>(Engine::CManagement::GetInstance()->
		Get_Component(ID_DYNAMIC, L"GameLogic_Layer", OBJ_PLAYER, L"Com_Transform"));

	if (pPlayerTransform == nullptr)
		return 0;

	_vec3 vPos, vLook;
	pPlayerTransform->Get_Info(INFO_POS, &vPos);
	vPos.y += 6.f;

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

	m_pTransformCom->Get_Info(INFO_POS, &m_vEye);
	m_pTransformCom->Get_Info(INFO_LOOK, &vLook);
	m_vAt = m_vEye + vLook;

	pPlayerTransform->m_vAngle.y = m_pTransformCom->m_vAngle.y;


	return 0;
}

void CRoadCamera::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CCamera::LateUpdate_GameObject(fTimeDelta);


	Key_Input(fTimeDelta);

	if (m_bFix)
	{
		Mouse_Fix();
		Mouse_Move();
	}
}

HRESULT CRoadCamera::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	pComponent = m_pTransformCom = static_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	return S_OK;
}

void CRoadCamera::Key_Input(const _float& fTimeDelta)
{	
	if (CDInputMgr::GetInstance()->Mouse_Pressing(DIM_RB))
	{
		m_fFov = fmax(D3DXToRadian(30.f), m_fFov - fTimeDelta);
		CCamera::Ready_GameObject();
	}
	else if(m_fFov != D3DXToRadian(60.f))
	{
		m_fFov = D3DXToRadian(60.f);
		CCamera::Ready_GameObject();
	}
	
}

void CRoadCamera::Mouse_Move()
{
	_long	dwMouseMove(0);
	if (dwMouseMove = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_Y))
	{
		m_pTransformCom->m_vAngle.x += D3DXToDegree(dwMouseMove * 0.003f);

		if (m_pTransformCom->m_vAngle.x > 50.f)
			m_pTransformCom->m_vAngle.x = 50.f;
		if (m_pTransformCom->m_vAngle.x < -50.f)
			m_pTransformCom->m_vAngle.x = -50.f;
	}

	if (dwMouseMove = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_X))
	{
		m_pTransformCom->m_vAngle.y += D3DXToDegree(dwMouseMove * 0.003f);

		if (m_pTransformCom->m_vAngle.y > 360.f)
			m_pTransformCom->m_vAngle.y = 0.f;
		if (m_pTransformCom->m_vAngle.y < -360.f)
			m_pTransformCom->m_vAngle.y = 0.f;
	}
}

void CRoadCamera::Mouse_Fix()
{
	POINT		ptMouse{ WINCX >> 1, WINCY >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}

void CRoadCamera::Cam_Shake(const _float& fTimeDelta)
{
	m_fShakeTime += fTimeDelta * m_fShakeSpeed;
	float ShakeValue = sinf(m_fShakeTime) * m_fShakePower;
	m_vEye.y += ShakeValue;
	m_vAt.y += ShakeValue;
}

CRoadCamera* CRoadCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev,
	const _vec3* pEye, const _vec3* pAt, const _vec3* pUp,
	const _float& fFov, const _float& fAspect,
	const _float& fNear, const _float& fFar)
{
	CRoadCamera* pCamera = new CRoadCamera(pGraphicDev);

	if (FAILED(pCamera->Ready_GameObject(pEye, pAt, pUp, fFov, fAspect, fNear, fFar)))
	{
		Safe_Release(pCamera);
		MSG_BOX("DynamicCamera Create Failed");
		return nullptr;
	}

	return pCamera;
}

void CRoadCamera::Free()
{
	CCamera::Free();
}