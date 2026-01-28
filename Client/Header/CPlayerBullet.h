#pragma once
#include "CBullet.h"

namespace Engine
{
    class CAnimation;
};


class CPlayerBullet :
    public CBullet
{
protected:
    CPlayerBullet(LPDIRECT3DDEVICE9 pGraphicDev);
    CPlayerBullet(const CPlayerBullet& rhs);
    virtual	~CPlayerBullet();

public:
    static CPlayerBullet* Create(LPDIRECT3DDEVICE9 pGraphicDev);

    static TextureSource& GetTextureSource()
    {
        return m_textureSource;
    }
    static AnimationSource& GetAnimSource()
    {
        return m_vAnimSource;
    }

public:
    HRESULT			        Ready_GameObject() override;
    _int			        Update_GameObject(const _float& fTimeDelta) override;
    void			        LateUpdate_GameObject(const _float& fTimeDelta) override;
    void			        Render_GameObject() override;

protected:
    HRESULT                 Add_Component() override;
    virtual void	        Free();
public:
    void					Activate() override;
    void					Deactivate() override;
    void                    SetDirection(_vec3 dir) override;
    void                    SetBillboard();
protected:
    static TextureSource    m_textureSource;
    static AnimationSource  m_vAnimSource;

    Engine::CAnimation* m_pAnimationCom;
};

