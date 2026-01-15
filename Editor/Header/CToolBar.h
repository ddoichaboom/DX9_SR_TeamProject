#pragma once
#include "CBase.h"

class CEditorCamera;
class CGrid;

enum EDITOR_MODE
{
    MODE_SELECT,               // 선택 모드
    MODE_PLACE_FLOOR,          // 바닥 배치 (Tile 대체)
    MODE_PLACE_DYNAMIC_FLOOR,
    MODE_PLACE_CEILING,        // 천장 배치 (새로 추가)
    MODE_PLACE_CUBE,           // 큐브 배치
    MODE_PLACE_WALL,           // 벽 배치 (새로 추가)
    MODE_PLACE_SPAWN_PLAYER,   // 플레이어 스폰 배치 (새로 추가)
    MODE_PLACE_SPAWN_MONSTER,  // 몬스터 스폰 배치 (새로 추가)
    MODE_END
};

class CToolBar : public CBase
{
    explicit CToolBar();
    virtual ~CToolBar();

public:
    HRESULT             Ready_ToolBar(CEditorCamera* pCamera, CGrid* pGrid);
    void                Update_ToolBar();
    void                Render_ToolBar();

public:
    EDITOR_MODE     Get_EditorMode() const { return m_eEditorMode; }

private:
    CEditorCamera* m_pCamera;
    CGrid* m_pGrid;

    // UI 상태
    bool                m_bShowGrid;

    EDITOR_MODE         m_eEditorMode;

public:
    static CToolBar* Create(CEditorCamera* pCamera, CGrid* pGrid);

private:
    virtual void        Free() override;
};

