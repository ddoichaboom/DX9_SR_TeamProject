#pragma once
#include "CEditorFloor.h"

namespace Engine
{
	class CScrollTexture;
}

class CEditorDynamicFloor :public CEditorFloor
{
private:
	explicit        CEditorDynamicFloor(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual         ~CEditorDynamicFloor();

public:
    static vector<TextureSource>& GetTextureSources()
    {
        return m_vTextureSource;
    }

public:
    virtual HRESULT Ready_GameObject() override;
    virtual _int    Update_GameObject(const _float& fTimeDelta) override;
    virtual void    LateUpdate_GameObject(const _float& fTimeDelta) override;
    virtual void    Render_GameObject() override;

private:
    virtual HRESULT Add_Component() override;

public:
    virtual void    Set_FloorType(_uint iType) override;  

protected:
    Engine::CScrollTexture*         m_pScrollTextureCom;

public:
    // 기본 생성 (배치용)
    static CEditorDynamicFloor* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

    static CEditorDynamicFloor* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot, _vec3 vScale);

    // 전체 파라미터 생성 (맵 로드)
    static CEditorDynamicFloor* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot, _vec3 vScale, _uint iType);

private:
    static vector<TextureSource>    m_vTextureSource;
    static vector<AnimationSource>  m_vAnimSource;

private:
    virtual void    Free() override;
};

