#pragma once
#include "CEditorWall.h"

namespace Engine
{
	class CAnimation;
}

class CEditorDynamicWall : public CEditorWall
{
protected:
	explicit CEditorDynamicWall(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEditorDynamicWall();

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

protected:
    virtual HRESULT Add_Component() override;

public:
    // 기본 생성 (XY 평면, 기본 크기)
    static CEditorDynamicWall* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

    // 방향 지정 생성
    static CEditorDynamicWall* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, WALL_DIR eDir);

    // 전체 파라미터 지정 (맵 로드용)
    static CEditorDynamicWall* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot, _vec3 vScale, WALL_DIR eDir);

    static CEditorDynamicWall* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot, _vec3 vScale, WALL_DIR eDir, _uint iType, _int iIdx);


public:
    virtual void Set_WallType(_uint eWallType) override;
    virtual void Set_TextureIdx(_int iIdx) override;

protected:
    virtual void Free() override;

private:
    Engine::CAnimation* m_pAnimationCom;

private:
    static vector<TextureSource>    m_vTextureSource;
    static vector<AnimationSource>  m_vAnimSource;
};

