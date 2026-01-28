#include "pch.h"
#include "CSniferCamera.h"
#include "CDInputMgr.h"
#include "CProtoMgr.h"


const _float eps = 0.001f;

CSniferCamera::CSniferCamera(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* _owner)
	: CCamera(pGraphicDev), m_pTransformCom(nullptr),m_pOwner(_owner)
{
	m_eOBJ_ID = OBJ_CAM;
	m_iID = Make_ID();
}

CSniferCamera::~CSniferCamera()
{

}


HRESULT CSniferCamera::Ready_GameObject(const _vec3* pEye,
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

	if (FAILED(CCamera::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	if (m_pOwner)
	{
		m_pOwnerTransformCom = dynamic_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, L"Com_Transform"));
		if (!m_pOwnerTransformCom) return E_FAIL;
		m_pTransformCom->Set_Pos(*m_pOwnerTransformCom->Get_Info(INFO_POS));
	}

	for (int rot = ROT_X; rot != ROT_END; rot++)
	{
		m_vAngleLimit[rot].x = -180.f;
		m_vAngleLimit[rot].y = 180.f;
	}

	return S_OK;
}

_int CSniferCamera::Update_GameObject(const _float& fTimeDelta)
{
	m_pTransformCom->Update_Component(fTimeDelta);

	m_vEye = *m_pTransformCom->Get_Info(INFO_POS);
	_vec3 vLook = *m_pTransformCom->Get_Info(INFO_LOOK);
	m_vUp = *m_pTransformCom->Get_Info(INFO_UP);
	m_vAt = m_vEye + vLook * m_fFar;

	m_fTime += fTimeDelta;

	if (m_bLerp)
	{
		if (m_fTime >= m_fLerpTime)
		{
			m_bLerp = false;
			m_fFov = m_fDestFOV;
			return 0 ;
		}
		m_fFov = m_fStartFOV + (m_fDestFOV - m_fStartFOV)* easeOutQuint(m_fTime / m_fLerpTime) ;
	}
	else if (m_bCameraShake)
	{
		_float t = m_fTime / m_fCameraShakeTime;
		_float p = 1.0f - t;

		_float randomfX = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
		_float randomfY = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;

		m_fCameraShakeOffset.x = p * randomfX * m_fCameraPower.x;
		m_fCameraShakeOffset.y = p * randomfY * m_fCameraPower.y;

		m_vEye.x += m_fCameraShakeOffset.x;
		m_vEye.y += m_fCameraShakeOffset.y;
		m_vAt = m_vEye + vLook * m_fFar;

		if (m_fTime >= m_fCameraShakeTime)
		{
			m_fCameraShakeOffset = { 0,0 };
			m_bCameraShake = false;
		}
	}
	return 0;
}

void CSniferCamera::LateUpdate_GameObject(const _float& fTimeDelta)
{
	MouseFix();
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);

}

HRESULT CSniferCamera::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	pComponent = m_pTransformCom = static_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	return S_OK;
}

void CSniferCamera::MouseFix()
{
	POINT		ptMouse{ WINCX >> 1, WINCY >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}


_float CSniferCamera::Get_Rot(ROTATION rot)
{
	if (!m_pTransformCom) return 0.f;
	return m_pTransformCom->m_vAngle[rot];
}

void CSniferCamera::Set_Rot(ROTATION rot, _float _angle)
{
	if (!m_pTransformCom) return;
	m_pTransformCom->Rotation(rot, _angle);
	
	_float angle = m_pTransformCom->m_vAngle[rot];

	if (angle < m_vAngleLimit[rot].x) angle = m_vAngleLimit[rot].x;
	else if(angle > m_vAngleLimit[rot].y)  angle = m_vAngleLimit[rot].y;

	m_pTransformCom->m_vAngle[rot] = angle;
}

void CSniferCamera::Set_AngleLimit(ROTATION rot, _float minAngle, _float maxAngle)
{
	m_vAngleLimit[rot].x = minAngle;
	m_vAngleLimit[rot].y = maxAngle;
}

void CSniferCamera::CameraShake()
{
	if (m_bCameraShake) return;
	m_bCameraShake = true;
	m_fTime = 0.f;
}

void CSniferCamera::ZoomIn(_float _fov)
{
	if (m_bZooming) return;
	m_fTime = 0.f;
	SaveFOV();
	m_bLerp = true;
	m_bZooming = true;

	m_fStartFOV = m_fFov;
	m_fDestFOV = _fov;
}

void CSniferCamera::ZoomOut()
{
	if (!m_bZooming) return;
	m_fTime = 0.f;
	m_bLerp = true;
	m_bZooming = false;

	m_fStartFOV = m_fFov;
	m_fDestFOV = LoadFOV();
}

void CSniferCamera::SaveFOV()
{
	m_fSavedFOV =  m_fFov;
}

_float CSniferCamera::LoadFOV()
{
	return m_fSavedFOV;
}

CSniferCamera* CSniferCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* _owner,
	const _vec3* pEye, const _vec3* pAt, const _vec3* pUp,
	const _float& fFov, const _float& fAspect,
	const _float& fNear, const _float& fFar)
{
	CSniferCamera* pCamera = new CSniferCamera(pGraphicDev, _owner);

	if (FAILED(pCamera->Ready_GameObject(pEye, pAt, pUp, fFov, fAspect, fNear, fFar)))
	{
		Safe_Release(pCamera);
		MSG_BOX("Snifer Camera Create Failed");
		return nullptr;
	}

	return pCamera;
}

void CSniferCamera::Free()
{
	CCamera::Free();
}