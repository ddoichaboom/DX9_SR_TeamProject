#pragma once
#include "CEditorObject.h"

namespace Engine
{
    class CCubeTex;
    class CCubeTexture;
}

class CEditorDisplayCubeObject : public CEditorObject
{
protected:
    explicit        CEditorDisplayCubeObject(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual         ~CEditorDisplayCubeObject();

public:
    static vector<TextureSource>& GetTextureSources()
    {
        return m_vTextureSource;
    }

public:
    virtual HRESULT         Ready_GameObject() override;
    virtual _int            Update_GameObject(const _float& fTimeDelta) override;
    virtual void            LateUpdate_GameObject(const _float& fTimeDelta) override;
    virtual void            Render_GameObject() override;

protected:
    HRESULT                 Add_Component();
    virtual void		    Free() override;

private:
    Engine::CCubeTex* m_pCubeBufferCom;
    Engine::CCubeTexture* m_pCubeTextureCom;

public:
    void							Set_CubeObjectType(DISPLAY_CUBE_OBJECT_TYPE eType);
    DISPLAY_CUBE_OBJECT_TYPE        Get_CubeObjectType() const { return m_eObjectType; }

    static  CEditorDisplayCubeObject* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);
    static  CEditorDisplayCubeObject* Create(LPDIRECT3DDEVICE9 pGraphicDev,
                                            _vec3 vPos, _vec3 vRot, _vec3 vScale, DISPLAY_CUBE_OBJECT_TYPE eType);



private:
    DISPLAY_CUBE_OBJECT_TYPE		m_eObjectType;
    static vector<TextureSource>    m_vTextureSource;
};

