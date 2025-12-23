#pragma once
#include "CEditorObject.h"

class CEditorCube : public CEditorObject
{
private:
    explicit            CEditorCube(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual             ~CEditorCube();

public:
    virtual HRESULT     Ready_GameObject() override;
    virtual _int        Update_GameObject(const _float& fTimeDelta) override;
    virtual void        LateUpdate_GameObject(const _float& fTimeDelta) override;
    virtual void        Render_GameObject() override;

private:
    HRESULT             Add_Component();

public:
    static CEditorCube* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

private:
    virtual void        Free() override;
};

