#include "pch.h"
#include "CInspector.h"
#include "CEditorCamera.h"
#include "CEditorScene.h"
#include "CEditorObject.h"
#include "CEditorWall.h"
#include "CEditorSpawnPoint.h"


CInspector::CInspector()
    : m_pCamera(nullptr)
    , m_iSelectedType(0)
{
}

CInspector::~CInspector()
{
}

HRESULT CInspector::Ready_Inspector(CEditorCamera* pCamera, CEditorScene* pScene)
{
    m_pCamera = pCamera;
    m_pScene = pScene;
    // AddRef 하지 않음

    return S_OK;
}

void CInspector::Update_Inspector()
{
    // 현재는 업데이트 로직 없음
}

void CInspector::Render_Inspector()
{
    ImGui::Begin("Inspector");

    // 선택된 오브젝트 표시
    if (m_pScene)
    {
        CEditorObject* pObj = m_pScene->Get_SelectedObject();

        if (pObj)
        {
            Render_ObjectProperties();
        }
        else
        {
            // 카메라 또는 그리드 선택
            if (m_iSelectedType == 0)
            {
                Render_CameraProperties();
            }
            else if (m_iSelectedType == 1)
            {
                Render_GridProperties();
            }
            else
            {
                ImGui::Text("No Object Selected");
            }
        }
    }
    else
    {
        ImGui::Text("No Object Selected");
    }

    ImGui::End();
}

void CInspector::Render_CameraProperties()
{
    ImGui::Text("Camera");
    ImGui::Separator();

    if (m_pCamera)
    {
        // 카메라 모드
        CAMERA_MODE     eMode = m_pCamera->Get_CameraMode();
        const char* szMode = (eMode == MODE_FREE) ? "FREE" : "FPS";
        ImGui::Text("Mode: %s", szMode);

        ImGui::Spacing();

        // Transform ( 읽기 전용 )
        ImGui::Text("Transform");
        ImGui::Separator();
        ImGui::Text("Position : (Not Implemented)");
        ImGui::Text("Rotation : (Not Implemented)");

        ImGui::Spacing();

        // 카메라 속성
        ImGui::Text("Camera Properties");
        ImGui::Separator();

        // Question : fSpeed랑 fSensitivity 모두 static으로 선언해야하나?
        // 1. 카메라 속도 
        _float fSpeed = m_pCamera->Get_CameraSpeed();
        if (ImGui::SliderFloat("Speed", &fSpeed, 0.1f, 10.0f))
        {
            m_pCamera->Set_CameraSpeed(fSpeed);
        }

        // 2. 마우스 감도
        _float fSensitivity = m_pCamera->Get_Sensitivity();
        if (ImGui::SliderFloat("Sensitivity", &fSensitivity, 0.01f, 0.5f))
        {
            m_pCamera->Set_Sensitivity(fSensitivity);
        }
    }
}

void CInspector::Render_GridProperties()
{
    ImGui::Text("Grid");
    ImGui::Separator();

    ImGui::Text("Size : 100 x 100");
    ImGui::Text("Interval : 1.0");
    ImGui::Text("Color : ( 100, 100, 100)");

    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
        "[ Phase 3 ] Read - Only Properties");
}

void CInspector::Render_ObjectProperties()
{
    if (!m_pScene)
        return;

    CEditorObject* pObj = m_pScene->Get_SelectedObject();

    if (!pObj)
        return;

    // 이름
    wstring wstrName = pObj->Get_Name();
    string strName(wstrName.begin(), wstrName.end());

    ImGui::Text("Object: %s", strName.c_str());
    ImGui::Separator();

    // Transform
    ImGui::Text("Transform");
    ImGui::Separator();

    _vec3 vPos = pObj->Get_Position();
    _vec3 vRot = pObj->Get_Rotation();
    _vec3 vScale = pObj->Get_Scale();

    // Position 
    _float fPos[3] = { vPos.x, vPos.y, vPos.z };
    if (ImGui::DragFloat3("Position", fPos, 0.1f))
    {
        pObj->Set_Position(_vec3(fPos[0], fPos[1], fPos[2]));
    }

    // Rotation
    _float fRot[3] = { vRot.x, vRot.y, vRot.z };
    if (ImGui::DragFloat3("Rotation", fRot, 1.0f))
    {
        pObj->Set_Rotation(_vec3(fRot[0], fRot[1], fRot[2]));
    }

    // Scale
    _float fScale[3] = { vScale.x, vScale.y, vScale.z };
    if (ImGui::DragFloat3("Scale", fScale, 0.1f, 0.1f, 10.f))
    {
        pObj->Set_Scale(_vec3(fScale[0], fScale[1], fScale[2]));
    }

    wstring cObjName = pObj->Get_Name();

    if (cObjName == L"Wall")
    {
        CEditorWall* pWall = dynamic_cast<CEditorWall*>(pObj);
        Render_WallProperties(pWall);
    }
    else if (cObjName == L"MonsterSpawn")
    {
        CEditorSpawnPoint* pSpawn = dynamic_cast<CEditorSpawnPoint*>(pObj);
        SPAWN_TYPE eType = pSpawn->Get_SpawnType();
        if (eType == SPAWN_MONSTER)
        {
            Render_MonsterSpawnPointProperties(pSpawn);
        }
    }
    



    ImGui::Spacing();
    ImGui::Separator();

    // 텍스처 
    ImGui::Text("Texture: None");
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
        "[Phase 5] Texture selection");

}

void CInspector::Render_WallProperties(CEditorWall* pWall)
{
    if (pWall)
    {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Wall Properties");

        WALL_DIR eDir = pWall->Get_WallDirection();
        const char* items[] = { "XY Plane (Front/Back)", "YZ Plane (Left/Right)" };
        int iCurrentDir = (int)eDir;

        if (ImGui::Combo("Direction", &iCurrentDir, items, IM_ARRAYSIZE(items)))
        {
            pWall->Set_WallDirection((WALL_DIR)iCurrentDir);
        }
    }
}

void CInspector::Render_MonsterSpawnPointProperties(CEditorSpawnPoint* pSpawn)
{
    if (pSpawn)
    {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Monster SpawnPoint Properties");

        ImGui::Spacing();

        // MonsterKey 입력 버퍼 (static으로 유지)
        static char szMonsterKey[64] = "";

        // 처음 선택 시 기존 값 복사
        const string& strCurrentKey = pSpawn->Get_MonsterKey();
        if (strCurrentKey.length() < 64)
        {
            strcpy_s(szMonsterKey, strCurrentKey.c_str());
        }

        // 입력 필드
        if (ImGui::InputText("Monster Key", szMonsterKey, IM_ARRAYSIZE(szMonsterKey)))
        {
            pSpawn->Set_MonsterKey(string(szMonsterKey));
        }

        // 도움말 텍스트
        ImGui::TextDisabled("Available: WhiteMan");

        // 프리셋 버튼
        ImGui::Spacing();
        ImGui::Text("Presets:");
        if (ImGui::Button("WhiteMan"))
        {
            strcpy_s(szMonsterKey, "WhiteMan");
            pSpawn->Set_MonsterKey("WhiteMan");
        }
        // 여기에 구현 몬스터 추가 
    }
}

CInspector* CInspector::Create(CEditorCamera* pCamera, CEditorScene* pScene)
{
    CInspector* pInstance = new CInspector;

    if (FAILED(pInstance->Ready_Inspector(pCamera, pScene)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CInspector Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CInspector::Free()
{
    // m_pCamera는 AddRef 안 했으므로 Release 안 함
}