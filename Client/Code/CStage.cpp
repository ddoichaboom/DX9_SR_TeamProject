#include "pch.h"
#include "CStage.h"
#include "CBackGround.h"
#include "CProtoMgr.h"
#include "CMapLoader.h"
#include "CPoolMgr.h"
#include "CManagement.h"
#include "CDInputMgr.h"
#include "CLoading.h"

CStage::CStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev), m_pBackGround(nullptr), m_pEnvironment_Layer(nullptr)
    , m_pGameLogic_Layer(nullptr), m_pLoading(nullptr), m_iCurrentRoomIndex(-1)
    , m_BaseResult(E_FAIL), m_TextureResult(E_FAIL), m_ObjectPoolResult(E_FAIL)
    , m_ReadyEnvResult(E_FAIL), m_ReadyGameResult(E_FAIL)
{
}

CStage::~CStage()
{
}

_int CStage::Update_Scene(const _float& fTimeDelta)
{
	_int iExit = Engine::CScene::Update_Scene(fTimeDelta);
	return iExit;
}

void CStage::LateUpdate_Scene(const _float& fTimeDelta)
{
	Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

void CStage::Render_Scene()
{
}


void CStage::Change_Room(_int iNewRoomIndex)
{
    // 디버깅 용도
    char szLog[256];
    sprintf_s(szLog, "Room Change: %d → %d", m_iCurrentRoomIndex, iNewRoomIndex);
    OutputDebugStringA(szLog);

    // 언로드할 방 결정 
    // 새 방 기준 +-1 벗어난 방 언로드
    set<_int> roomsToUnload;
    for (_int iLoadedRoom : m_setLoadedRooms)
    {
        if (iLoadedRoom < iNewRoomIndex - 1 || iLoadedRoom > iNewRoomIndex + 1)
        {
            roomsToUnload.insert(iLoadedRoom);
        }
    }

    // 언로드 실행 (두 레이어 모두)
    for (_int iRoomToUnload : roomsToUnload)
    {
        // Environment_Layer 언로드
        CMapLoader::GetInstance()->Unload_Room(
            m_wstrCurrentMapFile,
            iRoomToUnload,
            m_pEnvironment_Layer);

        // GameLogic_Layer 언로드 (Monster)
        CMapLoader::GetInstance()->Unload_Room(
            m_wstrCurrentMapFile,
            iRoomToUnload,
            m_pGameLogic_Layer);

        m_setLoadedRooms.erase(iRoomToUnload);
    }

    // 로드할 방 결정 
    set<_int> roomsToLoad;
    for (_int i = iNewRoomIndex - 1; i <= iNewRoomIndex + 1; ++i)
    {
        if (i < 0)
            continue;  // 음수 방 번호 방지

        if (m_setLoadedRooms.find(i) == m_setLoadedRooms.end())
        {
            roomsToLoad.insert(i);
        }
    }

    // 로드 실행 (두 레이어 모두)
    for (_int iRoomToLoad : roomsToLoad)
    {
        bool bSuccess = true;

        // Environment_Layer 로드 (Floor, Ceiling, Wall, Cube)
        if (FAILED(CMapLoader::GetInstance()->Load_Room(
            m_wstrCurrentMapFile,
            iRoomToLoad,
            m_pEnvironment_Layer,
            m_pGraphicDev,
            L"Environment_Layer")))
        {
            bSuccess = false;
        }

        // GameLogic_Layer 로드 (Monster)
        if (FAILED(CMapLoader::GetInstance()->Load_Room(
            m_wstrCurrentMapFile,
            iRoomToLoad,
            m_pGameLogic_Layer,
            m_pGraphicDev,
            L"GameLogic_Layer")))
        {
            bSuccess = false;
        }

        if (bSuccess)
        {
            m_setLoadedRooms.insert(iRoomToLoad);
        }
    }

    //  5단계: 현재 방 업데이트 
    m_iCurrentRoomIndex = iNewRoomIndex;
}


void CStage::Free()
{
	CScene::Free();
}
