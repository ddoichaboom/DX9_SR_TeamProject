#include "pch.h"
#include "CHierarchy.h"
#include "CEditorScene.h"
#include "CEditorObject.h"

CHierarchy::CHierarchy()
    : m_pScene(nullptr)
    , m_iSelectedIndex(-1)
{
}

CHierarchy::~CHierarchy()
{
}


HRESULT CHierarchy::Ready_Hierarchy(CEditorScene* pScene)
{
    m_pScene = pScene;
    // AddRef 하지 않음 (CEditorApp이 관리)

    return S_OK;
}

void CHierarchy::Update_Hierarchy()
{
    // 현재는 업데이트 로직 없음
}

void CHierarchy::Render_Hierarchy()
{
    ImGui::Begin("Hierarchy");

    ImGui::Text("Scene Objects");
    ImGui::Separator();

    // Phase 3 - 하드코딩 된 오브젝트 목록 표시
    if (ImGui::TreeNodeEx("Scene", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // Camera
        if (ImGui::Selectable("Camera", m_iSelectedIndex == -1))
        {
            m_iSelectedIndex = -1;
        }

        // Grid
        if (ImGui::Selectable("Grid", m_iSelectedIndex == -2))
        {
            m_iSelectedIndex = -2;
        }

        // 실제 오브젝트 목록
        if (m_pScene)
        {
            auto& objectList = m_pScene->Get_ObjectList();

            int iIndex = 0;
            for (auto& pObj : objectList)
            {
                // 이름 가져오기
                wstring wstrName = pObj->Get_Name();

                // wstring -> string 변환
                string strName(wstrName.begin(), wstrName.end());

                // ID 중복 방지
                ImGui::PushID(iIndex);

                _bool bSelected = (m_iSelectedIndex == iIndex);

                // 항상 Push하고, 색상을 조건에 따라 변경
                ImVec4 textColor = bSelected ?
                    ImVec4(1.0f, 0.8f, 0.2f, 1.0f) :  // 선택됨: 노란색
                    ImGui::GetStyleColorVec4(ImGuiCol_Text);  // 기본 색상

                ImGui::PushStyleColor(ImGuiCol_Text, textColor);

                if (ImGui::Selectable(strName.c_str(), bSelected))
                {
                    m_iSelectedIndex = iIndex;
                    m_pScene->Set_SelectedObject(pObj);
                }

                ImGui::PopStyleColor();  // 항상 Pop

                ImGui::PopID();

                ++iIndex;

            }
        }

        ImGui::TreePop();
    }

    ImGui::Spacing();
    ImGui::Separator();

    // 삭제 버튼
    if (m_iSelectedIndex >= 0)
    {
        if (ImGui::Button("Delete Selected (Del)"))
        {
            // 선택된 오브젝트 삭제
            if (m_pScene)
            {
                CEditorObject* pObj = m_pScene->Get_SelectedObject();
                if (pObj)
                {
                    m_pScene->Remove_Object(pObj);
                    m_pScene->Set_SelectedObject(nullptr);
                    m_iSelectedIndex = -1;
                }
            }
        }
    }

    ImGui::End();

}

void CHierarchy::Sync_Selection(CEditorObject* pSelectedObj)
{
    if (!m_pScene)
    {
        m_iSelectedIndex = -1;
        return;
    }

    if (pSelectedObj == nullptr)
    {
        m_iSelectedIndex = -1;
        return;
    }

    auto& objectList = m_pScene->Get_ObjectList();
    _int iIndex = 0;

    for (auto& pObj : objectList)
    {
        if (pObj == pSelectedObj)
        {
            m_iSelectedIndex = iIndex;
            return;
        }
        ++iIndex;
    }

    m_iSelectedIndex = -1;
}

CHierarchy* CHierarchy::Create(CEditorScene* pScene)
{
    CHierarchy* pInstance = new CHierarchy;

    if (FAILED(pInstance->Ready_Hierarchy(pScene)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CHierarchy Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CHierarchy::Free()
{
    // m_pScene은 AddRef 안 했으므로 Release 안 함
}