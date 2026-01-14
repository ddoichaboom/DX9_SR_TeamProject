#include "pch.h"
#include "CMapStage.h"
#include "CProtoMgr.h"
#include "CMapLoader.h"
#include "CPoolMgr.h"

// 환경 오브젝트 (필터링용, 실제 생성은 CMapLoader가 담당)
#include "CFloor.h"
#include "CCeiling.h"
#include "CWall.h"
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
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_FloorTexture", pCom_Texture)))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_FloorAnimation", 
        Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CFloor::GetAnimSources()))))
        return E_FAIL;

    // Ceiling Proto
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CCeiling::GetTextureSources());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_CeilingTexture", pCom_Texture)))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_CeilingAnimation",
        Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CCeiling::GetAnimSources()))))
        return E_FAIL;

    // Wall Proto
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CWall::GetTextureSources());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_WallTexture", pCom_Texture)))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_WallAnimation",
        Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CWall::GetAnimSources()))))
        return E_FAIL;

    return S_OK;
}

HRESULT CMapStage::Ready_Environment_Layer(const _tchar* pLayerTag)
{
    CLayer* pLayer = CLayer::Create();
    if (nullptr == pLayer)
        return E_FAIL;

    // Map 데이터 로드
    if (FAILED(CMapLoader::GetInstance()->Load_MapData(
        L"../../Map/test.json",
        pLayer,  // Environment_Layer에 추가
        m_pGraphicDev)))
    {
        MessageBox(nullptr, L"Map Load Failed", L"Error", MB_OK);
        return E_FAIL;
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

    return S_OK;
}

HRESULT CMapStage::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
    CLayer* pLayer = CLayer::Create();
    if (nullptr == pLayer)
        return E_FAIL;

    CGameObject* pGameObject = nullptr;

#pragma region Player
    CPlayer* pPlayer = nullptr;
    _vec3 pPlayerSpawnPos = CMapLoader::GetInstance()->Get_PlayerSpawnPos();

    pGameObject = pPlayer = CPlayer::Create(m_pGraphicDev, pPlayerSpawnPos);

    if (nullptr == pGameObject)
        return E_FAIL;

    if (FAILED(pLayer->Add_GameObject(pGameObject)))
        return E_FAIL;

    //pPlayer->Intro_Func();
#pragma endregion

    

     auto& monsterSpawns = CMapLoader::GetInstance()->Get_MonsterSpawns();

     _uint iMonsterIndex = 0;
     for (auto& pair : monsterSpawns)
     {
         string MonsterKey = pair.first;            // "WhiteMan", 추가 몬스터
         vector<_vec3> Positions = pair.second;

         for (auto& vPos : Positions)
         {
             CGameObject* pMonster = nullptr;

             if (MonsterKey == "WhiteMan")
             {
                 // Pool에서 가져오기
                 CWhiteMan* pWhiteMan = CPoolMgr::GetInstance()->Get_Object<CWhiteMan>();
                 if (pWhiteMan)
                 {
                     pWhiteMan->SetPos(vPos);

                     pMonster = pWhiteMan;
                 }
             }
             else if (MonsterKey == "BeamMon")
             {
                 CBeamMon* pBeamMon = CPoolMgr::GetInstance()->Get_Object<CBeamMon>();
                 if (pBeamMon)
                 {
                     pBeamMon->SetPos(vPos);
                     pMonster = pBeamMon;
                 }
             }
             else if (MonsterKey == "FlyMon")
             {
                 CFlyMon* pFlyMon = CPoolMgr::GetInstance()->Get_Object<CFlyMon>();
                 if (pFlyMon)
                 {
                     pFlyMon->SetPos(vPos);
                     pMonster = pFlyMon;
                 }
             }

             if (pMonster)
             {
                 if (FAILED(pLayer->Add_GameObject(pMonster)))
                 {
                     // Pool 객체는 ReturnToPool 호출 
                     pMonster->ReturnToPool();
                 }
             }
         }
     }
    m_mapLayer.insert({ pLayerTag, pLayer });

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