#include "pch.h"
#include "CMainMenuBar.h"

CMainMenuBar::CMainMenuBar()
    : m_bShowAbout(false)
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
        if (ImGui::MenuItem("New", "Ctrl+N"))
        {
            // Phase 3에서는 메시지만
            MessageBox(nullptr, L"New Map (Not Implemented)", L"Info", MB_OK);
        }

        if (ImGui::MenuItem("Open", "Ctrl+O"))
        {
            MessageBox(nullptr, L"Open Map (Not Implemented)", L"Info", MB_OK);
        }

        if (ImGui::MenuItem("Save", "Ctrl+S"))
        {
            MessageBox(nullptr, L"Save Map (Not Implemented)", L"Info", MB_OK);
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