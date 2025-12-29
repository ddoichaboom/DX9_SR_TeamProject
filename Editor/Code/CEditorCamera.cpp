#include "pch.h"
#include "CEditorCamera.h"

#include "CDInputMgr.h"

CEditorCamera::CEditorCamera(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
    , m_pGraphicDev(pGraphicDev)
    , m_vEye(0.f, 0.f, 0.f)
    , m_vAt(0.f, 0.f, 0.f)
    , m_vUp(0.f, 1.f, 0.f)
    , m_fPitch(0.f)
    , m_fYaw(0.f)
    , m_fSpeed(1.f)
    , m_fSensitivity(0.125f)
    , m_bRightButton(false)
    , m_eMode(MODE_FREE)
{
    m_pGraphicDev->AddRef();
    ZeroMemory(&m_ptPrevMouse, sizeof(POINT));
}

CEditorCamera::~CEditorCamera()
{
}

HRESULT CEditorCamera::Ready_GameObject()
{
    m_vEye = _vec3(0.f, 10.f, -10.f);
    m_vAt = _vec3(0.f, 0.f, 0.f);
    m_vUp = _vec3(0.f, 1.f, 0.f);

    // Projection Matrix 초기화
    D3DXMatrixPerspectiveFovLH(&m_matProj,
        D3DXToRadian(60.f),
        (_float)WINCX / WINCY,
        0.1f,
        1000.f);

    return S_OK;
}

_int CEditorCamera::Update_GameObject(const _float& fTimeDelta)
{
    // 1번 키 : FREE Camera 모드
    if (Engine::CDInputMgr::GetInstance()->Key_Down(DIK_1))
        m_eMode = MODE_FREE;

    // 2번 키 : FPS Camera 모드
    if (Engine::CDInputMgr::GetInstance()->Key_Down(DIK_2))
        m_eMode = MODE_FPS;

    Update_FreeCamera(fTimeDelta);
    Update_Mouse();

    // View Matrix 계산
    D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);

    return 0;
}

void CEditorCamera::LateUpdate_GameObject(const _float& fTimeDelta)
{
}

void CEditorCamera::Update_FreeCamera(const _float& fTimeDelta)
{
    // Look, Right 벡터 계산
    _vec3 vLook = m_vAt - m_vEye;

    // 카메라 모드에 따라 Look 벡터 수정 
    if (m_eMode == MODE_FPS)
    {
        vLook.y = 0.f;  // FPS 모드 - 수평만 이동하는 방식
    }
    // MODE_FREE는 그대로 (3D 자유 이동)

    D3DXVec3Normalize(&vLook, &vLook);

    _vec3 vRight;
    D3DXVec3Cross(&vRight, &vLook, &m_vUp);      // Look x Up = Right
    D3DXVec3Normalize(&vRight, &vRight);

    // 속도 (Shift: 2배)
    _float fSpeed = m_fSpeed;
    if (Engine::CDInputMgr::GetInstance()->Key_Pressing(DIK_LSHIFT))
        fSpeed *= 2.f;

    // WASD 이동
    if (Engine::CDInputMgr::GetInstance()->Key_Pressing(DIK_W))
    {
        m_vEye += vLook * fSpeed * fTimeDelta;
        m_vAt += vLook * fSpeed * fTimeDelta;
    }
    if (Engine::CDInputMgr::GetInstance()->Key_Pressing(DIK_S))
    {
        m_vEye -= vLook * fSpeed * fTimeDelta;
        m_vAt -= vLook * fSpeed * fTimeDelta;
    }
    if (Engine::CDInputMgr::GetInstance()->Key_Pressing(DIK_A))
    {
        m_vEye += vRight * fSpeed * fTimeDelta;
        m_vAt += vRight * fSpeed * fTimeDelta;
    }
    if (Engine::CDInputMgr::GetInstance()->Key_Pressing(DIK_D))
    {
        m_vEye -= vRight * fSpeed * fTimeDelta;
        m_vAt -= vRight * fSpeed * fTimeDelta;
    }

    // QE 상하 이동
    if (Engine::CDInputMgr::GetInstance()->Key_Pressing(DIK_Q))
    {
        m_vEye.y += fSpeed * fTimeDelta;
        m_vAt.y += fSpeed * fTimeDelta;
    }
    if (Engine::CDInputMgr::GetInstance()->Key_Pressing(DIK_E))
    {
        m_vEye.y -= fSpeed * fTimeDelta;
        m_vAt.y -= fSpeed * fTimeDelta;
    }
}

void CEditorCamera::Update_Mouse()
{
    if (Engine::CDInputMgr::GetInstance()->Mouse_Pressing(DIM_RB))
    {
        if (!m_bRightButton)
        {
            m_bRightButton = true;
            GetCursorPos(&m_ptPrevMouse);
        }
        else
        {
            POINT ptCurMouse;
            GetCursorPos(&ptCurMouse);

            _long dx = ptCurMouse.x - m_ptPrevMouse.x;
            _long dy = ptCurMouse.y - m_ptPrevMouse.y;

            m_fYaw += dx * m_fSensitivity;
            m_fPitch -= dy * m_fSensitivity;

            // Pitch 제한
            m_fPitch = max(-89.f, min(89.f, m_fPitch));

            // At 위치 업데이트
            _vec3 vLook;
            vLook.x = cosf(D3DXToRadian(m_fPitch)) * sinf(D3DXToRadian(m_fYaw));
            vLook.y = sinf(D3DXToRadian(m_fPitch));
            vLook.z = cosf(D3DXToRadian(m_fPitch)) * cosf(D3DXToRadian(m_fYaw));

            D3DXVec3Normalize(&vLook, &vLook);
            m_vAt = m_vEye + vLook;

            m_ptPrevMouse = ptCurMouse;
        }
    }
    else
    {
        m_bRightButton = false;
    }
}

CEditorCamera* CEditorCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CEditorCamera* pInstance = new CEditorCamera(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("EditorCamera Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CEditorCamera::Free()
{
    Safe_Release(m_pGraphicDev);

    CGameObject::Free();
}