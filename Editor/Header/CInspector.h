#pragma once
#include "CBase.h"

class CEditorCamera;
class CEditorScene;

class CInspector : public CBase
{
private:
    explicit CInspector();
    virtual ~CInspector();

public:
    HRESULT Ready_Inspector(CEditorCamera* pCamera, CEditorScene* pScene);
    void    Update_Inspector();
    void    Render_Inspector();

private:
    void    Render_CameraProperties();
    void    Render_GridProperties();

private:
    void    Render_ObjectProperties();

public:
    static CInspector* Create(CEditorCamera* pCamera, CEditorScene* pScene);

    void    Set_SelectedType(int iType) { m_iSelectedType = iType; }

private:
    CEditorCamera* m_pCamera;

    // 선택된 오브젝트 타입 (임시)
    int             m_iSelectedType;

    CEditorScene* m_pScene;

private:
    virtual void Free() override;

};

