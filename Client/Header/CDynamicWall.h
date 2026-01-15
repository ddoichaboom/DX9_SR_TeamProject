#pragma once
#include "CWall.h"

namespace Engine
{
	class CAnimation;
}

class CDynamicWall : public CWall
{
protected:
    explicit CDynamicWall(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CDynamicWall(const CDynamicWall& rhs);
    virtual ~CDynamicWall();

public:
    static vector<TextureSource>& GetTextureSources()
    {
        return m_vTextureSource;
    }
    static vector<AnimationSource>& GetAnimSources()
    {
        return m_vAnimSource;
    }

public:
    virtual HRESULT     Ready_GameObject() override;
    virtual _int        Update_GameObject(const _float& fTimeDelta) override;
    virtual void        LateUpdate_GameObject(const _float& fTimeDelta) override;
    virtual void        Render_GameObject() override;
    virtual void        Set_WallType(_uint eWallType) override;

protected:
    Engine::CAnimation* m_pAnimationCom;

protected:
    virtual HRESULT     Add_Component() override;

public:
    // 기본 생성 
    static CDynamicWall* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
    virtual void Free() override;

private:
    static vector<TextureSource>    m_vTextureSource;
    static vector<AnimationSource>  m_vAnimSource;
};

