#include "pch.h"
#include "CInspector.h"
#include "CEditorCamera.h"
#include "CEditorScene.h"
#include "CEditorObject.h"
#include "CEditorWall.h"
#include "CEditorDynamicWall.h"
#include "CEditorSpawnPoint.h"
#include "CEditorFloor.h"
#include "CEditorSlopeFloor.h"
#include "CEditorCeiling.h"
#include "CEditorDynamicFloor.h"
#include "CTexture.h"
#include "CEditorMapCollider.h"
#include "CEditorTriggerBox.h"
#include "CSelectionMgr.h"
#include "CEditorDoor.h"
#include "CEditorVendingMachine.h"
#include "CEditorInteractObject.h"
#include "CEditorDisplayObject.h"
#include "CEditorDisplayCubeObject.h"
#include "CEditorWindow.h"


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
                //Render_GridProperties();
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



void CInspector::Render_FloorTextureUI(CEditorFloor* pFloor)
{
    ImGui::Text("Floor Texture");
    ImGui::Separator();


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

        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
            "Dynamic floors have animated textures");

    }
    else if (CEditorSlopeFloor* pSlopeFloor = dynamic_cast<CEditorSlopeFloor*>(pFloor))
    {
        ImGui::Text("Slope Floor Properties");
        ImGui::Separator();

        // 경사 각도 슬라이더
        _float fSlopeAngle = pSlopeFloor->Get_SlopeAngle();
        if (ImGui::SliderFloat("Slope Angle", &fSlopeAngle, 0.f, 89.f))
        {
            pSlopeFloor->Set_SlopeAngle(fSlopeAngle);
        }

        // 경사 방향 콤보박스
        SLOPE_DIR eSlopeDir = pSlopeFloor->Get_SlopeDirection();
        const char* szSlopeDirs[] = {
            "+X Direction",
            "-X Direction",
            "+Z Direction",
            "-Z Direction"
        };

        int iSlopeDir = static_cast<int>(eSlopeDir);
        if (ImGui::Combo("Slope Direction", &iSlopeDir, szSlopeDirs, IM_ARRAYSIZE(szSlopeDirs)))
        {
            pSlopeFloor->Set_SlopeDirection(static_cast<SLOPE_DIR>(iSlopeDir));
        }

        ImGui::Spacing();
        ImGui::Separator();

        // 텍스처 설정 (CEditorFloor와 유사)
        ImGui::Text("Texture Settings");
        ImGui::Separator();

        _uint iFloorType = pSlopeFloor->Get_FloorType();
        const char* szSlopeFloorTypes[] = {
            "STATIC_FLOOR",
            "STATIC_FLOOR_FLUID",
            "STATIC_FLOOR_SLOPE",
        };

        _int iTypeIdx(0);
        if (iFloorType == STATIC_FLOOR)
            iTypeIdx = 0;
        else if (iFloorType == STATIC_FLOOR_FLUID)
            iTypeIdx = 1;
        else if (iFloorType == STATIC_FLOOR_SLOPE)
            iTypeIdx = 2;

        if (ImGui::Combo("Floor Type", &iTypeIdx, szSlopeFloorTypes, IM_ARRAYSIZE(szSlopeFloorTypes)))
        {
            // 콤보박스 인덱스를 enum 값으로 변환
            _int eNewType = STATIC_FLOOR;
            if (iTypeIdx == 0)
                eNewType = STATIC_FLOOR;
            else if (iTypeIdx == 1)
                eNewType = STATIC_FLOOR_FLUID;
            else if (iTypeIdx == 2)
                eNewType = STATIC_FLOOR_SLOPE;

            pSlopeFloor->Set_FloorType(eNewType);
        }

        // Texture Index 슬라이더
        _int iTextureIdx = pSlopeFloor->Get_TextureIdx();

        Engine::CTexture* pTextureCom = dynamic_cast<Engine::CTexture*>(
            pSlopeFloor->Get_Component(ID_DYNAMIC, L"Com_Texture"));

        _int iMaxIdx(0);

        if (pTextureCom)
        {
            Engine::TextureDesc* pDesc = pTextureCom->GetTextureDesc(iFloorType);
            if (pDesc)
            {
                iMaxIdx = (_int)pDesc->vMaxIdx.x;
            }
        }
        if (ImGui::SliderInt("Texture Index", &iTextureIdx, 0, iMaxIdx))
        {
            pSlopeFloor->Set_TextureIdx(iTextureIdx);
        }

        pSlopeFloor->Calculate_Rotation();


        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
            "Slope floors use rotation to create inclines");
    }
    else if (pFloor)
    {
        // Floor Type 콤보 박스    (현재는 STATIC_FLOOR만 지원)
        _uint iFloorType = pFloor->Get_FloorType();
        const char* szFloorTypes[] = { 
            "STATIC_FLOOR",
            "STATIC_FLOOR_FLUID",
            "STATIC_FLOOR_SLOPE",
            "STATIC_FLOOR_ROAD",
            "STATIC_FLOOR_UNIQUE"
        };

        _int iSelectedType = iFloorType;

        if (ImGui::Combo("Floor Type", &iSelectedType, szFloorTypes, IM_ARRAYSIZE(szFloorTypes)))
        {
            pFloor->Set_FloorType(iSelectedType);
        }

        _int iTextureIdx = pFloor->Get_TextureIdx();

        Engine::CTexture* pTextureCom = dynamic_cast<Engine::CTexture*>(
            pFloor->Get_Component(ID_DYNAMIC, L"Com_Texture"));

        _int iMaxIdx(0);

        if (pTextureCom)
        {
            Engine::TextureDesc* pDesc = pTextureCom->GetTextureDesc(iFloorType);
            if (pDesc)
            {
                iMaxIdx = (_int)pDesc->vMaxIdx.x;
            }
        }

        if (ImGui::SliderInt("Texture Index", &iTextureIdx, 0, iMaxIdx))
        {
            pFloor->Set_TextureIdx(iTextureIdx);
        }
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

    Engine::CTexture* pTextureCom = dynamic_cast<Engine::CTexture*>(
        pCeiling->Get_Component(ID_DYNAMIC, L"Com_Texture"));

    _int iMaxIdx(0);

    if (pTextureCom)
    {
        Engine::TextureDesc* pDesc = pTextureCom->GetTextureDesc(iCeilingType);
        if (pDesc)
        {
            iMaxIdx = (_int)pDesc->vMaxIdx.x;
        }
    }

    if (ImGui::SliderInt("Texture Index", &iTextureIdx, 0, iMaxIdx))
    {
        pCeiling->Set_TextureIdx(iTextureIdx);
    }

    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
        "Texture Index: 0 ~ 7 (FLOORS.dds 8x1 atlas)");
}

void CInspector::Render_WallTextureUI(CEditorWall* pWall)
{
    if (!pWall)
        return;

    ImGui::Text("Wall Texture");
    ImGui::Separator();

    // DynamicWall인지 먼저 확인
    CEditorDynamicWall * pDynamicWall = dynamic_cast<CEditorDynamicWall*>(pWall);

    if (pDynamicWall)
    {
        // ========== Dynamic Wall UI ==========
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Dynamic Wall (Animated)");
        ImGui::Separator();

        _uint iWallType = pDynamicWall->Get_WallType();

        // DynamicWall 타입 목록
        const char* szDynamicTypes[] = {
            "FAN",
            "FAN_BLOOD"
        };

        // 현재 타입에서 콤보 인덱스 계산
        _int iComboIdx = 0;
        if (iWallType >= DYNAMIC_WALL_FAN)
            iComboIdx = iWallType - DYNAMIC_WALL_FAN;

        // 범위 체크
        if (iComboIdx < 0 || iComboIdx >= IM_ARRAYSIZE(szDynamicTypes))
            iComboIdx = 0;

        if (ImGui::Combo("Dynamic Type", &iComboIdx, szDynamicTypes, IM_ARRAYSIZE(szDynamicTypes)))
        {
            _uint eNewType = DYNAMIC_WALL_FAN + iComboIdx;
            pDynamicWall->Set_WallType(eNewType);
        }

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Animated texture");
    }
    else
    {
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
            "STATIC_WALL_10",
            "STATIC_WALL_WATER",
            "STATIC_WALL_LAVA",
            "STATIC_WALL_ACID",
            "STATIC_WALL_FENCE",
            "STATIC_WALL_SIDEDASH",
            "STATIC_WALL_CORNER",
            "STATIC_WALL_DECO"
        };

        int iSelectedType = iWallType;  // STATIC_WALL_1 = 0, STATIC_WALL_2 = 1, ...

        if (ImGui::Combo("Wall Type", &iSelectedType, szWallTypes, IM_ARRAYSIZE(szWallTypes)))
        {
            pWall->Set_WallType(iSelectedType);
        }

        // Texture Index 슬라이더 (0 ~ 2, 3x3 아틀라스의 행)
        _int iTextureIdx = pWall->Get_TextureIdx();

        Engine::CTexture* pTextureCom = dynamic_cast<Engine::CTexture*>(
            pWall->Get_Component(ID_DYNAMIC, L"Com_Texture"));

        _int iMaxIdx(0);

        if (pTextureCom)
        {
            Engine::TextureDesc* pDesc = pTextureCom->GetTextureDesc(iWallType);
            if (pDesc)
            {
                iMaxIdx = (_int)pDesc->vMaxIdx.x;
            }
        }

        if (ImGui::SliderInt("Texture Index", &iTextureIdx, 0, iMaxIdx))
        {
            pWall->Set_TextureIdx(iTextureIdx);
        }

        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
            "Texture Index: 0 ~ 2 (3x1 atlas row)");
    }
}

void CInspector::Render_MapColliderProperties(CEditorMapCollider* pCollider)
{
    if (!pCollider)
        return;

    ImGui::Text("MapCollider Properties");
    ImGui::Separator();

    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Collision Box (Invisible in Game)");

    ImGui::Spacing();

    // Collider Scale (Transform Scale과 별도)
    _vec3 vColliderScale = pCollider->Get_ColliderScale();
    _float fScale[3] = { vColliderScale.x, vColliderScale.y, vColliderScale.z };
    if (ImGui::DragFloat3("Collider Scale", fScale, 0.5f, 1.0f, 200.f))
    {
        pCollider->Set_ColliderScale(_vec3(fScale[0], fScale[1], fScale[2]));
    }

    ImGui::Spacing();

    // 트리거 타입 선택
    COLLIDER_TAG eColliderTag = pCollider->Get_ColliderTag();
    const char* szColliderTags[] = {
        "TAG_NONE",
        "TAG_SIDE_DASH_X",
        "TAG_SIDE_DASH_Z",
        "TAG_FAN",
        "TAG_ELECTRIC",
        "TAG_ROAD_END"
    };

    int iSelectedTag(0);

    switch (eColliderTag)
    {
    case TAG_NONE:
        iSelectedTag = 0;
        break;
    case TAG_SIDE_DASH_X:
        iSelectedTag = 1;
        break;
    case TAG_SIDE_DASH_Z:
        iSelectedTag = 2;
        break;
    case TAG_FAN:
        iSelectedTag = 3;
        break;
    case TAG_ELECTRIC:
        iSelectedTag = 4;
        break;
    case TAG_ROAD_END:
        iSelectedTag = 5;
        break;
    default:
        iSelectedTag = 0;
        break;
    }

    if (ImGui::Combo("Collider Tag", &iSelectedTag, szColliderTags, IM_ARRAYSIZE(szColliderTags)))
    {
        switch (iSelectedTag)
        {
        case 0:
            pCollider->Set_ColliderTag(TAG_NONE);
            break;
        case 1:
            pCollider->Set_ColliderTag(TAG_SIDE_DASH_X);
            break;
        case 2:
            pCollider->Set_ColliderTag(TAG_SIDE_DASH_Z);
            break;
        case 3:
            pCollider->Set_ColliderTag(TAG_FAN);
            break;
        case 4:
            pCollider->Set_ColliderTag(TAG_ELECTRIC);
            break;
        case 5:
            pCollider->Set_ColliderTag(TAG_ROAD_END);
            break;
        }
    }
}

void CInspector::Render_TriggerBoxProperties(CEditorTriggerBox* pTrigger)
{
    if (!pTrigger)
        return;

    ImGui::Text("TriggerBox Properties");
    ImGui::Separator();

    // Collider Scale
    _vec3 vColliderScale = pTrigger->Get_ColliderScale();
    _float fScale[3] = { vColliderScale.x, vColliderScale.y, vColliderScale.z };
    if (ImGui::DragFloat3("Collider Scale", fScale, 0.5f, 1.0f, 200.f))
    {
        pTrigger->Set_ColliderScale(_vec3(fScale[0], fScale[1], fScale[2]));
    }

    ImGui::Spacing();

    // 트리거 타입 선택
    TRIGGER_TYPE eType = pTrigger->Get_TriggerType();
    const char* szTriggerTypes[] = {
        "Door (Room Transition)",
        "Event (STAGE_END)",
    };

    int iSelectedType = static_cast<int>(eType);
    if (ImGui::Combo("Trigger Type", &iSelectedType, szTriggerTypes, IM_ARRAYSIZE(szTriggerTypes)))
    {
        pTrigger->Set_TriggerType(static_cast<TRIGGER_TYPE>(iSelectedType));
    }


    ImGui::Spacing();

    // 편의 기능: 빠른 크기 프리셋
    ImGui::Text("Quick Size Presets:");
    if (ImGui::Button("Small (8x8x8)"))
    {
        pTrigger->Set_ColliderScale(_vec3(8.f, 8.f, 8.f));
    }
    ImGui::SameLine();
    if (ImGui::Button("Medium (16x16x16)"))
    {
        pTrigger->Set_ColliderScale(_vec3(16.f, 16.f, 16.f));
    }
    ImGui::SameLine();
    if (ImGui::Button("Large (32x32x32)"))
    {
        pTrigger->Set_ColliderScale(_vec3(32.f, 32.f, 32.f));
    }
}

void CInspector::Render_DoorProperties(CEditorDoor* pDoor)
{
    if (!pDoor)
        return;

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Door Properties");

    ImGui::Spacing();

    // ========== Door Type Combo ==========
    DOOR_TYPE eDoorType = pDoor->Get_DoorType();
    const char* szDoorTypes[] = {
        "DOOR_1",
        "DOOR_2",
        "DOOR_3",
        "DOOR_ELEVATOR"
    };

    int iCurrentType = static_cast<int>(eDoorType);
    if (ImGui::Combo("Door Type", &iCurrentType, szDoorTypes, IM_ARRAYSIZE(szDoorTypes)))
    {
        pDoor->Set_DoorType(static_cast<DOOR_TYPE>(iCurrentType));
    }

    ImGui::Spacing();

    // ========== Door ID Input ==========
    _int iDoorID = pDoor->Get_DoorID();
    if (ImGui::InputInt("Door ID", &iDoorID))
    {
        // 음수 방지
        if (iDoorID >= 0)
        {
            pDoor->Set_DoorID(iDoorID);
        }
    }
}

void CInspector::Render_ObjectProperties()
{
    if (!m_pScene)
        return;

    CEditorObject* pObj = m_pScene->Get_SelectedObject();
    list<CEditorObject*>& SelectedList = m_pScene->Get_SelectedObjects();

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


    if ( vPos.x != fPos[0] || vPos.y != fPos[1] || vPos.z != fPos[2] ||
         vRot.x != fRot[0] || vRot.y != fRot[1] || vRot.z != fRot[2] ||
        vScale.x != fScale[0] || vScale.y != fScale[1] || vScale.z != fScale[2])

    {
        _vec3 fPosDelta = { fPos[0] - vPos.x , fPos[1] - vPos.y, fPos[2] - vPos.z };
        _vec3 fRotDelta = { fRot[0] - vRot.x , fRot[1] - vRot.y ,fRot[2] - vRot.z };
        _vec3 fScaleDelta = { fScale[0] - vScale.x , fScale[1] - vScale.y , fScale[2] - vScale.z };

        for (auto& pSelectedObj : SelectedList)
        {
            if (pObj == pSelectedObj)
                continue;

            vPos = pSelectedObj->Get_Position();
            vRot = pSelectedObj->Get_Rotation();
            vScale = pSelectedObj->Get_Scale();

            vPos += fPosDelta;
            vRot += fRotDelta;
            vScale += fScaleDelta;

            pSelectedObj->Set_Position(vPos);
            pSelectedObj->Set_Rotation(vRot);
            pSelectedObj->Set_Scale(vScale);
        }
    }


    wstring cObjName = pObj->Get_Name();

    if (cObjName == L"Wall" || cObjName == L"DynamicWall")
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

    if (CEditorMapCollider* pMapCollider = dynamic_cast<CEditorMapCollider*>(pObj))
    {
        Render_MapColliderProperties(pMapCollider);
    }
    else if (CEditorTriggerBox* pTriggerBox = dynamic_cast<CEditorTriggerBox*>(pObj))
    {
        Render_TriggerBoxProperties(pTriggerBox);
    }

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
    else if (CEditorDoor* pDoor = dynamic_cast<CEditorDoor*>(pObj))
    {
        Render_DoorProperties(pDoor);
    }
    else if (CEditorInteractObject* pInteract = dynamic_cast<CEditorInteractObject*>(pObj))
    {
        Render_InteractObjectProperties(pInteract);
    }
    else if (CEditorDisplayObject* pDisplay = dynamic_cast<CEditorDisplayObject*>(pObj))
    {
        Render_DisplayObjectProperties(pDisplay);
    }
    else if (CEditorDisplayCubeObject* pDisplayCube = dynamic_cast<CEditorDisplayCubeObject*>(pObj))
    {
        Render_DisplayCubeObjectProperties(pDisplayCube);
    }
    // 단일 텍스처라 필요없을 듯?
    //else if (CEditorWindow* pWindow = dynamic_cast<CEditorWindow*>(pObj))
    //{
    //    Render_WindowProperties(pWindow);
    //}
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

void CInspector::Render_InteractObjectProperties(CEditorInteractObject* pInteract)
{
    if (!pInteract)
        return;

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("InteractObject Properties");

    ImGui::Spacing();

    // 아이템 타입 콤보박스
    OBJ_ITEM_TYPE eItemType = pInteract->Get_ItemType();
    const char* szItemTypes[] = {
        "Extinguisher",
        "Axe"
    };

    // OBJ_ITEM_TYPE enum에서 콤보 인덱스로 변환
    int iSelectedType = 0;
    if (eItemType == ITEM_EXTINGUISHER)
        iSelectedType = 0;
    else if (eItemType == ITEM_AXE)
        iSelectedType = 1;

    if (ImGui::Combo("Item Type", &iSelectedType, szItemTypes, IM_ARRAYSIZE(szItemTypes)))
    {
        OBJ_ITEM_TYPE eNewType = ITEM_EXTINGUISHER;
        if (iSelectedType == 0)
            eNewType = ITEM_EXTINGUISHER;
        else if (iSelectedType == 1)
            eNewType = ITEM_AXE;

        pInteract->Set_ItemType(eNewType);

        // 이름도 변경
        if (eNewType == ITEM_AXE)
            pInteract->Set_Name(L"Axe");
        else
            pInteract->Set_Name(L"Extinguisher");
    }

    ImGui::Spacing();

    // 프리셋 버튼
    ImGui::Text("Presets:");
    if (ImGui::Button("Extinguisher"))
    {
        pInteract->Set_ItemType(ITEM_EXTINGUISHER);
        pInteract->Set_Name(L"Extinguisher");
    }
    ImGui::SameLine();
    if (ImGui::Button("Axe"))
    {
        pInteract->Set_ItemType(ITEM_AXE);
        pInteract->Set_Name(L"Axe");
    }
}

void CInspector::Render_DisplayObjectProperties(CEditorDisplayObject* pDisplay)
{
    if (!pDisplay)
        return;

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("DisplayObject Properties");

    ImGui::Spacing();

    // DisplayObject 타입 콤보박스
    DISPLAY_OBJECT_TYPE eType = pDisplay->Get_DisplayObjectType();
    const char* szDisplayTypes[] = {
        "JUMP_BORDER",
        "CABLES",
        "PALMS",
        "PASSARELA",
        "RUG",
        "SIGNS",
        "PLATE",
        "ROAD_CORNER",
        "ROAD_PLATE",
        "TRAFFIC_LIGHTS",
        "TRAFFIC_SIGN_1",
        "TRAFFIC_SIGN_2",
        "TRAFFIC_SIGN_3",
        "WALL_WINDOW",
        "PASSTAIRS",
        "STREET_LIGHTS",
        "SCREEN_DISPLAY",
        "NAKAMURA_TEXT",
        "DOOR"
    };

    int iSelectedType = static_cast<int>(eType);
    if (ImGui::Combo("Display Type", &iSelectedType, szDisplayTypes, IM_ARRAYSIZE(szDisplayTypes)))
    {
        DISPLAY_OBJECT_TYPE eNewType = static_cast<DISPLAY_OBJECT_TYPE>(iSelectedType);
        pDisplay->Set_DisplayObjectType(eNewType, 0);  // textureIdx 초기화
    }

    ImGui::Spacing();

    // Texture Index 선택 (해당 타입에 여러 텍스처가 있는 경우)
    _uint iTextureId = pDisplay->Get_TextureIdx();

    // 타입별 최대 텍스처 인덱스 설정
    Engine::CTexture* pTextureCom = dynamic_cast<Engine::CTexture*>(
        pDisplay->Get_Component(ID_STATIC, L"Com_Texture"));

    _int iMaxIdx(0);

    if (pTextureCom)
    {
        Engine::TextureDesc* pDesc = pTextureCom->GetTextureDesc(iSelectedType);
        if (pDesc)
        {
            iMaxIdx = (_int)pDesc->vMaxIdx.x;
        }
    }

    if (iMaxIdx > 0)
    {
        _int iIdx = static_cast<_int>(iTextureId);
        if (ImGui::SliderInt("Texture Index", &iIdx, 0, iMaxIdx))
        {
            pDisplay->Set_TextureIdx(iIdx);
            pDisplay->Set_DisplayObjectType(eType, static_cast<_uint>(iIdx));
        }
    }

    if (WALL_WINDOW == pDisplay->Get_DisplayObjectType())
    {
        ImGui::Spacing();

        ImGui::Text("Presets:");
        if (ImGui::Button("SMALL"))
        {
            pDisplay->Set_Scale(_vec3(6.f, 6.f, 1.f));
        }
        ImGui::SameLine();
        if (ImGui::Button("MEDIUM"))
        {
            pDisplay->Set_Scale(_vec3(12.f, 12.f, 1.f));
        }
        ImGui::SameLine();
        if (ImGui::Button("LARGE"))
        {
            pDisplay->Set_Scale(_vec3(20.f, 20.f, 1.f));
        }
    }




}

void CInspector::Render_DisplayCubeObjectProperties(CEditorDisplayCubeObject* pDisplayCube)
{
    if (!pDisplayCube)
        return;

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("DisplayCubeObject Properties");

    ImGui::Spacing();

    // DisplayCubeObject 타입 콤보박스
    DISPLAY_CUBE_OBJECT_TYPE eType = pDisplayCube->Get_CubeObjectType();
    const char* szDisplayCubeTypes[] = {
        "BOX",
        "CONCRETE_BLOCK",
        "WALL_BLOCK",
        "BUILDING"
    };

    int iSelectedType = static_cast<int>(eType);
    if (ImGui::Combo("Cube Type", &iSelectedType, szDisplayCubeTypes, IM_ARRAYSIZE(szDisplayCubeTypes)))
    {
        DISPLAY_CUBE_OBJECT_TYPE eNewType = static_cast<DISPLAY_CUBE_OBJECT_TYPE>(iSelectedType);
        pDisplayCube->Set_CubeObjectType(eNewType);
    }

    ImGui::Spacing();

    // 프리셋 버튼
    ImGui::Text("Presets:");
    if (ImGui::Button("SMALL"))
    {
        pDisplayCube->Set_Scale(_vec3(16.f, 16.f, 16.f));
    }
    ImGui::SameLine();
    if (ImGui::Button("MEDIUM"))
    {
        pDisplayCube->Set_Scale(_vec3(32.f, 32.f, 32.f));
    }
    ImGui::SameLine();
    if (ImGui::Button("LARGE"))
    {
        pDisplayCube->Set_Scale(_vec3(64.f, 64.f, 64.f));
    }
}

void CInspector::Render_WindowProperties(CEditorWindow* pWindow)
{
    // 단일 텍스처라 구현 필요 X 
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