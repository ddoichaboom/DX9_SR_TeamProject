#pragma once
#include "CEditorObject.h"

enum WALL_DIR
{
    WALL_XY,
    WALL_YZ,
    WALL_END
};

class CEditorWall : public CEditorObject
{
private:
    explicit        CEditorWall(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual         ~CEditorWall();

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

private:
    WALL_DIR        m_eWallDir;     // 벽 방향

public:
    // 기본 생성 (XY 평면, 기본 크기)
    static CEditorWall* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

    // 방향 지정 생성
    static CEditorWall* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, WALL_DIR eDir);

    // 전체 파라미터 지정 (맵 로드용)
    static CEditorWall* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot, _vec3 vScale, WALL_DIR eDir);

private:
    virtual void    Free() override;
};

