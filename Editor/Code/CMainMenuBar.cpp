#include "pch.h"
#include "CMainMenuBar.h"
#include "CEditorScene.h"
#include "CFileIO.h"

CMainMenuBar::CMainMenuBar()
    : m_bShowAbout(false)
    , m_pScene(nullptr)
{
}

CMainMenuBar::~CMainMenuBar()
{
}

HRESULT CMainMenuBar::Ready_MenuBar()
{
    return S_OK;
}

void CMainMenuBar::Update_MenuBar()
{
    // 현재는 업데이트 로직 없음
}

void CMainMenuBar::Render_MenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_N, ImGuiInputFlags_RouteGlobal))
            Handle_NewMap();

        if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S, ImGuiInputFlags_RouteGlobal))
            Handle_SaveMap();

        if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_O, ImGuiInputFlags_RouteGlobal))
            Handle_OpenMap();

        Render_FileMenu();
        Render_EditMenu();
        Render_ViewMenu();
        Render_HelpMenu();

        ImGui::EndMainMenuBar();
    }

    // About 다이얼로그
    if (m_bShowAbout)
    {
        ImGui::Begin("About Editor", &m_bShowAbout);
        ImGui::Text("Map Editor v1.0");
        ImGui::Text("Made By DDOICHABOOM");
        ImGui::Separator();
        ImGui::Text("Built with ImGui + DirectX 9");
        ImGui::End();
    }
}

void CMainMenuBar::Render_FileMenu()
{
    if (ImGui::BeginMenu("File"))
    {
        // New Map
        if (ImGui::MenuItem("New", "Ctrl+N"))
        {
            Handle_NewMap();
        }

        ImGui::Separator();

        // Save Map
        if (ImGui::MenuItem("Save", "Ctrl+S"))
        {
            Handle_SaveMap();
        }

       
        // Open Map
        if (ImGui::MenuItem("Open", "Ctrl+O"))
        {
            Handle_OpenMap();
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Exit", "Alt+F4"))
        {
            PostQuitMessage(0);
        }

        ImGui::EndMenu();
    }
}

void CMainMenuBar::Render_EditMenu()
{
    if (ImGui::BeginMenu("Edit"))
    {
        if (ImGui::MenuItem("Undo", "Ctrl+Z"))
        {
            // Phase 5+
        }

        if (ImGui::MenuItem("Redo", "Ctrl+Y"))
        {
            // Phase 5+
        }

        ImGui::EndMenu();
    }
}

void CMainMenuBar::Render_ViewMenu()
{
    if (ImGui::BeginMenu("View"))
    {
        // Phase 3에서는 윈도우 표시/숨김 토글 추가 예정
        ImGui::MenuItem("Scene View", nullptr, nullptr, false);  // 비활성화
        ImGui::MenuItem("Hierarchy", nullptr, nullptr, false);
        ImGui::MenuItem("Inspector", nullptr, nullptr, false);

        ImGui::EndMenu();
    }
}

void CMainMenuBar::Render_HelpMenu()
{
    if (ImGui::BeginMenu("Help"))
    {
        if (ImGui::MenuItem("About"))
        {
            m_bShowAbout = true;
        }

        ImGui::EndMenu();
    }
}

void CMainMenuBar::Handle_NewMap()
{
    if (!m_pScene)
    {
        MessageBox(nullptr, L"Scene is not Set", L"Error", MB_OK);
    }
    else
    {
        _int result = MessageBox(nullptr,
            L"Clear current Map?",
            L"New Map",
            MB_OKCANCEL | MB_ICONQUESTION);

        if (IDOK == result)
        {
            m_pScene->Clear_AllObjects();
            MessageBox(nullptr, L"Map Cleared", L"New Map", MB_OK);
        }
    }
}

void CMainMenuBar::Handle_SaveMap()
{
    if (!m_pScene)
    {
        MessageBox(nullptr, L"Scene is not Set", L"Error", MB_OK);
    }
    else
    {
        wchar_t wszPath[MAX_PATH] = L"";

        OPENFILENAME ofn;
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = g_hWnd;
        ofn.lpstrFilter = L"JSON Map Files (*.json)\0*.json\0All Files (*.*)\0*.*\0";
        ofn.lpstrFile = wszPath;
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrDefExt = L"json";
        ofn.lpstrTitle = L"Save Map";
        ofn.Flags = OFN_OVERWRITEPROMPT;

        if (GetSaveFileName(&ofn))
        {
            if (FAILED(CFileIO::GetInstance()->Save_MapData(wszPath, m_pScene)))
            {
                MessageBox(nullptr, L"Failed to save map", L"Error", MB_OK | MB_ICONERROR);
            }
        }
    }
}

void CMainMenuBar::Handle_OpenMap()
{
    if (!m_pScene)
    {
        MessageBox(nullptr, L"Scene is not set", L"Error", MB_OK);
    }
    else
    {
        wchar_t wszPath[MAX_PATH] = L"";

        OPENFILENAME ofn;
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = g_hWnd;
        ofn.lpstrFilter = L"JSON Map Files (*.json)\0*.json\0All Files (*.*)\0*.*\0";
        ofn.lpstrFile = wszPath;
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrTitle = L"Open Map";
        ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

        if (GetOpenFileName(&ofn))
        {
            LPDIRECT3DDEVICE9 pGraphicDev = m_pScene->Get_GraphicDev();

            if (FAILED(CFileIO::GetInstance()->Load_MapData(wszPath, m_pScene, pGraphicDev)))
            {
                MessageBox(nullptr, L"Failed to load map", L"Error", MB_OK | MB_ICONERROR);
            }
        }
    }
}

CMainMenuBar* CMainMenuBar::Create()
{
    CMainMenuBar* pInstance = new CMainMenuBar;

    if (FAILED(pInstance->Ready_MenuBar()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CMainMenuBar Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CMainMenuBar::Free()
{
    // 정리할 리소스 없음
}