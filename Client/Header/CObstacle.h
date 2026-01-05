#pragma once
#include "CGameObject.h"

namespace Engine
{
    class CCubeTex;
    class CTransform;
    class CTexture;
}

class CObstacle : public CGameObject
{
private:
    explicit CObstacle(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CObstacle(const CObstacle& rhs);
    virtual ~CObstacle();

public:
    virtual HRESULT     Ready_GameObject() override;
    virtual _int        Update_GameObject(const _float& fTimeDelta) override;
    virtual void        LateUpdate_GameObject(const _float& fTimeDelta) override;
    virtual void        Render_GameObject() override;

private:
    HRESULT             Add_Component();
    HRESULT             Ready_Material();

private:
    Engine::CCubeTex* m_pBufferCom;
    Engine::CTransform* m_pTransformCom;
    Engine::CTexture* m_pTextureCom;

public:
    // 기본 생성 (위치만)
    static CObstacle* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

    // 확장 생성 (위치, 회전, 스케일)
    static CObstacle* Create(LPDIRECT3DDEVICE9 pGraphicDev,
        _vec3 vPos, _vec3 vRot, _vec3 vScale);

private:
    virtual void Free() override;
};

