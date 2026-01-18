#pragma once
#include "CFloor.h"

namespace Engine
{
    class CScrollTexture;
}

class CDynamicFloor : public CFloor
{
protected:
	explicit CDynamicFloor(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CDynamicFloor(const CDynamicFloor& rhs);
	virtual ~CDynamicFloor();

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
    virtual void        Set_FloorType(_uint eFloorType) override;
protected:
    Engine::CScrollTexture* m_pScrollTextureCom;

protected:
    virtual HRESULT     Add_Component() override;

public:
    // 기본 생성 
    static CDynamicFloor* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
    virtual void Free() override;

private:
    static vector<TextureSource>    m_vTextureSource;
};

