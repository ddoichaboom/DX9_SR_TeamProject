#include "pch.h"
#include "CMapLoader.h"
#include "CLayer.h"
#include "CTransform.h"
#include "CPoolMgr.h"
#include "CManagement.h"
#include "CCollision.h"

// 환경 오브젝트
#include "CFloor.h"
#include "CDynamicFloor.h"
#include "CCeiling.h"
#include "CDynamicCeiling.h"
#include "CWall.h"
#include "CDynamicWall.h"
#include "CVendingMachine.h"
#include "CSlopeFloor.h"
#include "CMapCollider.h"
#include "CRoomTrigger.h"
#include "CStageEndTrigger.h"
#include "CDoor.h"
#include "CExtinguisher.h"
#include "CDisplayObject.h"
#include "CWindow.h"
#include "CDisplayCubeObject.h"

// 캐릭터,몬스터 (SpawnPoint 처리용)
#include "CPlayer.h"
#include "CWhiteMan.h"
#include "CBeamMon.h"
#include "CFlyMon.h"
#include "CTestCharacter.h"
#include "CBoss.h"
#include "CAxe.h"

#include <fstream>

using namespace Engine;

IMPLEMENT_SINGLETON(CMapLoader)

vector<wstring> CMapLoader::m_vecMapFiles =
{
    {L"../../Map/TutorialStage.json"},
    {L"../../Map/MainStage.json"},
    {L"../../Map/BossStage.json"},
    {L"../../Map/SniperStage.json"},
    {L"../../Map/RoadMap_Proto.json"},
    //{L"../../Map/SniperStage_2.json"}     // 유리창 없는 파일
};

CMapLoader::CMapLoader()
{
}     

CMapLoader::~CMapLoader()
{
    Free();
}

string CMapLoader::WStringToString(const wstring& wstr)
{
    if (wstr.empty())
        return string();

    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    string result(size - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size, nullptr, nullptr);
    return result;
}

wstring CMapLoader::StringToWString(const string& str)
{
    if (str.empty())
        return wstring();

    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    wstring result(size - 1, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size);
    return result;
}

HRESULT CMapLoader::Preload_AllMapData(const wstring& wstrPath)
{

    try
    {
        // ========== 1단계: JSON 파일 열기 ==========
        std::ifstream file(wstrPath);
        if (!file.is_open())
        {
            MSG_BOX("JSON File Open Failed");
            return E_FAIL;
        }

        json jMap;
        file >> jMap;
        file.close();

        // ========== 2단계: 버전 확인 ==========
        _uint iVersion = jMap["version"];
        if (iVersion < 4)
        {
            MessageBox(nullptr, L"JSON 파일에 roomIndex 필드가 없습니다.\n버전 4 이상 필요.",
                L"Error", MB_OK | MB_ICONERROR);
            return E_FAIL;
        }

        // ========== 3단계: 맵 파일명 추출 ==========
        string strFileName = WStringToString(wstrPath);
        // "../../Map/test.json" → "test.json"
        size_t lastSlash = strFileName.find_last_of("/\\");
        if (lastSlash != string::npos)
            strFileName = strFileName.substr(lastSlash + 1);

        // ========== 4단계: 방별로 오브젝트 분류 ==========
        json jObjects = jMap["objects"];
        map<int, RoomData>& roomMap = m_mapAllRooms[strFileName];

        for (auto& jObj : jObjects)
        {
            // roomIndex 필드 읽기 (없으면 0번 방)
            _int iRoomIndex = 0;
            if (jObj.contains("roomIndex"))
                iRoomIndex = jObj["roomIndex"];

            // ObjectData 파싱
            ObjectData objData = Parse_ObjectData_FromJSON(jObj);

            // 해당 방의 RoomData에 추가
            if (roomMap.find(iRoomIndex) == roomMap.end())
            {
                RoomData newRoom;
                newRoom.iRoomIdx = iRoomIndex;
                roomMap[iRoomIndex] = newRoom;
            }

            roomMap[iRoomIndex].vObjects.push_back(objData);

            // 통계 업데이트
            if (objData.sType == "Floor")
                roomMap[iRoomIndex].iFloorCount++;
            else if (objData.sType == "DynamicFloor")
                roomMap[iRoomIndex].iDynamicFloorCount++;
            else if (objData.sType == "SlopeFloor")
                roomMap[iRoomIndex].iSlopeFloorCount++;
            else if (objData.sType == "Ceiling")
                roomMap[iRoomIndex].iCeilingCount++;
            else if (objData.sType == "DynamicCeiling")
                roomMap[iRoomIndex].iDynamicCeilingCount++;
            else if (objData.sType == "Wall")
                roomMap[iRoomIndex].iWallCount++;
            else if (objData.sType == "DynamicWall")
                roomMap[iRoomIndex].iDynamicWallCount++;
            else if (objData.sType == "VendingMachine")
                roomMap[iRoomIndex].iVendingMachineCount++;
            else if (objData.sType == "MapCollider")
                roomMap[iRoomIndex].iMapColliderCount++;
            else if (objData.sType == "RoomTriggerBox")
                roomMap[iRoomIndex].iRoomTriggerBoxCount++;
            else if (objData.sType == "Door")
                roomMap[iRoomIndex].iDoorCount++;
            else if (objData.sType == "Extinguisher")
                roomMap[iRoomIndex].iExtinguisherCount++;
            else if (objData.sType == "Axe")
                roomMap[iRoomIndex].iAxeCount++;
            else if (objData.sType == "DisplayObject")
                roomMap[iRoomIndex].iDisplayObjectCount++;
            else if (objData.sType == "DisplayCubeObject")
                roomMap[iRoomIndex].iDisplayCubeObjectCount++;
        }

        // ========== 5단계: 로그 출력 ==========
        char szLog[512];
        sprintf_s(szLog, "Preload Success: %s\nTotal Rooms: %d",
            strFileName.c_str(), (_int)roomMap.size());
        OutputDebugStringA(szLog);

        return S_OK;
    }
    catch (const json::exception& e)
    {
        char szError[512];
        sprintf_s(szError, "JSON Parse Error: %s", e.what());
        MessageBoxA(nullptr, szError, "Error", MB_OK);
        return E_FAIL;
    }
}

HRESULT CMapLoader::Load_Room(const wstring& wstrPath, _int iRoomIndex, CLayer* pLayer, LPDIRECT3DDEVICE9 pGraphicDev, const wstring& pLayerTag)
{
    if (!pLayer || !pGraphicDev)
    {
        MSG_BOX("CMapLoader::Load_Room - Invalid Parameters");
        return E_FAIL;
    }

    try
    {
        // ========== 1단계: 캐싱된 RoomData 가져오기 ==========
        string strFileName = WStringToString(wstrPath);
        size_t lastSlash = strFileName.find_last_of("/\\");
        if (lastSlash != string::npos)
            strFileName = strFileName.substr(lastSlash + 1);

        auto iterMap = m_mapAllRooms.find(strFileName);
        if (iterMap == m_mapAllRooms.end())
        {
            char szError[256];
            sprintf_s(szError, "Map file not preloaded: %s", strFileName.c_str());
            MessageBoxA(nullptr, szError, "Error", MB_OK);
            return E_FAIL;
        }

        auto iterRoom = iterMap->second.find(iRoomIndex);
        if (iterRoom == iterMap->second.end())
        {
            // 해당 방이 없음 (정상 종료 가능)
            char szLog[256];
            sprintf_s(szLog, "Room %d not found in %s (may be intended)",
                iRoomIndex, strFileName.c_str());
            OutputDebugStringA(szLog);
            return S_OK;
        }

        const RoomData& roomData = iterRoom->second;

        // ========== 2단계: 레이어별 오브젝트 로드 ==========
        _uint iLoadedCount = 0;
        bool bIsEnvironmentLayer = (pLayerTag == L"Environment_Layer");
        bool bIsGameLogicLayer = (pLayerTag == L"GameLogic_Layer");

        for (const auto& objData : roomData.vObjects)
        {
            // ========== Environment_Layer 처리 ==========
            if (bIsEnvironmentLayer)
            {
                if (objData.sType == "SpawnPoint" || objData.sType == "Axe")
                    continue;

                if (objData.sType == "Floor" || objData.sType == "DynamicFloor" ||
                    objData.sType == "SlopeFloor" || objData.sType == "Ceiling" ||
                    objData.sType == "DynamicCeiling" || objData.sType == "Wall" || 
                    objData.sType == "DynamicWall" || objData.sType == "VendingMachine" || 
                    objData.sType == "MapCollider" || objData.sType == "RoomTriggerBox" ||
                    objData.sType == "Door" || objData.sType == "Extinguisher" ||
                    objData.sType == "DisplayObject" || objData.sType == "Window"||
                    objData.sType == "DisplayCubeObject")
                {
                    // GameObject 획득 (풀에서)
                    CGameObject* pGameObject = Get_GameObject_FromPool(objData, pGraphicDev);

                    if (nullptr == pGameObject)
                        return E_FAIL;

                    // RoomIndex 태그 설정
                    pGameObject->Set_RoomIndex(iRoomIndex);

                    // Layer에 추가
                    if (FAILED(pLayer->Add_GameObject(pGameObject)))
                    {
                        pGameObject->ReturnToPool();
                    }
                    else
                    {
                        iLoadedCount++;
                    }

                }
                else if (objData.sType == "StageEndTriggerBox")
                {
                    CGameObject* pGameObject = CStageEndTrigger::Create(pGraphicDev, objData.vPos, objData.vScale);

                    if (nullptr == pGameObject)
                        return E_FAIL;

                    pGameObject->Set_RoomIndex(iRoomIndex);

                    if (FAILED(pLayer->Add_GameObject(pGameObject)))
                        return E_FAIL;

                }
            }
            // ========== GameLogic_Layer 처리 ==========
            else if (bIsGameLogicLayer)
            {
                // SpawnPoint만 처리
                if (objData.sType == "SpawnPoint")
                {
                    // Player SpawnPoint는 위치만 저장 (한 번만 생성됨)
                    if (objData.sSpawnType == "Player")
                    {
                        CGameObject* pGameObject = pLayer->Get_Object(OBJ_PLAYER);

                        if (nullptr == pGameObject)
                        {
                            CPlayer* pPlayer = nullptr;

                            pGameObject = pPlayer = CPlayer::Create(pGraphicDev, objData.vPos);

                            if (nullptr == pGameObject)
                                return E_FAIL;

                            if (FAILED(pLayer->Add_GameObject(pGameObject)))
                                return E_FAIL;
                        }
                        else
                        {
                            pGameObject->SetPos(objData.vPos);
                        }
                    }
                    else if (objData.sSpawnType == "BossMonster")
                    {
                        CGameObject* pBoss = CBoss::Create(pGraphicDev, objData.vPos);

                        if (nullptr == pBoss)
                            return E_FAIL;

                        if (FAILED(pLayer->Add_GameObject(pBoss)))
                            return E_FAIL;
                    }
                    // Monster SpawnPoint는 바로 Monster 생성
                    else if (objData.sSpawnType == "Monster")
                    {
                        // Monster 풀에서 획득
                        CGameObject* pGameObject = Get_GameObject_FromPool(objData, pGraphicDev);

                        if (pGameObject)
                        {
                            pGameObject->Set_RoomIndex(iRoomIndex);

                            // Layer에 추가
                            if (FAILED(pLayer->Add_GameObject(pGameObject)))
                            {
                                pGameObject->ReturnToPool();
                            }
                            else
                            {
                                iLoadedCount++;
                            }
                        }
                    }
                }
                else if (objData.sType == "Axe")
                {
                    CGameObject* pGameObject = Get_GameObject_FromPool(objData, pGraphicDev);

                    if (pGameObject)
                    {
                        pGameObject->Set_RoomIndex(iRoomIndex);

                        // Layer에 추가
                        if (FAILED(pLayer->Add_GameObject(pGameObject)))
                        {
                            pGameObject->ReturnToPool();
                        }
                        else
                        {
                            iLoadedCount++;
                        }
                    }
                }
            }
        }

        // ========== 3단계: 로그 출력 ==========
        char szLog[256];
        string strLayerTag = WStringToString(pLayerTag);
        sprintf_s(szLog, "Room %d [%s] loaded: %d objects",
            iRoomIndex, strLayerTag.c_str(), iLoadedCount);
        OutputDebugStringA(szLog);

        return S_OK;
    }
    catch (const exception& e)
    {
        char szError[512];
        sprintf_s(szError, "Load_Room Error: %s", e.what());
        MessageBoxA(nullptr, szError, "Error", MB_OK);
        return E_FAIL;
    }
}

HRESULT CMapLoader::Unload_Room(const wstring& wstrPath, _int iRoomIndex, CLayer* pLayer)
{
    if (!pLayer)
    {
        MSG_BOX("CMapLoader::Unload_Room - Invalid Layer");
        return E_FAIL;
    }

    _uint iUnloadedCount = 0;

    OBJ_ID objIDs[] = {
        OBJ_FLOOR, OBJ_CEILING, OBJ_WALL, OBJ_VENDINGMACHINE, OBJ_COL, OBJ_TRIGGER, OBJ_DOOR, OBJ_DISPLAY,  // Environment
        OBJ_MONSTER, OBJ_ITEM                                                            // GameLogic
    };

    for (OBJ_ID objID : objIDs)
    {
        // 해당 OBJ_ID의 오브젝트 범위 가져오기
        auto range = pLayer->Get_Objects(objID);

        // 범위 내 모든 오브젝트 순회
        for (auto iter = range.first; iter != range.second; )
        {
            CGameObject* pObj = iter->second;

            // ========== 조건 확인 ==========
            // 1. 방 번호 일치?
            if (pObj->Get_RoomIndex() != iRoomIndex)
            {
                ++iter;
                continue;
            }

            // 2. 플레이어/카메라 보호 (이중 안전장치)
            if (pObj->GetOBJID() == OBJ_PLAYER || pObj->GetOBJID() == OBJ_CAM)
            {
                ++iter;
                continue;
            }

            // ========== 언로드 처리 ==========
            pObj->SetDead();       // Layer에서 제거 예약
            //pObj->ReturnToPool();  // Dead 처리하면 Layer에서 알아서 Pool에 반환 해줌
            iUnloadedCount++;

            ++iter;
        }
    }

    // TODO 정상 작동 확인 후 지우기
    char szLog[256];
    sprintf_s(szLog, "Room %d unloaded: %d objects", iRoomIndex, iUnloadedCount);
    OutputDebugStringA(szLog);

    return S_OK;
}

_uint CMapLoader::Get_MaxObjectCount(const wstring& wstrPath, const string& objectType)
{


    // ========== 맵 파일명 추출 ==========
    string strFileName = WStringToString(wstrPath);
    size_t lastSlash = strFileName.find_last_of("/\\");
    if (lastSlash != string::npos)
        strFileName = strFileName.substr(lastSlash + 1);

    auto iterMap = m_mapAllRooms.find(strFileName);
    if (iterMap == m_mapAllRooms.end())
        return 0;

    // ========== 3개 방 연속 구간에서 최대값 계산 ==========
    // 예: [Room0, Room1, Room2], [Room1, Room2, Room3], ...
    _uint iMaxCount = 0;
    map<int, RoomData>& roomMap = iterMap->second;

    for (auto iter = roomMap.begin(); iter != roomMap.end(); ++iter)
    {
        _int iStartRoom = iter->first;
        _uint iSum = 0;

        // 현재 방 + 다음 2개 방 (총 3개)
        for (_int i = 0; i < 3; ++i)
        {
            _int iTargetRoom = iStartRoom + i;
            auto iterTarget = roomMap.find(iTargetRoom);
            if (iterTarget == roomMap.end())
                break;

            RoomData& roomData = iterTarget->second;

            if (objectType == "Floor")
                iSum += roomData.iFloorCount;
            else if (objectType == "DynamicFloor")
                iSum += roomData.iDynamicFloorCount;
            else if (objectType == "SlopeFloor")
                iSum += roomData.iSlopeFloorCount;
            else if (objectType == "Ceiling")
                iSum += roomData.iCeilingCount;
            else if (objectType == "DynamicCeiling")
                iSum += roomData.iDynamicCeilingCount;
            else if (objectType == "Wall")
                iSum += roomData.iWallCount;
            else if (objectType == "DynamicWall")
                iSum += roomData.iDynamicWallCount;
            else if (objectType == "VendingMachine")
                iSum += roomData.iVendingMachineCount;
            else if (objectType == "MapCollider")
                iSum += roomData.iMapColliderCount;
            else if (objectType == "RoomTriggerBox")
                iSum += roomData.iRoomTriggerBoxCount;
            else if (objectType == "Door")
                iSum += roomData.iDoorCount;
            else if (objectType == "Extinguisher")
                iSum += roomData.iExtinguisherCount;
            else if (objectType == "Axe")
                iSum += roomData.iAxeCount;
            else if (objectType == "DisplayObject")
                iSum += roomData.iDisplayObjectCount;
            else if (objectType == "DisplayCubeObject")
                iSum += roomData.iDisplayCubeObjectCount;
        }

        if (iSum > iMaxCount)
            iMaxCount = iSum;
    }

    return iMaxCount;
}

HRESULT CMapLoader::Get_MonsterSpawnPoses(const wstring& wstrPath, _int iRoomIndex, CLayer* pLayer, LPDIRECT3DDEVICE9 pGraphicDev, const wstring& pLayerTag, queue<_vec3>& _qSpawnPoses)
{
    if (!pLayer || !pGraphicDev)
    {
        MSG_BOX("CMapLoader::Load_Room - Invalid Parameters");
        return E_FAIL;
    }

    try
    {
        // ========== 1단계: 캐싱된 RoomData 가져오기 ==========
        string strFileName = WStringToString(wstrPath);
        size_t lastSlash = strFileName.find_last_of("/\\");
        if (lastSlash != string::npos)
            strFileName = strFileName.substr(lastSlash + 1);

        auto iterMap = m_mapAllRooms.find(strFileName);
        if (iterMap == m_mapAllRooms.end())
        {
            char szError[256];
            sprintf_s(szError, "Map file not preloaded: %s", strFileName.c_str());
            MessageBoxA(nullptr, szError, "Error", MB_OK);
            return E_FAIL;
        }

        auto iterRoom = iterMap->second.find(iRoomIndex);
        if (iterRoom == iterMap->second.end())
        {
            // 해당 방이 없음 (정상 종료 가능)
            char szLog[256];
            sprintf_s(szLog, "Room %d not found in %s (may be intended)",
                iRoomIndex, strFileName.c_str());
            OutputDebugStringA(szLog);
            return S_OK;
        }

        const RoomData& roomData = iterRoom->second;

        // ========== 2단계: 레이어별 오브젝트 로드 ==========
        _uint iLoadedCount = 0;
        bool bIsGameLogicLayer = (pLayerTag == L"GameLogic_Layer");
        if (!bIsGameLogicLayer)  return E_FAIL;

        for (const auto& objData : roomData.vObjects)
        {
            // SpawnPoint만 처리
            if (objData.sType == "SpawnPoint")
            {
                if (objData.sSpawnType == "Monster" && objData.sMonsterKey == "WhiteMan")
                {
                    _qSpawnPoses.push(objData.vPos);
                }
            }
        }
       
        return S_OK;
    }
    catch (const exception& e)
    {
        char szError[512];
        sprintf_s(szError, "Load_Room Error: %s", e.what());
        MessageBoxA(nullptr, szError, "Error", MB_OK);
        return E_FAIL;
    }
    return S_OK;
}



ObjectData CMapLoader::Parse_ObjectData_FromJSON(const json& jObj)
{
    ObjectData objData;

    if (jObj.contains("triggerType"))
    {
        objData.iTriggerType = jObj["triggerType"];

        if (objData.iTriggerType == TRIGGER_ROOM_CHANGE)
        {
            objData.sType = "RoomTriggerBox";
            if (jObj.contains("name"))
                objData.sName = "RoomTriggerBox";
        }
        else if (objData.iTriggerType == TRIGGER_STAGE_END)
        {
            objData.sType = "StageEndTriggerBox";
            if (jObj.contains("name"))
                objData.sName = "StageEndTriggerBox";
        }
    }
    else
    {
        // 기본 정보
        objData.sType = jObj["type"];
        if (jObj.contains("name"))
            objData.sName = jObj["name"];
    }


    // Transform
    objData.vPos.x = jObj["position"][0];
    objData.vPos.y = jObj["position"][1];
    objData.vPos.z = jObj["position"][2];

    objData.vRot.x = jObj["rotation"][0];
    objData.vRot.y = jObj["rotation"][1];
    objData.vRot.z = jObj["rotation"][2];

    objData.vScale.x = jObj["scale"][0];
    objData.vScale.y = jObj["scale"][1];
    objData.vScale.z = jObj["scale"][2];

    // 오브젝트별 속성 (선택적)
    if (jObj.contains("textureIdx"))
        objData.iTextureIdx = jObj["textureIdx"];
    if (jObj.contains("floorType"))
        objData.iFloorType = jObj["floorType"];
    if (jObj.contains("ceilingType"))
        objData.iCeilingType = jObj["ceilingType"];
    if (jObj.contains("wallType"))
        objData.iWallType = jObj["wallType"];
    if (jObj.contains("doorType"))
        objData.iDoorType = jObj["doorType"];
    if (jObj.contains("doorID"))
        objData.iDoorID = jObj["doorID"];
    if (jObj.contains("displayObjectType"))
        objData.eObjectType = static_cast<DISPLAY_OBJECT_TYPE>(jObj["displayObjectType"]);
    if (jObj.contains("displayCubeObjectType"))
        objData.eCubeObjectType = static_cast<DISPLAY_CUBE_OBJECT_TYPE>(jObj["displayCubeObjectType"]);

    if (jObj.contains("SlopeDirection"))
        objData.eSlopeDir = jObj["SlopeDirection"];

    // SpawnPoint 전용
    if (jObj.contains("spawnType"))
        objData.sSpawnType = jObj["spawnType"];
    if (jObj.contains("monsterKey"))
        objData.sMonsterKey = jObj["monsterKey"];
    if (jObj.contains("colliderTag"))
        objData.eColliderTag = jObj["colliderTag"];


    return objData;
}

CGameObject* CMapLoader::Get_GameObject_FromPool(const ObjectData& objData, LPDIRECT3DDEVICE9 pGraphicDev)
{
    CGameObject* pGameObject = nullptr;

    if (objData.sType == "Floor")
    {
        CFloor* pFloor = Engine::CPoolMgr::GetInstance()->Get_Object<CFloor>();
        if (pFloor)
        {
            pFloor->SetPos(objData.vPos);
            pFloor->SetAngle(objData.vRot);
            pFloor->SetScale(objData.vScale);
            pFloor->Set_TextureIdx(objData.iTextureIdx);
            pFloor->Set_FloorType(objData.iFloorType);
            pFloor->Get_Component(ID_STATIC, L"Com_Transform")->Update_Component(0.f);
            pGameObject = pFloor;
        }
    }
    else if (objData.sType == "DynamicFloor")
    {
        CDynamicFloor* pDynamicFloor = Engine::CPoolMgr::GetInstance()->Get_Object<CDynamicFloor>();
        if (pDynamicFloor)
        {
            pDynamicFloor->SetPos(objData.vPos);
            pDynamicFloor->SetAngle(objData.vRot);
            pDynamicFloor->SetScale(objData.vScale);
            pDynamicFloor->Set_FloorType(objData.iFloorType);
            pDynamicFloor->Get_Component(ID_STATIC, L"Com_Transform")->Update_Component(0.f);
            pGameObject = pDynamicFloor;
        }
    }
    else if (objData.sType == "SlopeFloor")
    {
        CSlopeFloor* pSlopeFloor = Engine::CPoolMgr::GetInstance()->Get_Object<CSlopeFloor>();
        if (pSlopeFloor)
        {
            pSlopeFloor->SetPos(objData.vPos);
            pSlopeFloor->SetAngle(objData.vRot);
            pSlopeFloor->SetScale(objData.vScale);
            pSlopeFloor->Set_TextureIdx(objData.iTextureIdx);
            pSlopeFloor->Set_FloorType(objData.iFloorType);
            pSlopeFloor->Set_SlopeDirection(objData.eSlopeDir);
            pSlopeFloor->Get_Component(ID_STATIC, L"Com_Transform")->Update_Component(0.f);
            pGameObject = pSlopeFloor;
        }
    }
    else if (objData.sType == "Ceiling")
    {
        CCeiling* pCeiling = Engine::CPoolMgr::GetInstance()->Get_Object<CCeiling>();
        if (pCeiling)
        {
            pCeiling->SetPos(objData.vPos);
            pCeiling->SetAngle(objData.vRot);
            pCeiling->SetScale(objData.vScale);
            pCeiling->Set_TextureIdx(objData.iTextureIdx);
            pCeiling->Set_CeilingType(objData.iCeilingType);
            pCeiling->Get_Component(ID_STATIC, L"Com_Transform")->Update_Component(0.f);
            pGameObject = pCeiling;
        }
    }
    else if (objData.sType == "DynamicCeiling")
    {
        CDynamicCeiling* pDynamicCeiling = Engine::CPoolMgr::GetInstance()->Get_Object<CDynamicCeiling>();
        if (pDynamicCeiling)
        {
            pDynamicCeiling->SetPos(objData.vPos);
            pDynamicCeiling->SetAngle(objData.vRot);
            pDynamicCeiling->SetScale(objData.vScale);
            pDynamicCeiling->Set_TextureIdx(objData.iTextureIdx);       // 동적 -> 정적 텍스처 변환 했을 때 사용할 텍스처 인덱스 
            pDynamicCeiling->Set_CeilingType(objData.iCeilingType);
            pDynamicCeiling->Get_Component(ID_STATIC, L"Com_Transform")->Update_Component(0.f);
            pGameObject = pDynamicCeiling;
        }
    }
    else if (objData.sType == "Wall")
    {
        CWall* pWall = Engine::CPoolMgr::GetInstance()->Get_Object<CWall>();
        if (pWall)
        {
            pWall->SetPos(objData.vPos);
            pWall->SetAngle(objData.vRot);
            pWall->SetScale(objData.vScale);
            pWall->Set_TextureIdx(objData.iTextureIdx);
            pWall->Set_WallType(objData.iWallType);
            pWall->Get_Component(ID_STATIC, L"Com_Transform")->Update_Component(0.f);
            pGameObject = pWall;
        }
    }
    else if (objData.sType == "DynamicWall")
    {
        CDynamicWall* pDynamicWall = Engine::CPoolMgr::GetInstance()->Get_Object<CDynamicWall>();
        if (pDynamicWall)
        {
            pDynamicWall->SetPos(objData.vPos);
            pDynamicWall->SetAngle(objData.vRot);
            pDynamicWall->SetScale(objData.vScale);
            pDynamicWall->Set_TextureIdx(objData.iTextureIdx);
            pDynamicWall->Set_WallType(objData.iWallType);
            pDynamicWall->Get_Component(ID_STATIC, L"Com_Transform")->Update_Component(0.f);
            pGameObject = pDynamicWall;
        }
    }
    else if (objData.sType == "VendingMachine")
    {
        CVendingMachine* pVendingMachine = Engine::CPoolMgr::GetInstance()->Get_Object<CVendingMachine>();
        if (pVendingMachine)
        {
            pVendingMachine->SetPos(objData.vPos);
            pVendingMachine->SetAngle(objData.vRot);
            pVendingMachine->SetScale(objData.vScale);
            pVendingMachine->Set_ColliderScale(objData.vScale);
            pVendingMachine->Get_Component(ID_STATIC, L"Com_Transform")->Update_Component(0.f);
            pGameObject = pVendingMachine;
        }
    }
    else if (objData.sType == "MapCollider")
    {
        // MapCollider 풀링 또는 직접 생성
        CMapCollider* pMapCollider = Engine::CPoolMgr::GetInstance()->Get_Object<CMapCollider>();
        if (pMapCollider)
        {
            pMapCollider->SetPos(objData.vPos);
            pMapCollider->Set_ColliderScale(objData.vScale);
            pMapCollider->Get_Component(ID_STATIC, L"Com_Transform")->Update_Component(0.f);
            pMapCollider->Set_ColliderTag(objData.eColliderTag);
            pMapCollider->Activate();
            pGameObject = pMapCollider;
        }

    }
    else if (objData.sType == "RoomTriggerBox")
    {
        // RoomTriggerBox 
            CRoomTrigger* pRoomTrigger = Engine::CPoolMgr::GetInstance()->Get_Object<CRoomTrigger>();
            if (pRoomTrigger)
            {
                pRoomTrigger->SetPos(objData.vPos);
                pRoomTrigger->Set_ColliderScale(objData.vScale);
                pRoomTrigger->Get_Component(ID_STATIC, L"Com_Transform")->Update_Component(0.f);
                pRoomTrigger->Activate();
                pGameObject = pRoomTrigger;
            }
    }
    else if (objData.sType == "Door")
    {
        CDoor* pDoor = Engine::CPoolMgr::GetInstance()->Get_Object<CDoor>();
        if (pDoor)
        {
            pDoor->Set_Position(objData.vPos);
            pDoor->Set_Scale(objData.vScale);
            pDoor->Set_Rotation(objData.vRot);
            pDoor->Set_DoorType(objData.iDoorType);
            pDoor->Set_DoorID(objData.iDoorID);

            // Transform 즉시 업데이트
            pDoor->Get_Component(ID_DYNAMIC, L"Com_Transform")->Update_Component(0.f);

            // Collider Transform도 즉시 업데이트
            Engine::CCollision* pCollision = static_cast<Engine::CCollision*>(
                pDoor->Get_Component(ID_DYNAMIC, L"Com_Collision"));
            if (pCollision)
                pCollision->LateUpdate_Component();

            pGameObject = pDoor;
        }
    }
    else if ((objData.sType == "SpawnPoint") && (objData.sSpawnType == "Monster"))
    {
        if (objData.sMonsterKey == "WhiteMan")
        {
            CWhiteMan* pWhiteMan = Engine::CPoolMgr::GetInstance()->Get_Object<CWhiteMan>();
            if (pWhiteMan)
            {
                pWhiteMan->SetPos(objData.vPos);
                pGameObject = pWhiteMan;
            }
        }
        else if (objData.sMonsterKey == "BeamMon")
        {
            CBeamMon* pBeamMon = Engine::CPoolMgr::GetInstance()->Get_Object<CBeamMon>();
            if (pBeamMon)
            {
                pBeamMon->SetPos(objData.vPos);
                pGameObject = pBeamMon;
            }
        }
        else if (objData.sMonsterKey == "FlyMon")
        {
            CFlyMon* pFlyMon = Engine::CPoolMgr::GetInstance()->Get_Object<CFlyMon>();
            if (pFlyMon)
            {
                pFlyMon->SetPos(objData.vPos);
                pGameObject = pFlyMon;
            }
        }
    }
    else if (objData.sType == "Extinguisher")
    {
        CExtinguisher* pExting = CPoolMgr::GetInstance()->Get_Object<CExtinguisher>();
        if (pExting)
        {
            pExting->SetPos(objData.vPos);
            pExting->SetTransformMatrix();
            pGameObject = pExting;
        }
    }
    else if (objData.sType == "Axe")
    {
        CAxe* pAxe = CPoolMgr::GetInstance()->Get_Object<CAxe>();
        if (pAxe)
        {
            pAxe->SetPos(objData.vPos);
            pAxe->SetTransformMatrix();
            pAxe->Activate();
            pGameObject = pAxe;
        }
    }
    else if (objData.sType == "DisplayObject")
    {
        CDisplayObject* pDisplay = CPoolMgr::GetInstance()->Get_Object<CDisplayObject>();
        if (pDisplay)
        {
            pDisplay->SetPos(objData.vPos);
            pDisplay->Set_Angle(objData.vRot);
            pDisplay->SetScale(objData.vScale);
            pDisplay->Set_DisplayObjectType(objData.eObjectType, objData.iTextureIdx);
            pDisplay->SetTransformMatrix();
            pGameObject = pDisplay;
        }
    }
    else if (objData.sType == "Window")
    {
        CWindow* pWindow = Engine::CPoolMgr::GetInstance()->Get_Object<CWindow>();
        if (pWindow)
        {
            pWindow->SetPos(objData.vPos);
            pWindow->SetAngle(objData.vRot);
            pWindow->SetScale(objData.vScale);
            pWindow->Get_Component(ID_STATIC, L"Com_Transform")->Update_Component(0.f);
            pGameObject = pWindow;
        }
    }
    else if (objData.sType == "DisplayCubeObject")
    {
        CDisplayCubeObject* pDisplayCube = Engine::CPoolMgr::GetInstance()->Get_Object<CDisplayCubeObject>();
        if (pDisplayCube)
        {
            pDisplayCube->SetPos(objData.vPos);
            pDisplayCube->Set_Angle(objData.vRot);
            pDisplayCube->SetScale(objData.vScale);
            pDisplayCube->Set_CubeObjectType(objData.eCubeObjectType);
            pDisplayCube->SetTransformMatrix();
            pGameObject = pDisplayCube;
        }
    }


    return pGameObject;
}

void CMapLoader::Free()
{
}