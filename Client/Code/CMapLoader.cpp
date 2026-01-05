#include "pch.h"
#include "CMapLoader.h"
#include "CLayer.h"
#include "CTransform.h"

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
                // Layer에 추가
                wstring wstrName = StringToWString(jObj["name"]);
                if (SUCCEEDED(pLayer->Add_GameObject(wstrName.c_str(), pGameObject)))
                {
                    iLoadedCount++;
                }
                else
                {
                    Safe_Release(pGameObject);
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
            //  CFloor::Create에서 Set_Angle, Set_Scale 호출
            pGameObject = CFloor::Create(pGraphicDev, vPos, vRot, vScale);
        }
        else if (strType == "Ceiling")
        {
            pGameObject = CCeiling::Create(pGraphicDev, vPos, vRot, vScale);
        }
        else if (strType == "Cube")
        {
            pGameObject = CObstacle::Create(pGraphicDev, vPos, vRot, vScale);
        }
        else if (strType == "Wall")
        {
            pGameObject = CWall::Create(pGraphicDev, vPos, vRot, vScale);
        }
        else if (strType == "SpawnPoint")
        {
            // SpawnPoint 처리 (Editor Phase 8에서 추가됨)
            if (jObj.contains("spawnType"))
            {
                string strSpawnType = jObj["spawnType"];

                if (strSpawnType == "Player")
                {
                    m_vPlayerSpawnPos = vPos;
                }
                else if (strSpawnType == "Monster")
                {
                    string strMonsterKey = "WhiteMan"; // 기본 값
                    if (jObj.contains("monsterKey"))
                        strMonsterKey = jObj["monsterKey"];

                    // map에 추가
                    m_mapMonsterSpawnPos[strMonsterKey].push_back(vPos);
                }
            }

            // SpawnPoint는 GameObject를 생성하지 않음
            pGameObject = nullptr;
        }

        // Phase 7 이후: 텍스처 설정
        // if (jObj.contains("texture"))
        // {
        //     string strTexKey = jObj["texture"];
        //     // CTexture* pTexture = ...;
        //     // pGameObject->Set_Texture(pTexture);
        // }

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