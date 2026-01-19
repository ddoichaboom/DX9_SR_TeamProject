#pragma once
#include "CBase.h"
class CEditorCamera;
class CEffectScene;

class CEffectToolBar :
    public CBase
{
    explicit CEffectToolBar();
    virtual ~CEffectToolBar();

public:
    HRESULT             Ready_ToolBar(CEffectScene* _pEffectScene);
    void                Update_ToolBar();
    void                Render_ToolBar();

public:
    static CEffectToolBar* Create(CEffectScene* _pEffectScene);

private:
    virtual void        Free() override;

protected:
    CEffectScene*        m_pEffectScene;
};

