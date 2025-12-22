#pragma once
#include "CBase.h"
#include "CGraphicDev.h"
#include "CManagement.h"

class CEditorScene;
class CMainMenuBar;
class CToolBar;
class CHierarchy;
class CInspector;


class CEditorApp : public CBase
{
private:
    explicit CEditorApp();
    virtual ~CEditorApp();

public:
    HRESULT Ready_Editor();
    void    Update_Editor(const _float& fTimeDelta);
    void    Render_Editor();

private:
    HRESULT Ready_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev, HWND hWnd, HINSTANCE hInst);
    HRESULT Ready_ImGui(HWND hWnd);
    HRESULT Ready_Prototype(LPDIRECT3DDEVICE9 pGraphicDev);
    HRESULT Ready_Scene();

public:
    static CEditorApp* Create(HWND hWnd, HINSTANCE hInst);

private:
    // Engine
    LPDIRECT3DDEVICE9       m_pGraphicDev;
    Engine::CGraphicDev* m_pDeviceClass;
    Engine::CManagement* m_pManagementClass;
    CEditorScene* m_pCurrentScene;

    // UI Componenets
    CMainMenuBar* m_pMainMenuBar;
    CToolBar* m_pToolBar;
    CHierarchy* m_pHierarchy;
    CInspector* m_pInspector;


    // ImGui State
    bool                    m_bShowDemo;
    ImVec4                  m_vClearColor;

private:
    virtual void Free();
};

