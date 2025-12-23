#pragma once
#include "CGameObject.h"

class CGrid : public  Engine::CGameObject
{
private:
    explicit        CGrid(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual         ~CGrid();

public:
    HRESULT         Ready_Grid(_uint iCountX, _uint iCountZ, _float fInterval);

    virtual _int    Update_GameObject(const _float& fTimeDelta) override;
    virtual void    Render_GameObject() override;

public:
    void            Set_Visible(_bool bVisible) { m_bVisible = bVisible; }
    _bool           Get_Visible() const { return m_bVisible; }

private:
    LPDIRECT3DDEVICE9           m_pGraphicDev;
    LPDIRECT3DVERTEXBUFFER9     m_pVB;

    _uint                       m_dwVtxCount;
    _bool                       m_bVisible;


public:
    static CGrid* Create(LPDIRECT3DDEVICE9 pGraphicDev,
        _uint iCountX, _uint iCountZ, _float fInterval);

private:
    virtual void Free() override;
};

