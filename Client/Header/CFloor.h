#pragma once
#include "CTerrain.h"

class CFloor : public CTerrain
{
protected:
    explicit CFloor(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CFloor(const CFloor& rhs);
    virtual ~CFloor();

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
    virtual void        Set_FloorType(_uint eFloorType);

protected:
    virtual HRESULT     Add_Component() override;

public:
    // 기본 생성 
    static CFloor* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
    virtual void Free() override;

private:
    static vector<TextureSource>    m_vTextureSource;
};

