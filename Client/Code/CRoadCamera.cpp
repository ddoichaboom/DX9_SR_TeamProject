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
	, m_fShakeTime(0.f), m_fShakeSpeed(0.5f), m_fShakePower(1.f), m_bStageEnd(false)

{
	m_eOBJ_ID = OBJ_CAM;
	m_iID = Make_ID();
}

CRoadCamera::CRoadCamera(const CRoadCamera& rhs)
	: CCamera(rhs), m_bFix(true), m_bCheck(true)
	, m_pTransformCom(nullptr), m_fSpeed(0.f)
	, m_fPitch(0.f), m_fYaw(0.f)
	, m_fShakeTime(0.f), m_fShakeSpeed(0.5f), m_fShakePower(1.f), m_bStageEnd(false)
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

void CRoadCamera::OnEvent(EVENT_TYPE _type, EventData* _pData)
{
	if (_type == EVENT_ENDING)
	{
		
		m_bStageEnd = true;
		m_bFix = false;		
	}
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

	CEventMgr::GetInstance()->Subscribe(EVENT_ENDING, this);
	return S_OK;
}

_int CRoadCamera::Update_GameObject(const _float& fTimeDelta)
{
	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);	
	_int iExit = CCamera::Update_GameObject(fTimeDelta);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);


	for (auto& pComponent : m_mapComponent[ID_DYNAMIC])
		pComponent.second->Update_Component(fTimeDelta);

	Engine::CTransform* pPlayerTransform = static_cast<CTransform*>(Engine::CManagement::GetInstance()->
		Get_Component(ID_DYNAMIC, L"GameLogic_Layer", OBJ_PLAYER, L"Com_Transform"));

	if (pPlayerTransform == nullptr)
		return 0;

	_vec3 vPos, vLook, vUp;
	pPlayerTransform->Get_Info(INFO_POS, &vPos);
	vPos.y += 6.f;
	vUp = _vec3(0.f, 1.f, 0.f);
	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

	m_pTransformCom->Get_Info(INFO_POS, &m_vEye);
	m_pTransformCom->Get_Info(INFO_LOOK, &vLook);
	m_vAt = m_vEye + vLook;

	_matrix matRot;
	D3DXMatrixRotationYawPitchRoll(&matRot,
		D3DXToRadian(m_pTransformCom->m_vAngle.y),
		D3DXToRadian(m_pTransformCom->m_vAngle.x),
		D3DXToRadian(m_pTransformCom->m_vAngle.z));

	D3DXVec3TransformNormal(&m_vUp, &vUp, &matRot);

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
	}
	else if(m_fFov != D3DXToRadian(60.f))
	{
		m_fFov = D3DXToRadian(60.f);
	}

	// Z축 기울기 목표값 및 회전 속도 설정
	_float fRollLimit = 3.0f;  // 기울기 각도
	_float fRollSpeed = 20.0f; // 기울어지는 속도

	// A 키: 왼쪽으로 기울임 (Z축 + 방향)
	if (CDInputMgr::GetInstance()->Key_Pressing(DIK_A))
	{
		m_pTransformCom->m_vAngle.z += fRollSpeed * fTimeDelta;
		if (m_pTransformCom->m_vAngle.z > fRollLimit)
			m_pTransformCom->m_vAngle.z = fRollLimit;
	}
	// D 키: 오른쪽으로 기울임 (Z축 - 방향)
	else if (CDInputMgr::GetInstance()->Key_Pressing(DIK_D))
	{
		m_pTransformCom->m_vAngle.z -= fRollSpeed * fTimeDelta;
		if (m_pTransformCom->m_vAngle.z < -fRollLimit)
			m_pTransformCom->m_vAngle.z = -fRollLimit;
	}
	// 아무것도 안 누르면 서서히 0으로 복귀 (복원력)
	else
	{
		if (m_pTransformCom->m_vAngle.z > 0.1f)
			m_pTransformCom->m_vAngle.z -= fRollSpeed * fTimeDelta;
		else if (m_pTransformCom->m_vAngle.z < -0.1f)
			m_pTransformCom->m_vAngle.z += fRollSpeed * fTimeDelta;
		else
			m_pTransformCom->m_vAngle.z = 0.f;
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