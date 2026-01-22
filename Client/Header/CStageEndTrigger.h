#pragma once
#include "CTrigger.h"

class CStageEndTrigger : public CTrigger
{
protected:
    explicit CStageEndTrigger(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CStageEndTrigger(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale);
    virtual ~CStageEndTrigger();

public:
    virtual HRESULT     Ready_GameObject() override;
    virtual _int        Update_GameObject(const _float& fTimeDelta) override;
    virtual void        Render_GameObject() override;

public:
    virtual void OnBeginCollision() override;

public:
    static CStageEndTrigger* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    static CStageEndTrigger* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale);

private:
    virtual void Free() override;

};

