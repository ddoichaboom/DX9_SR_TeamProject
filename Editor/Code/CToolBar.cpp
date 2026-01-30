#include "pch.h"
#include "CToolBar.h"
#include "CEditorCamera.h"
#include "CGrid.h"
#include "CEffectToolBar.h"

CToolBar::CToolBar()
    : m_pCamera(nullptr)
    , m_pGrid(nullptr)
    , m_bShowGrid(true)
    , m_eEditorMode(MODE_SELECT)
{
}

CToolBar::~CToolBar()
{
}

HRESULT CToolBar::Ready_ToolBar(CEditorCamera* pCamera, CGrid* pGrid)
{
    m_pCamera = pCamera;
    m_pGrid = pGrid;

    // AddRef 하지 않음 (CEditorScene에서 관리)

    return S_OK;
}


void CToolBar::Update_ToolBar()
{
    // 현재는 업데이트 로직 없음
}

void CToolBar::Render_ToolBar()
{
    ImGui::Begin("Tool Bar", nullptr, ImGuiWindowFlags_NoCollapse);

    // 배치 모드 
    ImGui::Text("Editor Mode:");
    ImGui::Separator();

    const char* szModeNames[] = {
          "Select",
          "Place Floor",
          "Place Dynamic Floor",
          "Place Slope Floor",
          "Place Ceiling",
          "Place VendingMachine",
          "Place Wall",
          "Place Dynamic Wall",
          "Spawn Player",
          "Spawn Monster",
          "Spawn BossMonster",
          "Place MapCollider",
          "Place_Door",
          "Place TriggerBox",
          "Place InteractObject",
          "Place DisplayObject"
    };

    int iCurrentMode = (int)m_eEditorMode;

    if (ImGui::Combo("##EditorModeCombo", &iCurrentMode, szModeNames, IM_ARRAYSIZE(szModeNames)))
    {
        m_eEditorMode = (EDITOR_MODE)iCurrentMode;
    }


    ImGui::Spacing();
    ImGui::Separator();

    // 카메라 모드 
    ImGui::Text("Camera Mode: ");
    ImGui::Separator();

    // 카메라 모드 선택
    if (m_pCamera)
    {
        CAMERA_MODE     eMode = m_pCamera->Get_CameraMode();

        if (ImGui::RadioButton("Free Camera", eMode == MODE_FREE))
        {
            m_pCamera->Set_CameraMode(MODE_FREE);
        }

        ImGui::SameLine();

        if (ImGui::RadioButton("FPS Camera", eMode == MODE_FPS))
        {
            m_pCamera->Set_CameraMode(MODE_FPS);
        }

        // 현재 카메라 모드 표시
        const char* szModeName = "";
        switch (eMode)
        {
        case MODE_FREE:
            szModeName = "Free (3D Flying)";
            break;

        case MODE_FPS:
            szModeName = "FPS (Ground Level)";
            break;

        default:
            szModeName = "UnKnown";
            break;
        }

        ImGui::Text("Current: %s", szModeName);
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Grid:");

    // 그리드 토글
    if (ImGui::Checkbox("Show Grid", &m_bShowGrid))
    {
        if (m_pGrid)
            m_pGrid->Set_Visible(m_bShowGrid);
    }

    ImGui::Spacing();
    ImGui::Separator();

    // 카메라 속성
    if (m_pCamera && ImGui::CollapsingHeader("Camera Settings"))
    {
        _float fSpeed = m_pCamera->Get_CameraSpeed();
        if (ImGui::SliderFloat("Speed", &fSpeed, 0.1f, 10.f))
        {
            m_pCamera->Set_CameraSpeed(fSpeed);
        }

        _float fSensitivity = m_pCamera->Get_Sensitivity();
        if (ImGui::SliderFloat("Mouse Sensitivity", &fSensitivity, 0.01f, 0.5f))
        {
            m_pCamera->Set_Sensitivity(fSensitivity);
        }
    }

    ImGui::End();
}

CToolBar* CToolBar::Create(CEditorCamera* pCamera, CGrid* pGrid)
{
    CToolBar* pInstance = new CToolBar;

    if (FAILED(pInstance->Ready_ToolBar(pCamera, pGrid)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CToolBar Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CToolBar::Free()
{
    // m_pCamera, m_pGrid는 AddRef 안 했으므로 Release 안 함
}