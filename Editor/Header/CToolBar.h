#pragma once
#include "CBase.h"

class CEditorCamera;
class CGrid;

enum EDITOR_MODE
{
    MODE_SELECT,        // 선택 모드
    MODE_PLACE_TILE,    // 타일 배치
    MODE_PLACE_CUBE,    // 큐브 배치
    EM_END
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

