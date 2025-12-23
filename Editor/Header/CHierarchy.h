#pragma once
#include "CBase.h"

class CEditorScene;
class CEditorObject;

class CHierarchy : public CBase
{
private:
    explicit CHierarchy();
    virtual ~CHierarchy();


public:
    HRESULT Ready_Hierarchy(CEditorScene* pScene);
    void    Update_Hierarchy();
    void    Render_Hierarchy();

    void    Sync_Selection(CEditorObject* pSelectedObj);

public:
    static CHierarchy* Create(CEditorScene* pScene);

private:
    CEditorScene* m_pScene;

    // 선택된 오브젝트 (Phase 5+)
    int             m_iSelectedIndex;

private:
    virtual void Free() override;

};

