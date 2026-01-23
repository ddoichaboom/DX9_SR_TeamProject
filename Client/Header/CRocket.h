#pragma once
#include "CGameObject.h"

namespace Engine
{
    class CRcTex;
    class CTransform;
    class CTexture;
    class CCollision;
    class CCollider;
}

class CToonFlash;
class CToonFog;

class CRocket :
    public CGameObject
{
protected:
    enum DIR_STATE { FRONT_90, FRONT_100, FRONT_110, FRONT_120, LEFT_45, LEFT_30, RIGHT_30, RIGHT_45, DIR_END };
    static _vec2 m_DirFrame[DIR_END];
protected:
    CRocket(LPDIRECT3DDEVICE9 pGraphicDev);
    CRocket(const CRocket& rhs);
    virtual	~CRocket();

public:
    static CRocket* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    static TextureSource& GetTextureSource()
    {
        return m_textureSource;
    }

public:
    HRESULT			        Ready_GameObject() override;
    _int			        Update_GameObject(const _float& fTimeDelta) override;
    void			        LateUpdate_GameObject(const _float& fTimeDelta) override;
    void			        Render_GameObject() override;

public:
    void                    SetPos(_vec3 _pos) override;
    void                    SetDirection(_vec3 _dir);
    void                    SetSpeed(_float _speed) { m_fSpeed = _speed; }

    void					Activate() override;
    void					Deactivate() override;
    _float					GetAttackDamage() override
    {
        return m_fAttackDamage;
    }
protected:
    HRESULT                 Add_Component() override;
    virtual void	        Free();
    void                    SetBillboard();
protected:
    static TextureSource    m_textureSource;

protected:
    CRcTex*                 m_pBufferCom;
    CTransform*             m_pTransformCom;
    CTexture*               m_pTextureCom;
    CCollision*             m_pCollisionCom;

    _float                  m_fSpeed;
    _vec3                   m_vDir;
    _float                  m_fLifeTime;
    _float                  m_fTime;

    CCollider*              m_pCollider;
    const _tchar*           m_szColliderName = L"ColBody";

    _float                  m_fVerticalAngle;
    _float                  m_fHorizonAngle;
    _vec2                   m_vCurFrame = { 0,0 };

protected:
    //Effect
    CToonFlash*             m_pToonFlash;
    _vec3                   vToonFlashPos{};
    _vec3                   vToonFlashLocalPos = { 0,0,-0.1f };
    CToonFog*               m_pToonFog;
    _float					m_fAttackDamage;
};

