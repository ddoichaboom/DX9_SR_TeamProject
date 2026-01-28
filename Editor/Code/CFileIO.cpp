// Windows 헤더
#include <windows.h>

// DirectX 9
#include <d3d9.h>
#include <d3dx9.h>

// STL
#include <vector>
#include <list>
#include <string>
#include <fstream>
#include <codecvt>

// CFileIO 관련
#include "CFileIO.h"
#include "CEditorScene.h"
#include "CEditorObject.h"
#include "CEditorVendingMachine.h"
#include "CEditorFloor.h"
#include "CEditorDynamicFloor.h"
#include "CEditorSlopeFloor.h"
#include "CEditorCeiling.h"
#include "CEditorSpawnPoint.h"
#include "CEditorWall.h"
#include "CEditorDynamicWall.h"
#include "CEditorMapCollider.h"
#include "CEditorTriggerBox.h"
#include "CEditorDoor.h"
#include "CEditorInteractObject.h"

using namespace std;
using namespace Engine;

extern HINSTANCE g_hInst;
extern HWND g_hWnd;


IMPLEMENT_SINGLETON(CFileIO)

CFileIO::CFileIO()
{
}

CFileIO::~CFileIO()
{
    Free();
}

string CFileIO::WStringToString(const wstring& wstr)
{
    if (wstr.empty())
        return string();

    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    string result(size - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size, nullptr, nullptr);
    return result;
}

wstring CFileIO::StringToWString(const string& str)
{
    if (str.empty())
        return wstring();

    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    wstring result(size - 1, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size);
    return result;
}

void CFileIO::SaveTransformData(json& jObj, CEditorObject* pObj)
{
    _vec3 vPos = pObj->Get_Position();
    _vec3 vRot = pObj->Get_Rotation();
    _vec3 vScale = pObj->Get_Scale();

    jObj["position"] = { vPos.x, vPos.y, vPos.z };
    jObj["rotation"] = { vRot.x, vRot.y, vRot.z };
    jObj["scale"] = { vScale.x, vScale.y, vScale.z };
    jObj["name"] = WStringToString(pObj->Get_Name());
}

void CFileIO::SaveTextureData(json& jObj, CEditorObject* pObj)
{
    // Floor 객체
    if (CEditorFloor* pFloor = dynamic_cast<CEditorFloor*>(pObj))
    {
        // Dynamic Floor 확인
        if (CEditorDynamicFloor* pDynamicFloor = dynamic_cast<CEditorDynamicFloor*>(pObj))
        {
            jObj["isDynamic"] = true;
            jObj["floorType"] = pDynamicFloor->Get_FloorType();
        }
        else
        {
            jObj["isDynamic"] = false;
            jObj["floorType"] = pFloor->Get_FloorType();
            jObj["textureIdx"] = pFloor->Get_TextureIdx();
        }
    }
    // Ceiling 객체
    else if (CEditorCeiling* pCeiling = dynamic_cast<CEditorCeiling*>(pObj))
    {
        jObj["ceilingType"] = pCeiling->Get_CeilingType();
        jObj["textureIdx"] = pCeiling->Get_TextureIdx();
    }
    // Wall 객체
    else if (CEditorWall* pWall = dynamic_cast<CEditorWall*>(pObj))
    {
        jObj["wallType"] = pWall->Get_WallType();
        jObj["textureIdx"] = pWall->Get_TextureIdx();
    }
}

HRESULT CFileIO::Save_MapData(const wstring& wstrPath, CEditorScene* pScene)
{
    if (!pScene)
    {
        MSG_BOX("CFileIO::Save_MapData - Invalid Scene");
        return E_FAIL;
    }

    try
    {
        // 1. JSON 객체 생성
        json jMap;

        // 2. 버전 정보
        jMap["version"] = FILE_VERSION;  

        // 클래스별 카운트 집계
        _uint iFloorCount = 0;
        _uint iDynamicFloorCount = 0;
        _uint iSlopeFloorCount = 0;
        _uint iCeilingCount = 0;
        _uint iWallCount = 0;
        _uint iDynamicWallCount = 0;
        _uint iVendingMachineCount = 0;
        _uint iMapColliderCount = 0;
        _uint iTriggerBoxCount = 0;
        _uint iDoorCount = 0;
        _uint iExtinguisherCount = 0;
        _uint iAxeCount = 0;


        auto& objectList = pScene->Get_ObjectList();

        // 객체 데이터 저장 
        json jObjects = json::array();

        // 객체 데이터 저장 

        for (auto& pObj : objectList)
        {
            json jObj;

            // 타입 판별 및 저장
            if (CEditorFloor* pFloor = dynamic_cast<CEditorFloor*>(pObj))
            {
                if (dynamic_cast<CEditorDynamicFloor*>(pObj))
                {
                    iDynamicFloorCount++;
                    jObj["type"] = "DynamicFloor";  
                }
                else if (CEditorSlopeFloor* pSlopeFloor = dynamic_cast<CEditorSlopeFloor*>(pObj))
                {
                    iSlopeFloorCount++;
                    jObj["type"] = "SlopeFloor";

                    jObj["SlopeDirection"] = pSlopeFloor->Get_SlopeDirection();
                }
                else
                {
                    iFloorCount++;
                    jObj["type"] = "Floor";
                }

                jObj["roomIndex"] = pObj->Get_RoomIndex();
                SaveTransformData(jObj, pObj);
                SaveTextureData(jObj, pObj);
            }
            else if (dynamic_cast<CEditorCeiling*>(pObj))
            {
                iCeilingCount++;
                jObj["type"] = "Ceiling";

                jObj["roomIndex"] = pObj->Get_RoomIndex();
                SaveTransformData(jObj, pObj);
                SaveTextureData(jObj, pObj);
            }
            else if (CEditorWall* pWall = dynamic_cast<CEditorWall*>(pObj))
            {
                CEditorDynamicWall* pDynamicWall = dynamic_cast<CEditorDynamicWall*>(pObj);

                if (pDynamicWall)
                {
                    iDynamicWallCount++;
                    jObj["type"] = "DynamicWall";
                }
                else
                {
                    iWallCount++;
                    jObj["type"] = "Wall";
                }

                // Wall 전용 필드
                jObj["wallDirection"] = static_cast<_int>(pWall->Get_WallDirection());

                jObj["roomIndex"] = pObj->Get_RoomIndex();
                SaveTransformData(jObj, pObj);
                SaveTextureData(jObj, pObj);
            }
            else if (dynamic_cast<CEditorVendingMachine*>(pObj))
            {
                iVendingMachineCount++;
                jObj["type"] = "VendingMachine";

                jObj["roomIndex"] = pObj->Get_RoomIndex();
                SaveTransformData(jObj, pObj);
            }
            else if (CEditorSpawnPoint* pSpawn = dynamic_cast<CEditorSpawnPoint*>(pObj))
            {
                jObj["type"] = "SpawnPoint";

                // SpawnPoint 전용 필드
                if (pSpawn->Get_SpawnType() == SPAWN_PLAYER)
                    jObj["spawnType"] = "Player";
                else if (pSpawn->Get_SpawnType() == SPAWN_MONSTER)
                    jObj["spawnType"] = "Monster";
                else if (pSpawn->Get_SpawnType() == SPAWN_BOSSMONSTER)
                    jObj["spawnType"] = "BossMonster";

                jObj["monsterKey"] = pSpawn->Get_MonsterKey();

                jObj["roomIndex"] = pObj->Get_RoomIndex();
                // Transform 데이터 저장 (동일)
                SaveTransformData(jObj, pObj);
            }
            else if (CEditorMapCollider* pMapCollider = dynamic_cast<CEditorMapCollider*>(pObj))
            {
                iMapColliderCount++;
                jObj["type"] = "MapCollider";

                jObj["roomIndex"] = pObj->Get_RoomIndex();

                // Position (Transform에서)
                _vec3 vPos = pObj->Get_Position();
                jObj["position"] = { vPos.x, vPos.y, vPos.z };

                // Rotation (사용하지 않지만 호환성 위해 저장)
                jObj["rotation"] = { 0.f, 0.f, 0.f };

                // Scale (Collider Scale 저장)
                _vec3 vColliderScale = pMapCollider->Get_ColliderScale();
                jObj["scale"] = { vColliderScale.x, vColliderScale.y, vColliderScale.z };

                // 이름
                wstring wstrName = pObj->Get_Name();
                string strName(wstrName.begin(), wstrName.end());
                jObj["name"] = strName;

                jObj["colliderTag"] = pMapCollider->Get_ColliderTag();
            }
            else if (CEditorTriggerBox* pTriggerBox = dynamic_cast<CEditorTriggerBox*>(pObj))
            {
                iTriggerBoxCount++;
                jObj["type"] = "TriggerBox";

                jObj["roomIndex"] = pObj->Get_RoomIndex();

                // Position
                _vec3 vPos = pObj->Get_Position();
                jObj["position"] = { vPos.x, vPos.y, vPos.z };

                // Rotation
                jObj["rotation"] = { 0.f, 0.f, 0.f };

                // Scale (Collider Scale)
                _vec3 vColliderScale = pTriggerBox->Get_ColliderScale();
                jObj["scale"] = { vColliderScale.x, vColliderScale.y, vColliderScale.z };

                // 이름
                wstring wstrName = pObj->Get_Name();
                string strName(wstrName.begin(), wstrName.end());
                jObj["name"] = strName;

                // TriggerBox 전용 필드
                jObj["triggerType"] = static_cast<_int>(pTriggerBox->Get_TriggerType());
            }
            else if (CEditorDoor* pDoor = dynamic_cast<CEditorDoor*>(pObj))
            {
                iDoorCount++;
                jObj["type"] = "Door";
                jObj["roomIndex"] = pObj->Get_RoomIndex();

                // Transform 저장
                SaveTransformData(jObj, pObj);

                // Door 전용 필드 저장
                jObj["doorType"] = static_cast<_int>(pDoor->Get_DoorType());
                jObj["doorID"] = pDoor->Get_DoorID();
            }
            else if (CEditorInteractObject* pInteract = dynamic_cast<CEditorInteractObject*>(pObj))
            {
                OBJ_ITEM_TYPE eItemType = pInteract->Get_ItemType();

                // Client CMapLoader 호환: "Extinguisher", "Axe"로 저장
                if (eItemType == ITEM_AXE)
                {
                    iAxeCount++;
                    jObj["type"] = "Axe";
                }
                else
                {
                    iExtinguisherCount++;
                    jObj["type"] = "Extinguisher";
                }

                jObj["roomIndex"] = pObj->Get_RoomIndex();
                SaveTransformData(jObj, pObj);

                // itemType 필드 (Editor 로드 시 아이템 타입 복원용)
                jObj["itemType"] = static_cast<_int>(eItemType);
            }
            else
            {
                continue;  // 알 수 없는 타입 - 건너뜀
            }

            // 배열에 추가
            jObjects.push_back(jObj);
        }

        // 클래스별 카운트 저장
        jMap["floorCount"] = iFloorCount;
        jMap["dynamicFloorCount"] = iDynamicFloorCount;
        jMap["ceilingCount"] = iCeilingCount;
        jMap["wallCount"] = iWallCount;
        jMap["dynamicWallCount"] = iDynamicWallCount;
        jMap["VendingMachineCount"] = iVendingMachineCount;
        jMap["mapColliderCount"] = iMapColliderCount;      
        jMap["triggerBoxCount"] = iTriggerBoxCount;
        jMap["doorCount"] = iDoorCount;
        jMap["extinguisherCount"] = iExtinguisherCount;
        jMap["axeCount"] = iAxeCount;

        jMap["objects"] = jObjects;
        jMap["objectCount"] = jObjects.size();

        // 4. 파일로 저장 (들여쓰기 적용)
        std::ofstream file(wstrPath);
        if (!file.is_open())
        {
            MSG_BOX("Failed to create file");
            return E_FAIL;
        }

        file << jMap.dump(2);  // indent=2
        file.close();

        // 5. 성공 메시지
        wchar_t wszMsg[256];
        swprintf_s(wszMsg, L"Map saved successfully! (v%d)\n%d objects saved.",
            FILE_VERSION, (int)jObjects.size());
        MessageBox(nullptr, wszMsg, L"Save Map", MB_OK);

        return S_OK;
    }
    catch (const json::exception& e)
    {
        char szError[512];
        sprintf_s(szError, "JSON Error: %s", e.what());
        MessageBoxA(nullptr, szError, "Save Error", MB_OK | MB_ICONERROR);
        return E_FAIL;
    }
    catch (...)
    {
        MSG_BOX("Unknown error during save");
        return E_FAIL;
    }
}

HRESULT CFileIO::Load_MapData(const wstring& wstrPath,
                                CEditorScene* pScene,
                                LPDIRECT3DDEVICE9 pGraphicDev)
{
    if (!pScene || !pGraphicDev)
    {
        MSG_BOX("CFileIO::Load_MapData - Invalid Parameters");
        return E_FAIL;
    }

    try
    {
        // 1. 파일 열기
        std::ifstream file(wstrPath);
        if (!file.is_open())
        {
            MSG_BOX("Failed to open file");
            return E_FAIL;
        }

        // 2. JSON 파싱
        json jMap;
        file >> jMap;
        file.close();

        // 3. 버전 확인
        _uint iVersion = jMap["version"];

        // v1, v2 모두 지원 (하위 호환성)
        if (iVersion < 1 || iVersion > FILE_VERSION)
        {
            wchar_t wszError[256];
            swprintf_s(wszError, L"Unsupported file version: %d\nCurrent version: %d",
                iVersion, FILE_VERSION);
            MessageBox(nullptr, wszError, L"Load Error", MB_OK | MB_ICONERROR);
            return E_FAIL;
        }

        if (iVersion < 4)
        {
            MessageBox(nullptr, L"Warning: This map was saved without texture data (v3 or older).\nTextures will use default values.",
                L"Load Warning", MB_OK | MB_ICONWARNING);
        }

        // 4. 기존 오브젝트 전부 삭제
        pScene->Clear_AllObjects();

        // 5. 오브젝트 로드
        json jObjects = jMap["objects"];

        for (auto& jObj : jObjects)
        {
            // 타입 읽기
            string strType = jObj["type"];
            _int   iRoomIndex = jObj["roomIndex"];

            // Transform 읽기
            _vec3 vPos, vRot, vScale;

            vPos.x = jObj["position"][0];
            vPos.y = jObj["position"][1];
            vPos.z = jObj["position"][2];

            vRot.x = jObj["rotation"][0];
            vRot.y = jObj["rotation"][1];
            vRot.z = jObj["rotation"][2];

            vScale.x = jObj["scale"][0];
            vScale.y = jObj["scale"][1];
            vScale.z = jObj["scale"][2];


            // 오브젝트 생성
            CEditorObject* pObj = nullptr;

            if (strType == "DynamicFloor")
            {
                pObj = CEditorDynamicFloor::Create(pGraphicDev, vPos, vRot, vScale);

                // 텍스처 데이터 복원 (v4)
                if (iVersion >= 4 && jObj.contains("floorType"))
                {
                    CEditorDynamicFloor* pDynamicFloor = dynamic_cast<CEditorDynamicFloor*>(pObj);
                    if (pDynamicFloor)
                    {
                        _uint iFloorType = jObj["floorType"];
                        pDynamicFloor->Set_FloorType(iFloorType);
                    }
                }
            }
            else if (strType == "SlopeFloor")
            {
                pObj = CEditorSlopeFloor::Create(pGraphicDev, vPos, vRot, vScale);

                if (iVersion >= 4 && jObj.contains("floorType"))
                {
                    CEditorSlopeFloor* pSlopeFloor = dynamic_cast<CEditorSlopeFloor*>(pObj);
                    if (pSlopeFloor)
                    {
                        _uint iFloorType = jObj["floorType"];
                        pSlopeFloor->Set_FloorType(iFloorType);

                        if (jObj.contains("textureIdx"))
                        {
                            _int iTextureIdx = jObj["textureIdx"];
                            pSlopeFloor->Set_TextureIdx(iTextureIdx);
                        }
                        if (jObj.contains("SlopeDirection"))
                        {
                            SLOPE_DIR eSlopeDir = jObj["SlopeDirection"];
                            pSlopeFloor->Set_SlopeDirection(eSlopeDir);

                        }

                    }
                }

            }
            else if (strType == "Floor")
            {
                pObj = CEditorFloor::Create(pGraphicDev, vPos, vRot, vScale);

                if (iVersion >= 4 && jObj.contains("floorType"))
                {
                    CEditorFloor* pFloor = dynamic_cast<CEditorFloor*>(pObj);
                    if (pFloor)
                    {
                        _uint iFloorType = jObj["floorType"];
                        pFloor->Set_FloorType(iFloorType);

                        if (jObj.contains("textureIdx"))
                        {
                            _int iTextureIdx = jObj["textureIdx"];
                            pFloor->Set_TextureIdx(iTextureIdx);
                        }

                    }
                }
            }
            else if (strType == "Ceiling")
            {
                pObj = CEditorCeiling::Create(pGraphicDev, vPos, vRot, vScale);

                if (iVersion >= 4 && jObj.contains("ceilingType"))
                {
                    CEditorCeiling* pCeiling = dynamic_cast<CEditorCeiling*>(pObj);
                    if (pCeiling)
                    {
                        _uint iCeilingType = jObj["ceilingType"];
                        pCeiling->Set_CeilingType(iCeilingType);

                        if (jObj.contains("textureIdx"))
                        {
                            _int iTextureIdx = jObj["textureIdx"];
                            pCeiling->Set_TextureIdx(iTextureIdx);
                        }
                    }
                }
            }
            else if (strType == "Wall" || strType == "DynamicWall")
            {
                WALL_DIR eDir = WALL_XY_FRONT;
                if (jObj.contains("wallDirection"))
                {
                    int iWallDir = jObj["wallDirection"];
                    eDir = static_cast<WALL_DIR>(iWallDir);
                }

                if (strType == "DynamicWall")
                {
                    pObj = CEditorDynamicWall::Create(pGraphicDev, vPos, vRot, vScale, eDir);
                }
                else
                {
                    pObj = CEditorWall::Create(pGraphicDev, vPos, vRot, vScale, eDir);
                }

                if (pObj)
                {
                    CEditorWall* pWall = dynamic_cast<CEditorWall*>(pObj);
                    if (pWall)
                    {
                        _uint iWallType = jObj["wallType"];
                        pWall->Set_WallType(iWallType);

                        if (jObj.contains("textureIdx"))
                        {
                            _int iTextureIdx = jObj["textureIdx"];
                            pWall->Set_TextureIdx(iTextureIdx);
                        }
                    }
                }
            }
            else if (strType == "VendingMachine")
            {
                pObj = CEditorVendingMachine::Create(pGraphicDev, vPos, vRot, vScale);
            }
            else if (strType == "SpawnPoint")
            {
                // 스폰 타입 읽기
                string strSpawnType = jObj["spawnType"];
                SPAWN_TYPE eSpawnType = SPAWN_PLAYER;

                if (strSpawnType == "Player")
                    eSpawnType = SPAWN_PLAYER;
                else if (strSpawnType == "Monster")
                    eSpawnType = SPAWN_MONSTER;
                else if (strSpawnType == "BossMonster")
                    eSpawnType = SPAWN_BOSSMONSTER;

                // 몬스터 키 읽기 (옵션)
                string strMonsterKey = "";
                if (jObj.contains("monsterKey"))
                {
                    strMonsterKey = jObj["monsterKey"];
                }

                pObj = CEditorSpawnPoint::Create(pGraphicDev, vPos, vRot, vScale,
                    eSpawnType, strMonsterKey);
            }
            else if (strType == "MapCollider")
            {
                pObj = CEditorMapCollider::Create(pGraphicDev, vPos, vScale);
                if (pObj && jObj.contains("colliderTag"))
                {
                    CEditorMapCollider* pMapCollider = dynamic_cast<CEditorMapCollider*>(pObj);
                    if (pMapCollider)
                    {
                        COLLIDER_TAG  eTag = static_cast<COLLIDER_TAG>((_int)jObj["colliderTag"]);
                        pMapCollider->Set_ColliderTag(eTag);
                    }
                }
            }
            else if (strType == "TriggerBox")
            {
                // TriggerBox 전용 필드 읽기
                TRIGGER_TYPE eTriggerType = TRIGGER_ROOM_CHANGE;

                if (jObj.contains("triggerType"))
                    eTriggerType = static_cast<TRIGGER_TYPE>((_int)jObj["triggerType"]);


                pObj = CEditorTriggerBox::Create(pGraphicDev, vPos, vScale,
                    eTriggerType);
            }
            else if (strType == "Door")
            {
                pObj = CEditorDoor::Create(pGraphicDev, vPos, vRot, vScale);

                if (pObj)
                {
                    CEditorDoor* pDoor = dynamic_cast<CEditorDoor*>(pObj);
                    if (pDoor)
                    {
                        // Door Type 로드
                        if (jObj.contains("doorType"))
                        {
                            DOOR_TYPE eDoorType = jObj["doorType"];
                            pDoor->Set_DoorType(eDoorType);
                        }

                        // Door ID 로드
                        if (jObj.contains("doorID"))
                        {
                            _int iDoorID = jObj["doorID"];
                            pDoor->Set_DoorID(iDoorID);
                        }
                    }
                }
            }
            else if (strType == "Extinguisher" || strType == "Axe")
            {
                // itemType 필드가 있으면 사용, 없으면 타입 문자열로 결정
                OBJ_ITEM_TYPE eItemType = ITEM_EXTINGUISHER;

                if (jObj.contains("itemType"))
                {
                    eItemType = static_cast<OBJ_ITEM_TYPE>((_int)jObj["itemType"]);
                }
                else
                {
                    if (strType == "Axe")
                        eItemType = ITEM_AXE;
                    else
                        eItemType = ITEM_EXTINGUISHER;
                }

                pObj = CEditorInteractObject::Create(pGraphicDev, vPos, eItemType);
            }

            if (!pObj)
                continue;

            // 이름 읽기
            string strName = jObj["name"];
            pObj->Set_Name(StringToWString(strName));
            pObj->Set_RoomIndex(iRoomIndex);
            
            // Scene에 추가
            pScene->Add_Object(pObj);

            Safe_Release(pObj);
        }

        // 6. 성공 메시지
        wchar_t wszMsg[256];
        swprintf_s(wszMsg, L"Map loaded successfully! (v%d)\n%d objects loaded.",
            iVersion, (int)jObjects.size());
        MessageBox(nullptr, wszMsg, L"Load Map", MB_OK);

        return S_OK;
    }
    catch (const json::exception& e)
    {
        char szError[512];
        sprintf_s(szError, "JSON Parse Error: %s", e.what());
        MessageBoxA(nullptr, szError, "Load Error", MB_OK | MB_ICONERROR);
        return E_FAIL;
    }
    catch (...)
    {
        MSG_BOX("Unknown error during load");
        return E_FAIL;
    }
}

void CFileIO::Free()
{

}

