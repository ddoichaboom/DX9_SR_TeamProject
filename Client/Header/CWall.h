#pragma once
#include "CTerrain.h"

class CWall : public CTerrain
{
protected:
    explicit CWall(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CWall(const CWall& rhs);
    virtual ~CWall();

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
    virtual void        Set_WallType(_uint eWallType);

protected:
    virtual HRESULT     Add_Component() override;

public:
    // 기본 생성
    static CWall*       Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
    virtual void        Free() override;
    _uint               m_iWallType;

private:
    static vector<TextureSource>    m_vTextureSource;
};

