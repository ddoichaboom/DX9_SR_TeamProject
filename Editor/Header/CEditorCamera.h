#pragma once
#include "CGameObject.h"

enum CAMERA_MODE
{
    MODE_FREE,      // 자유 비행 ( 기본, 3D 편집용)
    MODE_FPS,       // 1인칭 ( 지면 기준 이동 )
    MODE_ORBIT,     // 궤도 ( 특정 지점 중심 회전 ) - Phase 5 이후에 구현 
    CM_END
};

class CEditorCamera : public Engine::CGameObject
{
private:
    explicit        CEditorCamera(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual         ~CEditorCamera();

public:
    virtual HRESULT Ready_GameObject() override;
    virtual _int    Update_GameObject(const _float& fTimeDelta) override;
    virtual void    LateUpdate_GameObject(const _float& fTimeDelta) override;
    virtual void    Render_GameObject() override {}

public:
    void            Set_Position(_vec3 vPos)
    {
        _vec3 vDelta = vPos - m_vEye;
        m_vEye = vPos;
        m_vAt += vDelta;
    }

    void            Set_LookAt(_vec3 vLookAt) { m_vAt = vLookAt; }
    void            Set_Speed(_float fSpeed) { m_fSpeed = fSpeed; }

    // 감도 ( Get / Set )  
    _float          Get_Sensitivity() const { return m_fSensitivity; }
    void            Set_Sensitivity(_float fSensitivity) { m_fSensitivity = fSensitivity; }

    // 카메라 모드 ( Get / Set ) 
    CAMERA_MODE     Get_CameraMode() const { return m_eMode; }
    void            Set_CameraMode(CAMERA_MODE eMode) { m_eMode = eMode; }

    // 카메라 이동 속도 ( Get / Set ) 
    _float          Get_CameraSpeed() const { return m_fSpeed; }
    void            Set_CameraSpeed(_float fSpeed) { m_fSpeed = fSpeed; }

    // 뷰 스페이스 행렬 / 투영 행렬 - Get 
    void            Get_ViewMatrix(_matrix* pOut) { *pOut = m_matView; }
    void            Get_ProjMatrix(_matrix* pOut) { *pOut = m_matProj; }

private:
    void            Update_FreeCamera(const _float& fTimeDelta);
    void            Update_Mouse();

private:
    LPDIRECT3DDEVICE9   m_pGraphicDev;

    _vec3               m_vEye;
    _vec3               m_vAt;
    _vec3               m_vUp;

    _float              m_fPitch;
    _float              m_fYaw;

    _float              m_fSpeed;
    _float              m_fSensitivity;

    _matrix             m_matView;
    _matrix             m_matProj;

    POINT               m_ptPrevMouse;

    CAMERA_MODE         m_eMode;

public:
    static CEditorCamera* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    virtual void Free() override;

};

