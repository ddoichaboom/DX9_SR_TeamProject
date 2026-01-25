#pragma once
#include "CEditorObject.h"

class CEditorVendingMachine : public CEditorObject
{
private:
    explicit            CEditorVendingMachine(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual             ~CEditorVendingMachine();

public:
    virtual HRESULT     Ready_GameObject() override;
    virtual _int        Update_GameObject(const _float& fTimeDelta) override;
    virtual void        LateUpdate_GameObject(const _float& fTimeDelta) override;
    virtual void        Render_GameObject() override;

private:
    HRESULT             Add_Component();

public:
    static CEditorVendingMachine* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);
    static CEditorVendingMachine* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot, _vec3 vScale);

private:
    virtual void        Free() override;
};

