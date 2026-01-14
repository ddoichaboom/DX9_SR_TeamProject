#pragma once
#include "CEditorObject.h"

namespace Engine
{
    class CTexture;
}

enum WALL_DIR
{
    WALL_XY_FRONT,
    WALL_XY_BACK,
    WALL_YZ_LEFT,
    WALL_YZ_RIGHT,
    WALL_END
};

class CEditorWall : public CEditorObject
{
private:
    explicit        CEditorWall(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual         ~CEditorWall();

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

public:
    // Wall 방향 설정/조회
    void            Set_WallDirection(WALL_DIR eDir);
    WALL_DIR        Get_WallDirection() const { return m_eWallDir; }

private:
    HRESULT         Add_Component();

public:
    void            Set_TextureIdx(_int iIdx);
    _int            Get_TextureIdx() const { return m_iTextureIdx; }

    void            Set_WallType(_uint iType);
    _uint           Get_WallType() const { return m_iWallType; }

protected:
    CTexture*       m_pTextureCom;
    _int            m_iTextureIdx;      // 0 ~ 2 (아틀라스 인덱스)
    _uint           m_iWallType;       // enum 값 

private:
    WALL_DIR        m_eWallDir;     // 벽 방향

public:
    // 기본 생성 (XY 평면, 기본 크기)
    static CEditorWall* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

    // 방향 지정 생성
    static CEditorWall* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, WALL_DIR eDir);

    // 전체 파라미터 지정 (맵 로드용)
    static CEditorWall* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot, _vec3 vScale, WALL_DIR eDir);

    static CEditorWall* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot, _vec3 vScale, WALL_DIR eDir, _uint iType, _int iIdx);

private:
    static vector<TextureSource> m_vTextureSource;

protected:
    virtual void    Free() override;
};

