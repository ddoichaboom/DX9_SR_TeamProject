#pragma once
#include "CEditorFloor.h"

namespace Engine
{
	class CAnimation;
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

    static vector<AnimationSource>& GetAnimSources()
    {
        return m_vAnimSource;
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

    // 애니메이션 제어
    void            Play_Animation();
    void            Pause_Animation();
    void            Set_AnimationSpeed(_float fSpeed);
    _float          Get_AnimationSpeed() const { return m_fAnimSpeed; }
    bool            Is_Playing() const;

protected:
    Engine::CAnimation*             m_pAnimationCom;
    _float                          m_fAnimSpeed;

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

