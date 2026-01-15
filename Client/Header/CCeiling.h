#pragma once
#include "CTerrain.h"

class CCeiling : public CTerrain
{
protected:
    explicit CCeiling(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CCeiling(const CCeiling& rhs);
    virtual ~CCeiling();

public:
    static vector<TextureSource>& GetTextureSources()
    {
        return m_vTextureSource;
    }

public:
    virtual HRESULT     Ready_GameObject() override;
    virtual _int        Update_GameObject(const _float& fTimeDelta) override;
    virtual void        LateUpdate_GameObject(const _float& fTimeDelta) override;
    virtual void        Render_GameObject() override;
    virtual void        Set_CeilingType(_uint eCeilingType);

protected:
    virtual HRESULT     Add_Component() override;

public:
    static CCeiling*    Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
    virtual void        Free() override;

private:
    static vector<TextureSource>    m_vTextureSource;
};

