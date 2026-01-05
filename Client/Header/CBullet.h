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

//TODO : 총알이 위로 향하는 경우가 있다면 추가 구현 
class CBullet :
    public CGameObject
{
    enum DIR_STATE { DIR_RIGHT, DIR_LEFT, DIR_MAXLEFT, DIR_MAXRIGHT,DIR_END};
protected:
    CBullet(LPDIRECT3DDEVICE9 pGraphicDev);
    CBullet(const CBullet& rhs);
    virtual	~CBullet();

public:
    static CBullet*         Create(LPDIRECT3DDEVICE9 pGraphicDev);
    static TextureSource&   GetTextureSource()
    {
        return m_textureSource;
    }

public:
    HRESULT			        Ready_GameObject() override;
    _int			        Update_GameObject(const _float& fTimeDelta) override;
    void			        LateUpdate_GameObject(const _float& fTimeDelta) override;
    void			        Render_GameObject() override;
   
public:
    void                    SetRotation(ROTATION eType, const _float& fAngle);
    void                    SetPos(_vec3 _pos);
    void                    SetDirection(_vec3 dir);
    void                    SetSpeed(_float _speed) { m_fSpeed = _speed; }

    void					Activate() override;
    void					Deactivate() override;
protected:
    HRESULT                 Add_Component() override;
    virtual void	        Free();

protected:
    static TextureSource    m_textureSource;
    static _vec2            m_vFrameIdx[DIR_END];

protected:
    Engine::CRcTex*         m_pBufferCom;
    Engine::CTransform*     m_pTransformCom;
    Engine::CTexture*       m_pTextureCom;
    Engine::CCollision*     m_pCollisionCom;

    _float                  m_fSpeed;
    _vec3                   m_vDir;
    DIR_STATE               m_STATE;
    
    //X회전에 추가로 더할 값(윗 면이 카메라에 보이기 위한 추가 회전)
    const _float            m_RotXoffset = 110.f;
    _matrix                 m_matPreRot;
    _float                  m_fLifeTime;
    _float                  m_fTime;


    CCollider*              m_pCollider;
    const _tchar*           m_szColliderName = L"ColBody";

};

