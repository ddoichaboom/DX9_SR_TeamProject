#pragma once
#include "CEditorObject.h"

class CEditorFloor : public CEditorObject
{
private:
    explicit        CEditorFloor(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual         ~CEditorFloor();

public:
    virtual HRESULT Ready_GameObject() override;
    virtual _int    Update_GameObject(const _float& fTimeDelta) override;
    virtual void    LateUpdate_GameObject(const _float& fTimeDelta) override;
    virtual void    Render_GameObject() override;

private:
    HRESULT         Add_Component();

public:
    // 기본 생성 (위치만)
    static CEditorFloor* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

    // 전체 파라미터 지정 (맵 로드용)
    static CEditorFloor* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot, _vec3 vScale);

private:
    virtual void    Free() override;
};

