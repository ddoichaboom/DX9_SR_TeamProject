#include "pch.h"
#include "CMapLoader.h"
#include "CLayer.h"
#include "CTransform.h"
#include "CPoolMgr.h"

// 환경 오브젝트
#include "CFloor.h"
#include "CCeiling.h"
#include "CWall.h"
#include "CObstacle.h"

// 캐릭터,몬스터 (SpawnPoint 처리용)
#include "CPlayer.h"
#include "CWhiteMan.h"
#include "CTestCharacter.h"

#include <fstream>

using namespace Engine;

IMPLEMENT_SINGLETON(CMapLoader)

CMapLoader::CMapLoader()
    : m_vPlayerSpawnPos(0, 0, 0)
    , m_iFloorCount(0)
    , m_iCeilingCount(0)
    , m_iWallCount(0)
    , m_iObstacleCount(0)
{
    m_mapMonsterSpawnPos.clear();
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

HRESULT CMapLoader::Parse_MapData(const wstring& wstrPath)
{
    try
    {
        std::ifstream file(wstrPath);
        if (!file.is_open())
        {
            MSG_BOX("JSON File Open Failed");
            return E_FAIL;
        }

        json jMap;
        file >> jMap;
        file.close();

        _uint iVersion = jMap["version"];

        // 클래스별 카운트 직접 읽기
        if (iVersion >= 3 && jMap.contains("floorCount"))
        {
            m_iFloorCount = jMap["floorCount"];
            m_iCeilingCount = jMap["ceilingCount"];
            m_iWallCount = jMap["wallCount"];
            m_iObstacleCount = jMap["obstacleCount"];
        }
        else  // 배열 순회하여 카운팅
        {
            m_iFloorCount = 0;
            m_iCeilingCount = 0;
            m_iWallCount = 0;
            m_iObstacleCount = 0;

            json jObjects = jMap["objects"];
            for (auto& jObj : jObjects)
            {
                string strType = jObj["type"];
                if (strType == "Floor") 
                    m_iFloorCount++;
                else if (strType == "Ceiling") 
                    m_iCeilingCount++;
                else if (strType == "Wall") 
                    m_iWallCount++;
                else if (strType == "Cube") 
                    m_iObstacleCount++;
            }
        }

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

HRESULT CMapLoader::Load_MapData(const wstring& wstrPath,
    Engine::CLayer* pLayer,
    LPDIRECT3DDEVICE9 pGraphicDev)
{
    if (!pLayer || !pGraphicDev)
    {
        MSG_BOX("CMapLoader::Load_MapData - Invalid Parameters");
        return E_FAIL;
    }

    try
    {
        // 1. 파일 열기
        std::ifstream file(wstrPath);
        if (!file.is_open())
        {
            wchar_t wszError[512];
            swprintf_s(wszError, L"Failed to open map file:\n%s", wstrPath.c_str());
            MessageBox(nullptr, wszError, L"Load Error", MB_OK | MB_ICONERROR);
            return E_FAIL;
        }

        // 2. JSON 파싱
        json jMap;
        file >> jMap;
        file.close();

        // 3. 버전 확인 (버전 1과 2 모두 지원)
        _uint iVersion = jMap["version"];
        if (iVersion < 1 || iVersion > FILE_VERSION)
        {
            wchar_t wszError[256];
            swprintf_s(wszError, L"Unsupported file version: %d\nCurrent version: %d",
                iVersion, FILE_VERSION);
            MessageBox(nullptr, wszError, L"Load Error", MB_OK | MB_ICONERROR);
            return E_FAIL;
        }

        // 4. 오브젝트 로드
        json jObjects = jMap["objects"];
        _uint iLoadedCount = 0;

        for (auto& jObj : jObjects)
        {
            // GameObject 생성
            CGameObject* pGameObject = Create_GameObject_FromJSON(jObj, pGraphicDev);

            if (pGameObject)
            {
                if (FAILED(pLayer->Add_GameObject(pGameObject)))
                {
                    // Pool 객체는 Safe_Release 하지 말고 반납
                    pGameObject->ReturnToPool();
                }
                else
                {
                    iLoadedCount++;
                }
            }
        }

        // 5. 성공 메시지
        wchar_t wszMsg[256];
        swprintf_s(wszMsg, L"Map loaded successfully!\n%d objects loaded (v%d format).",
            iLoadedCount, iVersion);
        MessageBox(nullptr, wszMsg, L"Load Map", MB_OK);

        return S_OK;
    }
    catch (const json::exception& e)
    {
        // JSON 파싱 오류
        char szError[512];
        sprintf_s(szError, "JSON Parse Error: %s", e.what());
        MessageBoxA(nullptr, szError, "Load Error", MB_OK | MB_ICONERROR);
        return E_FAIL;
    }
    catch (...)
    {
        MSG_BOX("Unknown error during map load");
        return E_FAIL;
    }
}

CGameObject* CMapLoader::Create_GameObject_FromJSON(const json& jObj,
    LPDIRECT3DDEVICE9 pGraphicDev)
{
    try
    {
        // 타입 읽기
        string strType = jObj["type"];

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

        // 타입별 오브젝트 생성
        CGameObject* pGameObject = nullptr;

        if (strType == "Floor")
        {
            CFloor* pFloor = Engine::CPoolMgr::GetInstance()->Get_Object<CFloor>();
            if (pFloor)
            {
                pFloor->SetPos(vPos);
                pFloor->SetAngle(vRot);
                pFloor->SetScale(vScale);
                pFloor->Set_FloorType(100);
                pFloor->Get_Component(ID_STATIC, L"Com_Transform")->Update_Component(0.f);
                pGameObject = pFloor;
            }
        }
        else if (strType == "Ceiling")
        {
            CCeiling* pCeiling = Engine::CPoolMgr::GetInstance()->Get_Object<CCeiling>();
            if (pCeiling)
            {
                pCeiling->SetPos(vPos);
                pCeiling->SetAngle(vRot);
                pCeiling->SetScale(vScale);
                pCeiling->Get_Component(ID_STATIC, L"Com_Transform")->Update_Component(0.f);
                pGameObject = pCeiling;
            }
        }
        else if (strType == "Cube")
        {
            CObstacle* pObstacle = Engine::CPoolMgr::GetInstance()->Get_Object<CObstacle>();
            if (pObstacle)
            {
                pObstacle->SetPos(vPos);
                pObstacle->SetAngle(vRot);
                pObstacle->SetScale(vScale);
                pObstacle->Get_Component(ID_STATIC, L"Com_Transform")->Update_Component(0.f);
                pGameObject = pObstacle;
            }
        }
        else if (strType == "Wall")
        {
            CWall* pWall = Engine::CPoolMgr::GetInstance()->Get_Object<CWall>();
            if (pWall)
            {
                pWall->SetPos(vPos);
                pWall->SetAngle(vRot);
                pWall->SetScale(vScale);
                pWall->Get_Component(ID_STATIC, L"Com_Transform")->Update_Component(0.f);
                pGameObject = pWall;
            }
        }
        else if (strType == "SpawnPoint")
        {
            if (jObj.contains("spawnType"))
            {
                string strSpawnType = jObj["spawnType"];

                if (strSpawnType == "Player")
                {
                    m_vPlayerSpawnPos = vPos;
                }
                else if (strSpawnType == "Monster")
                {
                    string strMonsterKey = jObj["monsterKey"];

                    if (jObj.contains("monsterKey"))
                        strMonsterKey = jObj["monsterKey"];

                    // map에 추가
                    m_mapMonsterSpawnPos[strMonsterKey].push_back(vPos);
                }
            }

            // SpawnPoint는 GameObject를 생성하지 않음
            pGameObject = nullptr;
        }

        return pGameObject;
    }
    catch (...)
    {
        return nullptr;
    }
}

void CMapLoader::Free()
{
}