#include "pch.h"
#include "CMapStage.h"
#include "CProtoMgr.h"
#include "CMapLoader.h"
#include "CPoolMgr.h"
#include "CManagement.h"

// 환경 오브젝트 (필터링용, 실제 생성은 CMapLoader가 담당)
#include "CFloor.h"
#include "CDynamicFloor.h"
#include "CCeiling.h"
#include "CDynamicCeiling.h"
#include "CWall.h"
#include "CDynamicWall.h"
#include "CObstacle.h"

// 게임 로직 오브젝트
#include "CPlayer.h"
#include "CFirstCamera.h"
#include "CWhiteMan.h"
#include "CBeamMon.h"
#include "CFlyMon.h"
#include "CBullet.h"
#include "CBeam.h"

//Player
#include "CLeftPart.h"
#include "CRightPart.h"
#include "CMiddlePart.h"
#include "CPistol.h"
#include "CKatana.h"

#include "CLoading.h"
#include "CBackGround.h"
#include "CDebugObject.h"


//UI
#include "CPhoneBG.h"


CMapStage::CMapStage(LPDIRECT3DDEVICE9 pGraphicDev) : CStage(pGraphicDev), m_pLoading(nullptr)
{
}

CMapStage::~CMapStage()
{
}

HRESULT CMapStage::Ready_Scene()
{
    m_pBackGround = CBackGround::Create(m_pGraphicDev);


    //각 함수를 Loading에서 처리하도록 함 
    m_pLoading = CLoading::Create(m_pGraphicDev,
        [&]() {  m_BaseResult = Ready_Prototype(); },
        [&]() {  m_TextureResult = Ready_Prototype_OnlyTexture(); },
        [&]() {  m_ObjectPoolResult = Ready_ObjectPool(); },
        [&]() {  m_ReadyEnvResult = Ready_Environment_Layer(L"Environment_Layer"); },
        [&]() {  m_ReadyGameResult = Ready_GameLogic_Layer(L"GameLogic_Layer"); }
    );
    

  /*  if (FAILED(Ready_Prototype()))
        return E_FAIL;

    if (FAILED(Ready_ObjectPool()))
        return E_FAIL;

    if (FAILED(Ready_Environment_Layer(L"Environment_Layer")))
        return E_FAIL;

    if (FAILED(Ready_GameLogic_Layer(L"GameLogic_Layer")))
        return E_FAIL;*/

    return S_OK;
}

_int CMapStage::Update_Scene(const _float& fTimeDelta)
{
    if (m_pLoading->IsEnd() == false)
    {
        m_pBackGround->Update_GameObject(fTimeDelta);
        m_pLoading->Update_Loading();
        return 0;
    }

    int iExit = CStage::Update_Scene(fTimeDelta);

    Update_RoomLoading(fTimeDelta);

    return iExit;
}

void CMapStage::LateUpdate_Scene(const _float& fTimeDelta)
{
    CStage::LateUpdate_Scene(fTimeDelta);
}

void CMapStage::Render_Scene()
{
    if (m_pLoading->IsEnd() == false)
    {
        m_pBackGround->Render_GameObject();
    }
}

HRESULT CMapStage::Ready_ObjectPool()
{
    // ========== Pool 생성 ==========
    if (!Engine::CPoolMgr::GetInstance()->HasPool<CFloor>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CFloor>(m_pGraphicDev)))
        {
            MSG_BOX("Floor Pool Create Failed");
            return E_FAIL;
        }
    }

    if (!Engine::CPoolMgr::GetInstance()->HasPool<CDynamicFloor>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CDynamicFloor>(m_pGraphicDev)))
        {
            MSG_BOX("DynamicFloor Pool Create Failed");
            return E_FAIL;
        }
    }

    if (!Engine::CPoolMgr::GetInstance()->HasPool<CCeiling>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CCeiling>(m_pGraphicDev)))
        {
            MSG_BOX("Ceiling Pool Create Failed");
            return E_FAIL;
        }
    }

    if (!Engine::CPoolMgr::GetInstance()->HasPool<CWall>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CWall>(m_pGraphicDev)))
        {
            MSG_BOX("Wall Pool Create Failed");
            return E_FAIL;
        }
    }

    if (!Engine::CPoolMgr::GetInstance()->HasPool<CObstacle>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CObstacle>(m_pGraphicDev)))
        {
            MSG_BOX("Obstacle Pool Create Failed");
            return E_FAIL;
        }
    }

    if (!Engine::CPoolMgr::GetInstance()->HasPool<CBullet>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CBullet>(m_pGraphicDev)))
        {
            MSG_BOX("Bullet Pool Create Failed");
            return E_FAIL;
        }
    }

    if (!Engine::CPoolMgr::GetInstance()->HasPool<CWhiteMan>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CWhiteMan>(m_pGraphicDev)))
        {
            MSG_BOX("WhiteMan Pool Create Failed");
            return E_FAIL;
        }
    }

    if (!Engine::CPoolMgr::GetInstance()->HasPool<CBeamMon>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CBeamMon>(m_pGraphicDev)))
        {
            MSG_BOX("BeamMon Pool Create Failed");
            return E_FAIL;
        }
    }

    if (!Engine::CPoolMgr::GetInstance()->HasPool<CFlyMon>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CFlyMon>(m_pGraphicDev)))
        {
            MSG_BOX("FlyMon Pool Create Failed");
            return E_FAIL;
        }
    }

    return S_OK;
}


HRESULT CMapStage::Ready_Prototype_OnlyTexture()
{
   if(FAILED(Ready_PlayerTextureProto())) return E_FAIL;
   if(FAILED(Ready_MonsterTextureProto())) return E_FAIL;
   if(FAILED(Ready_TerrainTextureProto())) return E_FAIL;
   if (FAILED(Ready_UITextureProto())) return E_FAIL;
   return S_OK;
}

HRESULT CMapStage::Ready_PlayerTextureProto()
{
    CTexture* pCom_Texture = nullptr;

    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CLeftPart::GetTextureSources());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_LeftTexture", pCom_Texture)))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_LeftAnimation",
        Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CLeftPart::GetAnimSources()))))
        return E_FAIL;

    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CRightPart::GetTextureSources());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RightTexture", pCom_Texture)))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RightAnimation",
        Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CRightPart::GetAnimSources()))))
        return E_FAIL;

    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CMiddlePart::GetTextureSources());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_MiddleTexture", pCom_Texture)))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_MiddleAnimation",
        Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CMiddlePart::GetAnimSources()))))
        return E_FAIL;

    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CKatana::GetTextureSources());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_KatanaTexture", pCom_Texture)))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_KatanaAnimation",
        Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CKatana::GetAnimSources()))))
        return E_FAIL;

    return S_OK;
}

HRESULT CMapStage::Ready_MonsterTextureProto()
{
    CTexture* pCom_Texture = nullptr;

    // WhiteMan
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CWhiteMan::GetTextureSources());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_WhiteManTexture", pCom_Texture)))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_WhiteManAnimation", Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CWhiteMan::GetAnimSources()))))
        return E_FAIL;

    //Beam Mon Texture 
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CBeamMon::GetTextureSources());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BeamMonTexture", pCom_Texture)))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BeamMonAnimation", Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CBeamMon::GetAnimSources()))))
        return E_FAIL;

    //Beam Texture
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CBeam::GetTextureSources());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BeamTexture", pCom_Texture)))
        return E_FAIL;

    //FlyMon Texture
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CFlyMon::GetTextureSources());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_FlyMonTexture", pCom_Texture)))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_FlyMonAnimation", Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CFlyMon::GetAnimSources()))))
        return E_FAIL;

    //Bullet Texture
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CBullet::GetTextureSource());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BulletTexture", pCom_Texture)))
        return E_FAIL;


    return S_OK;
}

HRESULT CMapStage::Ready_TerrainTextureProto()
{
    CTexture* pCom_Texture = nullptr;

    // Floor Proto 
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CFloor::GetTextureSources());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Static_FloorTexture", pCom_Texture)))
        return E_FAIL;

    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CDynamicFloor::GetTextureSources());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Dynamic_FloorTexture", pCom_Texture)))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_FloorAnimation", 
        Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CDynamicFloor::GetAnimSources()))))
        return E_FAIL;

    // Ceiling Proto 
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CCeiling::GetTextureSources());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Static_CeilingTexture", pCom_Texture)))
        return E_FAIL;

    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CDynamicCeiling::GetTextureSources());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Dynamic_CeilingTexture", pCom_Texture)))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_CeilingAnimation",
        Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CDynamicCeiling::GetAnimSources()))))
        return E_FAIL;

    // Wall Proto 
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CWall::GetTextureSources());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Static_WallTexture", pCom_Texture)))
        return E_FAIL;

    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CDynamicWall::GetTextureSources());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Dynamic_WallTexture", pCom_Texture)))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_WallAnimation",
        Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CDynamicWall::GetAnimSources()))))
        return E_FAIL;

    return S_OK;
}

HRESULT CMapStage::Ready_UITextureProto()
{
    CTexture* pCom_Texture = nullptr;

    // Floor Proto 
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CPhoneBG::GetTextureSource());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_PhoneBGTexture", pCom_Texture)))
        return E_FAIL;


    return S_OK;
}

void CMapStage::Update_RoomLoading(const _float& fTimeDelta)
{
    CPlayer* pPlayer = nullptr;

    Engine::CTransform* pPlayerTransform = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()->
        Get_Component(ID_DYNAMIC, L"GameLogic_Layer", OBJ_PLAYER, L"Com_Transform"));

    if (pPlayerTransform == nullptr)
        return;

    _vec3 vPlayerPos;
    pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);

    // Door 구현이 안되어 있어서 우선 플레이어의 Z축 좌표를 기준으로 방 인덱스 재설정
    _int iNewRoomIndex = (_int)(vPlayerPos.z / 200.0f);      // 맵 찍으면서 설정해야 함(문 구현 전까지)

    // 방 변경 감지 
    if (iNewRoomIndex != m_iCurrentRoomIndex)
    {
        Change_Room(iNewRoomIndex);
    }
}

void CMapStage::Change_Room(_int iNewRoomIndex)
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

HRESULT CMapStage::Ready_Environment_Layer(const _tchar* pLayerTag)
{
    CLayer* pLayer = CLayer::Create();
    if (nullptr == pLayer)
        return E_FAIL;

    m_wstrCurrentMapFile = L"../../Map/test02.json";

    // 0번방 로드 
    if (FAILED(CMapLoader::GetInstance()->Load_Room(
        m_wstrCurrentMapFile,
        0,      // room Index 
        pLayer,
        m_pGraphicDev,
        pLayerTag)))
    {
        MessageBox(nullptr, L"Room 0 Load Failed", L"Error", MB_OK);
        return E_FAIL;
    }
    m_setLoadedRooms.insert(0);
    m_iCurrentRoomIndex = 0;

    // 1번방 미리 로드 
    if (FAILED(CMapLoader::GetInstance()->Load_Room(
        m_wstrCurrentMapFile,
        1,  // roomIndex
        pLayer,
        m_pGraphicDev,
        pLayerTag)))
    {
        // 1번방이 있으면 오류 체크 위해 주석 해제 
        MessageBox(nullptr, L"Room 1 Load Failed", L"Error", MB_OK);
        return E_FAIL;
    }
    else
    {
        m_setLoadedRooms.insert(1);
    }

    // 카메라 생성 (PlayerSpawn 위치 사용)
    _vec3 vPlayerSpawnPos = CMapLoader::GetInstance()->Get_PlayerSpawnPos();

    _vec3 vEye = vPlayerSpawnPos;
    _vec3 vAt = { vPlayerSpawnPos.x, vPlayerSpawnPos.y, vPlayerSpawnPos.z};
    _vec3 vUp = { 0.f, 1.f, 0.f };

    CGameObject* pGameObject = CFirstCamera::Create(m_pGraphicDev, &vEye, &vAt, &vUp);

    if (nullptr == pGameObject)
        return E_FAIL;

    if (FAILED(pLayer->Add_GameObject(pGameObject)))
        return E_FAIL;


    pGameObject = CDebugObject::Create(m_pGraphicDev, _vec3(62.f, 8.f, 80.f), _vec3(4.f, 16.f, 80.f));

    if (nullptr == pGameObject)
        return E_FAIL;

    if (FAILED(pLayer->Add_GameObject(pGameObject)))
        return E_FAIL;


    pGameObject = CDebugObject::Create(m_pGraphicDev, _vec3(80.f, 45.f, 80.f), _vec3(25.f, 6.f, 45.f));

    if (nullptr == pGameObject)
        return E_FAIL;

    if (FAILED(pLayer->Add_GameObject(pGameObject)))
        return E_FAIL;

   
    pGameObject = CDebugObject::Create(m_pGraphicDev, _vec3(100.f, 8.f, 80.f), _vec3(4.f, 16.f, 80.f));

    if (nullptr == pGameObject)
        return E_FAIL;

    if (FAILED(pLayer->Add_GameObject(pGameObject)))
        return E_FAIL;
       

    m_mapLayer.insert({ pLayerTag, pLayer });
    m_pEnvironment_Layer = pLayer;
    return S_OK;
}

HRESULT CMapStage::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
    CLayer* pLayer = CLayer::Create();
    if (nullptr == pLayer)
        return E_FAIL;

    // 0번 방 로드
    if (FAILED(CMapLoader::GetInstance()->Load_Room(
        m_wstrCurrentMapFile,
        0,
        pLayer,
        m_pGraphicDev,
        pLayerTag)))
    {
        MessageBox(nullptr, L"Room 0 (Monster) Load Failed", L"Erro", MB_OK);
        return E_FAIL;
    }
    m_setLoadedRooms.insert(0);
    m_iCurrentRoomIndex = 0;

    if (FAILED(CMapLoader::GetInstance()->Load_Room(
        m_wstrCurrentMapFile,
        1,
        pLayer,
        m_pGraphicDev,
        pLayerTag)))
    {
        /*MessageBox(nullptr, L"Room 1 (Monster) Load Failed", L"Erro", MB_OK);
        return E_FAIL;*/
    }
    else
    {
        m_setLoadedRooms.insert(1);
    }

    CGameObject* pGameObject = nullptr;

#pragma region Player
    CPlayer* pPlayer = nullptr;
    _vec3 pPlayerSpawnPos = CMapLoader::GetInstance()->Get_PlayerSpawnPos();

    pGameObject = pPlayer = CPlayer::Create(m_pGraphicDev, pPlayerSpawnPos);

    if (nullptr == pGameObject)
        return E_FAIL;

    if (FAILED(pLayer->Add_GameObject(pGameObject)))
        return E_FAIL;
#pragma endregion

    m_mapLayer.insert({ pLayerTag, pLayer });
    m_pGameLogic_Layer = pLayer;
    return S_OK;
}

HRESULT CMapStage::Ready_Prototype()
{
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RcTexUp", Engine::CRcTexUp::Create(m_pGraphicDev))))
        return E_FAIL;

    // RcTex (CFloor, CCeiling, CWall에서 사용)
    //if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RcTex", Engine::CRcTex::Create(m_pGraphicDev))))
    //    return E_FAIL;

    // CubeTex (CObstacle에서 사용)
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_CubeTex", Engine::CCubeTex::Create(m_pGraphicDev))))
        return E_FAIL;

    // Transform (모든 오브젝트에서 사용)
    //if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Transform", Engine::CTransform::Create(m_pGraphicDev))))
    //    return E_FAIL;

    // Collision 
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Collision", Engine::CCollision::Create(m_pGraphicDev))))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_StateComponent", Engine::CStateComponent::Create(m_pGraphicDev))))
        return E_FAIL;

    //if (FAILED(Ready_TerrainTextureProto()))
    //    return E_FAIL;

    //// 기존 몬스터 관련된 것들 패킹
    //if (FAILED(Ready_MonsterProto()))
    //    return E_FAIL;

    //// 기존 플레이어 프로토 등록 로직들 패킹
    //if (FAILED(Ready_PlayerTextureProto()))
    //    return E_FAIL;

    return S_OK;
}

CMapStage* CMapStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMapStage* pMapStage = new CMapStage(pGraphicDev);

    if (FAILED(pMapStage->Ready_Scene()))
    {
        Safe_Release(pMapStage);
        MSG_BOX("Map Stage Create Failed");
        return nullptr;
    }

    return pMapStage;
}

void CMapStage::Free()
{
    Safe_Release(m_pBackGround);
    Safe_Release(m_pLoading);
    CScene::Free();
}