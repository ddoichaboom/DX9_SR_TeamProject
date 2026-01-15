#include "pch.h"
#include "CMapStage.h"
#include "CProtoMgr.h"
#include "CMapLoader.h"
#include "CPoolMgr.h"
#include "CManagement.h"
#include "CDInputMgr.h"

// 환경 오브젝트 (필터링용, 실제 생성은 CMapLoader가 담당)
#include "CFloor.h"
#include "CDynamicFloor.h"
#include "CCeiling.h"
#include "CDynamicCeiling.h"
#include "CWall.h"
#include "CDynamicWall.h"
#include "CObstacle.h"
#include "CDoorTrigger.h"

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
#include "CMapCollider.h"


//UI
#include "CPhoneBG.h"
#include "CShopBG.h"
#include "CShopItem.h"


CMapStage::CMapStage(LPDIRECT3DDEVICE9 pGraphicDev) : CStage(pGraphicDev), m_pLoading(nullptr)
, m_BaseResult(E_FAIL), m_TextureResult(E_FAIL), m_ObjectPoolResult(E_FAIL)
, m_ReadyEnvResult(E_FAIL), m_ReadyGameResult(E_FAIL)
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
    
    //메세지 구독 신청
    CEventMgr::GetInstance()->Subscribe(EVENT_DOOR_IN, this);
    CEventMgr::GetInstance()->Subscribe(EVENT_DOOR_OUT, this);

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

    if(m_pLoading->IsEnd()) Check_Collision();
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

    if (!Engine::CPoolMgr::GetInstance()->HasPool<CTrigger>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CTrigger>(m_pGraphicDev)))
        {
            MSG_BOX("Trigger Pool Create Failed");
            return E_FAIL;
        }
    }

    return S_OK;
}


HRESULT CMapStage::Ready_Prototype_OnlyTexture()
{
   //if(FAILED(Ready_PlayerTextureProto())) return E_FAIL;
   if(FAILED(Ready_MonsterTextureProto())) return E_FAIL;
   if(FAILED(Ready_TerrainTextureProto())) return E_FAIL;
   if (FAILED(Ready_UITextureProto())) return E_FAIL;
   return S_OK;
}

//HRESULT CMapStage::Ready_PlayerTextureProto()
//{
//    CTexture* pCom_Texture = nullptr;
//
//    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CLeftPart::GetTextureSources());
//    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_LeftTexture", pCom_Texture)))
//        return E_FAIL;
//
//    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_LeftAnimation",
//        Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CLeftPart::GetAnimSources()))))
//        return E_FAIL;
//
//    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CRightPart::GetTextureSources());
//    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RightTexture", pCom_Texture)))
//        return E_FAIL;
//
//    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RightAnimation",
//        Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CRightPart::GetAnimSources()))))
//        return E_FAIL;
//
//    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CMiddlePart::GetTextureSources());
//    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_MiddleTexture", pCom_Texture)))
//        return E_FAIL;
//
//    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_MiddleAnimation",
//        Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CMiddlePart::GetAnimSources()))))
//        return E_FAIL;
//
//    return S_OK;
//}

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

    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CShopBG::GetTextureSources());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_ShopBGTexture", pCom_Texture)))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_ShopBGAnimation",
        Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CShopBG::GetAnimSources()))))
        return E_FAIL;

    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CShopItem::GetTextureSources());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_ShopItemTexture", pCom_Texture)))
        return E_FAIL;

    return S_OK;
}

void CMapStage::Update_RoomLoading(const _float& fTimeDelta)
{
    //CPlayer* pPlayer = nullptr;

    //Engine::CTransform* pPlayerTransform = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()->
    //    Get_Component(ID_DYNAMIC, L"GameLogic_Layer", OBJ_PLAYER, L"Com_Transform"));

    //if (pPlayerTransform == nullptr)
    //    return;

    //_vec3 vPlayerPos;
    //pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);

    //// Door 구현이 안되어 있어서 우선 플레이어의 Z축 좌표를 기준으로 방 인덱스 재설정
    //_int iNewRoomIndex = (_int)(vPlayerPos.z / 200.0f);      // 맵 찍으면서 설정해야 함(문 구현 전까지)

    // 테스트용 ( U / I 키로 룸 인덱스 증감 )
    _int iNewRoomIndex = m_iCurrentRoomIndex;

    if (Engine::CDInputMgr::GetInstance()->Key_Down(DIK_U))
        iNewRoomIndex++;
    else if (Engine::CDInputMgr::GetInstance()->Key_Down(DIK_I) && (iNewRoomIndex > 0))
    {
         iNewRoomIndex--;
    }

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

    m_wstrCurrentMapFile = L"../../Map/Tutorial.json";

    // 0번방 로드 
    if (FAILED(CMapLoader::GetInstance()->Load_Room(
        m_wstrCurrentMapFile,
        0,      // room Index 
        pLayer,
        m_pGraphicDev,
        pLayerTag)))
    {
        MessageBox(nullptr, L"Room 0 (Environment) Load Failed", L"Error", MB_OK);
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
        MessageBox(nullptr, L"Room 1 (Environment) Load Failed", L"Error", MB_OK);
        return E_FAIL;
    }
    else
    {
        m_setLoadedRooms.insert(1);
    }

    // 카메라 생성 (PlayerSpawn 위치 사용)
    _vec3 vPlayerSpawnPos = CMapLoader::GetInstance()->Get_PlayerSpawnPos();

    _vec3 vEye = vPlayerSpawnPos;
    _vec3 vAt = { vPlayerSpawnPos.x, vPlayerSpawnPos.y, vPlayerSpawnPos.z };
    _vec3 vUp = { 0.f, 1.f, 0.f };

    CGameObject* pGameObject = CFirstCamera::Create(m_pGraphicDev, &vEye, &vAt, &vUp);

    if (nullptr == pGameObject)
        return E_FAIL;

    if (FAILED(pLayer->Add_GameObject(pGameObject)))
        return E_FAIL;


    //이 트리거가 있는 방의 번호 첫 인자로 입력
    CDoorTrigger* trigger = CDoorTrigger::Create(m_pGraphicDev, 0, _vec3(28.f, 8.f, 250.f), _vec3(16.f, 16.f, 16.f));
    
    pGameObject = trigger;
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
        MessageBox(nullptr, L"Room 0 (GameLogic) Load Failed", L"Erro", MB_OK);
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
        MessageBox(nullptr, L"Room 1 (GameLogic) Load Failed", L"Erro", MB_OK);
        return E_FAIL;
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

//Main으로 옮김 
HRESULT CMapStage::Ready_Prototype()
{
    // RcTex (CFloor, CCeiling, CWall에서 사용)
    //if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RcTex", Engine::CRcTex::Create(m_pGraphicDev))))
    //    return E_FAIL;

    // CubeTex (CObstacle에서 사용)
    //if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_CubeTex", Engine::CCubeTex::Create(m_pGraphicDev))))
    //    return E_FAIL;

    // Transform (모든 오브젝트에서 사용)
    //if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Transform", Engine::CTransform::Create(m_pGraphicDev))))
    //    return E_FAIL;

    // Collision 

    //if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_StateComponent", Engine::CStateComponent::Create(m_pGraphicDev))))
    //    return E_FAIL;

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


void CMapStage::Check_Collision()
{
   auto iter_Map_Col = m_mapLayer[L"Environment_Layer"]->Get_Objects(OBJ_COL);
   auto iter_Map_Trigger = m_mapLayer[L"Environment_Layer"]->Get_Objects(OBJ_TRIGGER);
   auto iter_Map_Mon = m_mapLayer[L"GameLogic_Layer"]->Get_Objects(OBJ_MONSTER);
   CGameObject* player = m_mapLayer[L"GameLogic_Layer"]->Get_Object(OBJ_PLAYER);

   vector<CCollider*> PlayerColliders;

   if (player) 
   {
       CCharacter * cPlayer = static_cast<CCharacter*>(player);
       cPlayer->GetAllCollider(PlayerColliders);
   }

   //Player, Monster - 맵 콜라이더 충돌
   for (multimap<OBJ_ID, CGameObject*>::iterator it_col = iter_Map_Col.first; it_col != iter_Map_Col.second; it_col++)
   {
       //맵에 있는 콜라이더
       CCollision* mapCol_Collision = static_cast<CCollision*>(it_col->second->Get_Component(ID_DYNAMIC, L"Com_Collision"));
       CCollider * mapCollider =  mapCol_Collision->GetCollider();
       if (!mapCollider) continue;

       //몬스터
       for (auto it_mon = iter_Map_Mon.first; it_mon != iter_Map_Mon.second; it_mon++)
       {
           CCharacter* monster = static_cast<CCharacter*>(it_mon->second);
           CCollider* monCollider = monster->GetCollider();
           if (!monCollider) continue;
           //주의 맵을 마지막 인자로 들어가기 
           CCollision::Collision_Diff(monCollider, mapCollider);
       }
       //플레이어 
       for (auto& collider : PlayerColliders)
       {
           CCollision::Collision_Diff(collider, mapCollider);
       }

   }
   //Player- Trigger 충돌
   for (multimap<OBJ_ID, CGameObject*>::iterator it_tri = iter_Map_Trigger.first; it_tri != iter_Map_Trigger.second; it_tri++)
   {
       CCollision* mapTrig_Collision = static_cast<CCollision*>(it_tri->second->Get_Component(ID_DYNAMIC, L"Com_Collision"));
       CCollider* mapCollider = mapTrig_Collision->GetCollider();
       if (!mapCollider) continue;

       for (auto& collider : PlayerColliders)
       {
           CCollision::Collision_Base(collider, mapCollider);
       }
   }

}

void CMapStage::OnEvent(EVENT_TYPE _type, EventData* _pData)
{
    if (_type == EVENT_DOOR_IN)
    {
        if (m_iCurrentRoomIndex == 0) // 방이 언로드 되는 것을 보기 위해 임의로 2번 증가 시키기
            Change_Room(m_iCurrentRoomIndex + 1);
        
        Change_Room(m_iCurrentRoomIndex + 1);
    }
    
    else if (_type == EVENT_DOOR_OUT)
    {
        Change_Room(m_iCurrentRoomIndex - 1);
    }
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