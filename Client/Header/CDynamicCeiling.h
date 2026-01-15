#pragma once
#include "CCeiling.h"

namespace Engine
{
    class CAnimation;
}

class CDynamicCeiling : public CCeiling
{
protected:
	explicit CDynamicCeiling(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CDynamicCeiling(const CDynamicCeiling& rhs);
	virtual ~CDynamicCeiling();

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
    virtual void        Set_CeilingType(_uint eCeilingType) override;

protected:
    Engine::CAnimation* m_pAnimationCom;

protected:
    virtual HRESULT     Add_Component() override;

public:
    // 기본 생성 
    static CDynamicCeiling* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
    virtual void Free() override;

private:
    static vector<TextureSource>    m_vTextureSource;
    static vector<AnimationSource>  m_vAnimSource;
};

