#include "pch.h"
#include "CInspector.h"
#include "CEditorCamera.h"
#include "CEditorScene.h"
#include "CEditorObject.h"
#include "CEditorWall.h"
#include "CEditorSpawnPoint.h"
#include "CEditorFloor.h"
#include "CEditorCeiling.h"
#include "CEditorDynamicFloor.h"


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

void CInspector::Render_FloorTextureUI(CEditorFloor* pFloor)
{
    ImGui::Text("Floor Texture");
    ImGui::Separator();

    CEditorDynamicFloor* pDynamicFloor = dynamic_cast<CEditorDynamicFloor*>(pFloor);

    if (CEditorDynamicFloor* pDynamicFloor = dynamic_cast<CEditorDynamicFloor*>(pFloor))
    {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Dynamic Floor (Animated)");
        ImGui::Separator();

        // Dynamic Floor Type 
        _uint iFloorType = pDynamicFloor->Get_FloorType();
        const char* szDynamicTypes[] = {
            "DYNAMIC_FLOOR_WATER",
            "DYNAMIC_FLOOR_LAVA",
            "DYNAMIC_FLOOR_ACID"
        };

        _int iComboIdx = 0;
        if (iFloorType == DYNAMIC_FLOOR_WATER)
            iComboIdx = 0;
        else if (iFloorType == DYNAMIC_FLOOR_LAVA)
            iComboIdx = 1;
        else if (iFloorType == DYNAMIC_FLOOR_ACID)
            iComboIdx = 2;

        if (ImGui::Combo("Dynamic Type", &iComboIdx, szDynamicTypes, IM_ARRAYSIZE(szDynamicTypes)))
        {
            // 콤보박스 인덱스를 enum 값으로 변환
            _uint eNewType = DYNAMIC_FLOOR_WATER;
            if (iComboIdx == 0) eNewType = DYNAMIC_FLOOR_WATER;
            else if (iComboIdx == 1) eNewType = DYNAMIC_FLOOR_LAVA;
            else if (iComboIdx == 2) eNewType = DYNAMIC_FLOOR_ACID;

            pDynamicFloor->Set_FloorType(eNewType);
        }

        // 애니메이션 속도 슬라이더
        _float fSpeed = pDynamicFloor->Get_AnimationSpeed();
        if (ImGui::SliderFloat("Animation Speed", &fSpeed, 0.1f, 3.0f))
        {
            pDynamicFloor->Set_AnimationSpeed(fSpeed);
        }

        // 재생/일시정지 체크박스
        bool bIsPlaying = pDynamicFloor->Is_Playing();
        if (ImGui::Checkbox("Playing", &bIsPlaying))
        {
            if (bIsPlaying)
                pDynamicFloor->Play_Animation();
            else
                pDynamicFloor->Pause_Animation();
        }

        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
            "Dynamic floors have animated textures");

    }
    else
    {
        // Floor Type 콤보 박스    (현재는 STATIC_FLOOR만 지원)
        _uint iFloorType = pFloor->Get_FloorType();
        const char* szFloorTypes[] = { "STATIC_FLOOR" };
        _int iSelectedType = 0;

        if (ImGui::Combo("Floor Type", &iSelectedType, szFloorTypes, IM_ARRAYSIZE(szFloorTypes)))
        {
            pFloor->Set_FloorType(iSelectedType);
        }

        int iTextureIdx = pFloor->Get_TextureIdx();
        if (ImGui::SliderInt("Texture Index", &iTextureIdx, 0, 7))
        {
            pFloor->Set_TextureIdx(iTextureIdx);
        }

        // 힌트 텍스트
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
            "Texture Index: 0 ~ 7 (FLOORS.dds 8x1 atlas)");
    }
}

void CInspector::Render_CeilingTextureUI(CEditorCeiling* pCeiling)
{
    ImGui::Text("Ceiling Texture");
    ImGui::Separator();

    // Ceiling Type 콤보박스 (STATIC_CEILING만 지원)
    _uint iCeilingType = pCeiling->Get_CeilingType();
    const char* szCeilingTypes[] = { "STATIC_CEILING" };
    int iSelectedType = 0;

    if (ImGui::Combo("Ceiling Type", &iSelectedType, szCeilingTypes, IM_ARRAYSIZE(szCeilingTypes)))
    {
        pCeiling->Set_CeilingType(iSelectedType);
    }

    // Texture Index 슬라이더 (0 ~ 7)
    int iTextureIdx = pCeiling->Get_TextureIdx();
    if (ImGui::SliderInt("Texture Index", &iTextureIdx, 0, 7))
    {
        pCeiling->Set_TextureIdx(iTextureIdx);
    }

    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
        "Texture Index: 0 ~ 7 (FLOORS.dds 8x1 atlas)");
}

void CInspector::Render_WallTextureUI(CEditorWall* pWall)
{
    ImGui::Text("Wall Texture");
    ImGui::Separator();

    // Wall Type 콤보박스 (STATIC_WALL_1 ~ STATIC_WALL_10)
    _uint iWallType = pWall->Get_WallType();
    const char* szWallTypes[] = {
        "STATIC_WALL_1",
        "STATIC_WALL_2",
        "STATIC_WALL_3",
        "STATIC_WALL_4",
        "STATIC_WALL_5",
        "STATIC_WALL_6",
        "STATIC_WALL_7",
        "STATIC_WALL_8",
        "STATIC_WALL_9",
        "STATIC_WALL_10"
    };

    int iSelectedType = iWallType;  // STATIC_WALL_1 = 0, STATIC_WALL_2 = 1, ...

    if (ImGui::Combo("Wall Type", &iSelectedType, szWallTypes, IM_ARRAYSIZE(szWallTypes)))
    {
        pWall->Set_WallType(iSelectedType);
    }

    // Texture Index 슬라이더 (0 ~ 2, 3x3 아틀라스의 행)
    int iTextureIdx = pWall->Get_TextureIdx();
    if (ImGui::SliderInt("Texture Index", &iTextureIdx, 0, 2))
    {
        pWall->Set_TextureIdx(iTextureIdx);
    }

    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
        "Texture Index: 0 ~ 2 (3x1 atlas row)");
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

    // Room Settings 
    if (ImGui::CollapsingHeader("Room Settings", ImGuiTreeNodeFlags_DefaultOpen))
    {
        _int iRoomIdx = pObj->Get_RoomIndex();
        if (ImGui::InputInt("Room Index", &iRoomIdx))
        {
            if (iRoomIdx >= 0)
            {
                pObj->Set_RoomIndex(iRoomIdx);
            }
        }

        ImGui::TextDisabled("Tip: Room 0 : Start, Room 1 = Next, etc.");
    }

    ImGui::Spacing();
    ImGui::Separator();

    // 텍스처 
    ImGui::Text("Texture");
    if (CEditorFloor* pFloor = dynamic_cast<CEditorFloor*>(pObj))
    {
        Render_FloorTextureUI(pFloor);
    }
    else if (CEditorCeiling* pCeiling = dynamic_cast<CEditorCeiling*>(pObj))
    {
        Render_CeilingTextureUI(pCeiling);
    }
    else if (CEditorWall* pWall = dynamic_cast<CEditorWall*>(pObj))
    {
        Render_WallTextureUI(pWall);
    }

}

void CInspector::Render_WallProperties(CEditorWall* pWall)
{
    if (pWall)
    {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Wall Properties");

        WALL_DIR eDir = pWall->Get_WallDirection();
        const char* items[] = { "XY Plane (Front)", "XY Plane (Back)" , "YZ Plane (Left)", "YZ Plane (Right)" };
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
        ImGui::TextDisabled("Available: WhiteMan, BeamMon, FlyMon");

        // 프리셋 버튼
        ImGui::Spacing();
        ImGui::Text("Presets:");
        if (ImGui::Button("WhiteMan"))
        {
            strcpy_s(szMonsterKey, "WhiteMan");
            pSpawn->Set_MonsterKey("WhiteMan");
        }
        else if (ImGui::Button("BeamMon"))
        {
            strcpy_s(szMonsterKey, "BeamMon");
            pSpawn->Set_MonsterKey("BeamMon");
        }
        else if (ImGui::Button("FlyMon"))
        {
            strcpy_s(szMonsterKey, "FlyMon");
            pSpawn->Set_MonsterKey("FlyMon");
        }
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