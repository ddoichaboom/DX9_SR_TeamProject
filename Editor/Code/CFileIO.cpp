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
#include "CEditorCube.h"
#include "CEditorFloor.h"
#include "CEditorCeiling.h"
#include "CEditorSpawnPoint.h"
#include "CEditorWall.h"

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
        jMap["version"] = FILE_VERSION;  // v2

        // 3. 오브젝트 배열
        json jObjects = json::array();

        auto& objectList = pScene->Get_ObjectList();

        for (auto& pObj : objectList)
        {
            json jObj;

            // 타입 판별 및 저장
            if (dynamic_cast<CEditorFloor*>(pObj))
            {
                jObj["type"] = "Floor";
            }
            else if (dynamic_cast<CEditorCeiling*>(pObj))
            {
                jObj["type"] = "Ceiling";
            }
            else if (dynamic_cast<CEditorCube*>(pObj))
            {
                jObj["type"] = "Cube";
            }
            else if (dynamic_cast<CEditorWall*>(pObj))
            {
                jObj["type"] = "Wall";

                // Wall 전용 필드: 방향
                CEditorWall* pWall = dynamic_cast<CEditorWall*>(pObj);
                jObj["wallDirection"] = static_cast<_int>(pWall->Get_WallDirection());
            }
            else if (dynamic_cast<CEditorSpawnPoint*>(pObj))
            {
                jObj["type"] = "SpawnPoint";

                // SpawnPoint 전용 필드
                CEditorSpawnPoint* pSpawn = dynamic_cast<CEditorSpawnPoint*>(pObj);

                // 스폰 타입 (문자열로 저장)
                if (pSpawn->Get_SpawnType() == SPAWN_PLAYER)
                    jObj["spawnType"] = "Player";
                else if (pSpawn->Get_SpawnType() == SPAWN_MONSTER)
                    jObj["spawnType"] = "Monster";

                // 몬스터 키 (옵션, 빈 문자열 가능)
                jObj["monsterKey"] = pSpawn->Get_MonsterKey();
            }
            else
            {
                continue;  // 알 수 없는 타입 - 건너뜀
            }

            // Transform 데이터
            _vec3 vPos = pObj->Get_Position();
            _vec3 vRot = pObj->Get_Rotation();
            _vec3 vScale = pObj->Get_Scale();

            jObj["position"] = { vPos.x, vPos.y, vPos.z };
            jObj["rotation"] = { vRot.x, vRot.y, vRot.z };
            jObj["scale"] = { vScale.x, vScale.y, vScale.z };

            // 이름
            jObj["name"] = WStringToString(pObj->Get_Name());

            // 배열에 추가
            jObjects.push_back(jObj);
        }

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

        // 4. 기존 오브젝트 전부 삭제
        pScene->Clear_AllObjects();

        // 5. 오브젝트 로드
        json jObjects = jMap["objects"];

        for (auto& jObj : jObjects)
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

            // 오브젝트 생성
            CEditorObject* pObj = nullptr;

            if (strType == "Tile")
            {
                // v1 하위 호환성: Tile → Floor로 변환
                pObj = CEditorFloor::Create(pGraphicDev, vPos, vRot, vScale);
            }
            else if (strType == "Floor")
            {
                pObj = CEditorFloor::Create(pGraphicDev, vPos, vRot, vScale);
            }
            else if (strType == "Ceiling")
            {
                pObj = CEditorCeiling::Create(pGraphicDev, vPos, vRot, vScale);
            }
            else if (strType == "Cube")
            {
                pObj = CEditorCube::Create(pGraphicDev, vPos, vRot, vScale);
            }
            else if (strType == "Wall")
            {
                // Wall 방향 읽기
                int iWallDir = jObj["wallDirection"];
                WALL_DIR eDir = static_cast<WALL_DIR>(iWallDir);

                pObj = CEditorWall::Create(pGraphicDev, vPos, vRot, vScale, eDir);
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

                // 몬스터 키 읽기 (옵션)
                string strMonsterKey = "";
                if (jObj.contains("monsterKey"))
                {
                    strMonsterKey = jObj["monsterKey"];
                }

                pObj = CEditorSpawnPoint::Create(pGraphicDev, vPos, vRot, vScale,
                    eSpawnType, strMonsterKey);
            }

            if (!pObj)
                continue;

            // 이름 읽기
            string strName = jObj["name"];
            pObj->Set_Name(StringToWString(strName));

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

