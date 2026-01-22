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
#include "CSlopeFloor.h"

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

#include "CFontMgr.h"
#include "CUIManager.h"

//Effect
#include "CBlood.h"
#include "CTrail.h"
#include "CFlare.h"
#include "CExplosion.h"
#include "CBeamFlare.h"
#include "CBodyEmit.h"
#include "CHitUI.h"

#include "CLoadingEX.h"

#include "CCursor.h"
#include "CTextBG.h"
#include "CTextUI.h"

CMapStage::CMapStage(LPDIRECT3DDEVICE9 pGraphicDev) : CStage(pGraphicDev)
{
}

CMapStage::~CMapStage()
{
}

//각 단계 내의 Task 마다 임계영역이 겹치지 않아야 함
HRESULT CMapStage::Ready_Scene()
{
    m_pBackGround = CBackGround::Create(m_pGraphicDev);
    m_pLoadingEX = CLoadingEX::Create(m_pGraphicDev);

    if (!m_pLoadingEX) return E_FAIL;
    //1단계
    //이전 스테이지 이후 필요없는 오브젝트 풀 제거  
    m_pLoadingEX->AddTask(CLoadingEX::Lv1_INIT, [this]() { this->Remove_PrevObjectPool(); });
    //텍스쳐 제외 프로토타입 생성
    m_pLoadingEX->AddTask(CLoadingEX::Lv1_INIT, [this]() { this->Ready_Prototype(); });
    //캐릭터 텍스쳐 생성 
    m_pLoadingEX->AddTask(CLoadingEX::Lv1_INIT, [this]() { this->Ready_CharacterTextureProto(); });

    //2단계
    //1단계에서 만들어진 텍스쳐로 캐릭터 오브젝트 풀 생성 
    m_pLoadingEX->AddTask(CLoadingEX::Lv2_CHAR_RES, [this]() { this->Ready_ObjectPool_Character(); });
    m_pLoadingEX->AddTask(CLoadingEX::Lv2_CHAR_RES, [this]() { this->Ready_TerrainTextureProto(); });

    //3단계
    m_pLoadingEX->AddTask(CLoadingEX::Lv3_TERRAIN_RES, [this]() { this->Ready_ObjectPool_Terrain(); });
    m_pLoadingEX->AddTask(CLoadingEX::Lv3_TERRAIN_RES, [this]() { this->Ready_UITextureProto(); });

    //4단계
    m_pLoadingEX->AddTask(CLoadingEX::Lv4_UI_RES, [this]() { this->Ready_ObjectPool_UI(); });
    m_pLoadingEX->AddTask(CLoadingEX::Lv4_UI_RES, [this]() { this->Ready_EffectTextureProto(); });

    //5단계
    m_pLoadingEX->AddTask(CLoadingEX::Lv5_EFFECT_RES, [this]() { this->Ready_ObjectPool_Effect(); });

    //6단계 맵 - 환경 로드
    m_pLoadingEX->AddTask(CLoadingEX::Lv6_MAP_ENV_LOAD, [this]() { this->Ready_Environment_Layer(L"Environment_Layer"); });

    //7단계 맵 - 게임로직 로드 
    m_pLoadingEX->AddTask(CLoadingEX::Lv7_MAP_GAME_LOAD, [this]() { this->Ready_GameLogic_Layer(L"GameLogic_Layer"); });

    
    //메세지 구독 신청
    CEventMgr::GetInstance()->Subscribe(EVENT_DOOR_IN, this);
    CEventMgr::GetInstance()->Subscribe(EVENT_DOOR_OUT, this);

    return S_OK;
}

_int CMapStage::Update_Scene(const _float& fTimeDelta)
{
    if (m_pLoadingEX->IsEnd() == false)
    {
        m_pBackGround->Update_GameObject(fTimeDelta);
        m_pLoadingEX->Update_Loading();
        return 0;
    }
    int iExit = CStage::Update_Scene(fTimeDelta);
    //UI 업데이트
    CUIManager::GetInstance()->Update_GameObject(fTimeDelta);

    if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_P))
    {
        return RET_DEAD;
    }

    return iExit;
}

void CMapStage::LateUpdate_Scene(const _float& fTimeDelta)
{
    CStage::LateUpdate_Scene(fTimeDelta);

    //UI 업데이트
    CUIManager::GetInstance()->LateUpdate_GameObject(fTimeDelta);

    if (m_pLoadingEX->IsEnd()) Check_Collision();
}

void CMapStage::Render_Scene()
{

}

HRESULT CMapStage::Ready_Prototype()
{
    //Main으로 옮김 
    return S_OK;
}

HRESULT CMapStage::Remove_PrevObjectPool()
{
    return S_OK;
}

HRESULT CMapStage::Ready_ObjectPool_Character()
{
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

HRESULT CMapStage::Ready_ObjectPool_Terrain()
{
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

    if (!Engine::CPoolMgr::GetInstance()->HasPool<CSlopeFloor>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CSlopeFloor>(m_pGraphicDev)))
        {
            MSG_BOX("SlopeFloor Pool Create Failed");
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

    //if (!Engine::CPoolMgr::GetInstance()->HasPool<CTrigger>())
    //{
    //    if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CTrigger>(m_pGraphicDev)))
    //    {
    //        MSG_BOX("Trigger Pool Create Failed");
    //        return E_FAIL;
    //    }
    //}

    if (!Engine::CPoolMgr::GetInstance()->HasPool<CMapCollider>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CMapCollider>(m_pGraphicDev)))
        {
            MSG_BOX("MapCollider Pool Create Failed");
            return E_FAIL;
        }
    }

    if (!Engine::CPoolMgr::GetInstance()->HasPool<CDoorTrigger>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CDoorTrigger>(m_pGraphicDev)))
        {
            MSG_BOX("DoorTrigger Pool Create Failed");
            return E_FAIL;
        }
    }

    return S_OK;
}

HRESULT CMapStage::Ready_ObjectPool_UI()
{
    if (!Engine::CPoolMgr::GetInstance()->HasPool<CTextBG>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CTextBG>(m_pGraphicDev)))
        {
            MSG_BOX("UI TextBG Pool Create Failed");
            return E_FAIL;
        }
    }

    if (!Engine::CPoolMgr::GetInstance()->HasPool<CTextUI>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CTextUI>(m_pGraphicDev)))
        {
            MSG_BOX("UI TextUI Pool Create Failed");
            return E_FAIL;
        }
    }

    return S_OK;
}

HRESULT CMapStage::Ready_ObjectPool_Effect()
{
    if (!Engine::CPoolMgr::GetInstance()->HasPool<CBlood>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CBlood>(m_pGraphicDev)))
        {
            MSG_BOX("Effect Blood Pool Create Failed");
            return E_FAIL;
        }
    }

    if (!Engine::CPoolMgr::GetInstance()->HasPool<CTrail>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CTrail>(m_pGraphicDev)))
        {
            MSG_BOX("Effect Trail Pool Create Failed");
            return E_FAIL;
        }
    }

    if (!Engine::CPoolMgr::GetInstance()->HasPool<CFlare>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CFlare>(m_pGraphicDev)))
        {
            MSG_BOX("Effect Flare Pool Create Failed");
            return E_FAIL;
        }
    }

    if (!Engine::CPoolMgr::GetInstance()->HasPool<CExplosion>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CExplosion>(m_pGraphicDev)))
        {
            MSG_BOX("Effect Explosion Pool Create Failed");
            return E_FAIL;
        }
    }

    if (!Engine::CPoolMgr::GetInstance()->HasPool<CBeamFlare>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CBeamFlare>(m_pGraphicDev)))
        {
            MSG_BOX("Effect BeamFlare Pool Create Failed");
            return E_FAIL;
        }
    }


    if (!Engine::CPoolMgr::GetInstance()->HasPool<CBodyEmit>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CBodyEmit>(m_pGraphicDev)))
        {
            MSG_BOX("Effect BodyEmit Pool Create Failed");
            return E_FAIL;
        }
    }

    if (!Engine::CPoolMgr::GetInstance()->HasPool<CHitUI>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CHitUI>(m_pGraphicDev)))
        {
            MSG_BOX("Effect HitUI Pool Create Failed");
            return E_FAIL;
        }
    }
    return S_OK;
}


//HRESULT CMapStage::Ready_Prototype_OnlyTexture()
//{
//   //if(FAILED(Ready_PlayerTextureProto())) return E_FAIL;
//   if(FAILED(Ready_MonsterTextureProto())) return E_FAIL;
//   if(FAILED(Ready_TerrainTextureProto())) return E_FAIL;
//   if (FAILED(Ready_UITextureProto())) return E_FAIL;
//   if (FAILED(Ready_EffectTextureProto())) return E_FAIL;
//   return S_OK;
//}


HRESULT CMapStage::Ready_CharacterTextureProto()
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

    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CSlopeFloor::GetTextureSources());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Slope_FloorTexture", pCom_Texture)))
        return E_FAIL;

    //Dynamic Floor Proto
    pCom_Texture = Engine::CScrollTexture::Create(m_pGraphicDev, CDynamicFloor::GetTextureSources(),0.2f);
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Dynamic_FloorTexture", pCom_Texture)))
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
    

    CUIManager::GetInstance()->Ready_GameObject(m_pGraphicDev);

    return S_OK;
}

HRESULT CMapStage::Ready_EffectTextureProto()
{
    CTexture* pCom_Texture = nullptr;
    //Blood Texture
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CBlood::GetTextureSources());
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_Blood_Texture", pCom_Texture)))
    {
        MSG_BOX("Proto Blood Ready Failed");
        return E_FAIL;
    }

    //Trail 
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CTrail::GetTextureSource());
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_Trail_Texture", pCom_Texture)))
    {
        MSG_BOX("Proto Trail Ready Failed");
        return E_FAIL;
    }
    
    //Flare
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CFlare ::GetTextureSource());
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_Flare_Texture", pCom_Texture)))
    {
        MSG_BOX("Proto Flare Ready Failed");
        return E_FAIL;
    }

    //Explosion
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CExplosion::GetTextureSource());
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_EXP_Texture", pCom_Texture)))
    {
        MSG_BOX("Proto Explosion Ready Failed");
        return E_FAIL;
    }

    //BeamFlare
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CBeamFlare::GetTextureSource());
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_BeamFlare_Texture", pCom_Texture)))
    {
        MSG_BOX("Proto BeamFlare Ready Failed");
        return E_FAIL;
    }

    //BodyEmit
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CBodyEmit::GetTextureSources());
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_BodyEmit_Texture", pCom_Texture)))
    {
        MSG_BOX("Proto BodyEmit Ready Failed");
        return E_FAIL;
    }

    //HitUI
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CHitUI::GetTextureSource());
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_HitUI_Texture", pCom_Texture)))
    {
        MSG_BOX("Proto HitUI Ready Failed");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CMapStage::Ready_Environment_Layer(const _tchar* pLayerTag)
{
    CLayer* pLayer = CLayer::Create();
    if (nullptr == pLayer)
        return E_FAIL;

    const vector<wstring>& vecMapFiles = CMapLoader::GetInstance()->Get_MapFiles();

    if (!vecMapFiles.empty())
        m_wstrCurrentMapFile = vecMapFiles[1];      // TODO : Tutorial Map의 끝 Trigger Box에 닿으면 다음 맵 Loading호출
    else
        return E_FAIL;


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

    //CGameObject* pGameObject = nullptr;


    ////이 트리거가 있는 방의 번호 첫 인자로 입력
    //pGameObject = CDoorTrigger::Create(m_pGraphicDev, 0, _vec3(26.f, 10.f, 250.f), _vec3(16.f, 16.f, 16.f));
    //if (nullptr == pGameObject) return E_FAIL;
    //if (FAILED(pLayer->Add_GameObject(pGameObject))) return E_FAIL;

    //pGameObject = CMapCollider::Create(m_pGraphicDev, _vec3(-72.f, 10.f, 120), _vec3(16.f, 16.f, 70.f));
    //if (nullptr == pGameObject) return E_FAIL;
    //if (FAILED(pLayer->Add_GameObject(pGameObject))) return E_FAIL;

    //pGameObject = CMapCollider::Create(m_pGraphicDev, _vec3(122.f, 10.f, 120), _vec3(16.f, 16.f, 70.f));
    //if (nullptr == pGameObject) return E_FAIL;
    //if (FAILED(pLayer->Add_GameObject(pGameObject))) return E_FAIL;


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

    // 카메라 생성
    CGameObject* pPlayerObj = pLayer->Get_Object(OBJ_PLAYER);

    if (nullptr == pPlayerObj)
        return E_FAIL;

    CTransform* pTransform = dynamic_cast<CTransform*>(pPlayerObj->Get_Component(ID_DYNAMIC, L"Com_Transform"));

    if (nullptr == pTransform)
        return E_FAIL;

    _vec3 vPlayerPos = { 0.f, 0.f, 0.f };
    pTransform->Get_Info(INFO_POS, &vPlayerPos);

    _vec3 vEye = vPlayerPos;
    _vec3 vAt = { vPlayerPos.x, vPlayerPos.y, vPlayerPos.z };
    _vec3 vUp = { 0.f, 1.f, 0.f };

    CGameObject* pCamera = CFirstCamera::Create(m_pGraphicDev, &vEye, &vAt, &vUp);

    if (nullptr == pCamera)
        return E_FAIL;

    if (FAILED(pLayer->Add_GameObject(pCamera)))
        return E_FAIL;


    CGameObject* pGameObject = CCursor::Create(m_pGraphicDev);

    if (pGameObject == nullptr)
        return E_FAIL;

    if (FAILED(pLayer->Add_GameObject(pGameObject)))
        return E_FAIL;

    m_mapLayer.insert({ pLayerTag, pLayer });
    m_pGameLogic_Layer = pLayer;
    return S_OK;
}




void CMapStage::Check_Collision()
{
   auto iter_Map_Col = m_mapLayer[L"Environment_Layer"]->Get_Objects(OBJ_COL);
   auto iter_Map_Trigger = m_mapLayer[L"Environment_Layer"]->Get_Objects(OBJ_TRIGGER);
   auto iter_Map_Bullet = m_mapLayer[L"GameLogic_Layer"]->Get_Objects(OBJ_BULLET);
   auto iter_Map_Mon = m_mapLayer[L"GameLogic_Layer"]->Get_Objects(OBJ_MONSTER);
   CGameObject* player = m_mapLayer[L"GameLogic_Layer"]->Get_Object(OBJ_PLAYER);

   //vector<CCollider*> PlayerColliders;
   CCollider* pPlayerCollider = nullptr;
   if (player) 
   {
       CCharacter * cPlayer = static_cast<CCharacter*>(player);
       pPlayerCollider = cPlayer->GetCollider(); // Main Collider 만 받아옴 
   }

   //Player, Monster - 맵 콜라이더 충돌
   for (multimap<OBJ_ID, CGameObject*>::iterator it_col = iter_Map_Col.first; it_col != iter_Map_Col.second; it_col++)
   {
       //객체 당 한번만 casting함. 
       //OBJ_COL 에 들어있는 오브젝트는 모두 MapCollider만 넣는다는 전제하에 static_cast로  진행
       COLLIDER_TAG eMapColliderTag = TAG_NONE;

       CMapCollider* pMapCollider = static_cast<CMapCollider*>(it_col->second);
       if (pMapCollider) eMapColliderTag = pMapCollider->Get_ColliderTag();

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
       //플레이어한테만 태그 전달 
       if(pPlayerCollider) CCollision::Collision_Diff(pPlayerCollider, mapCollider, eMapColliderTag);

   }


   //-------------------------------------------------------------------
   //Player 충돌 
   //-------------------------------------------------------------------

   if (!pPlayerCollider) return;

   //Player- Trigger 충돌
   for (multimap<OBJ_ID, CGameObject*>::iterator it_tri = iter_Map_Trigger.first; it_tri != iter_Map_Trigger.second; it_tri++)
   {
       CCollision* mapTrig_Collision = static_cast<CCollision*>(it_tri->second->Get_Component(ID_DYNAMIC, L"Com_Collision"));
       CCollider* mapCollider = mapTrig_Collision->GetCollider();
       if (!mapCollider) continue;

       CCollision::Collision_Base(pPlayerCollider, mapCollider);
   }

   //Player- Bullet 충돌
   for (multimap<OBJ_ID, CGameObject*>::iterator it_bullet = iter_Map_Bullet.first; it_bullet != iter_Map_Bullet.second; it_bullet++)
   {
       CCollision* mapBul_Collision = static_cast<CCollision*>(it_bullet->second->Get_Component(ID_DYNAMIC, L"Com_Collision"));
       CCollider* mapCollider = mapBul_Collision->GetCollider();
       if (!mapCollider) continue;

       CCollision::Collision_Base(pPlayerCollider, mapCollider);
   }
   
   // Player 충돌 End
}

void CMapStage::OnEvent(EVENT_TYPE _type, EventData* _pData)
{
    if (_type == EVENT_DOOR_IN)
    {
        //if (m_iCurrentRoomIndex == 0) // 방이 언로드 되는 것을 보기 위해 임의로 2번 증가 시키기
        //    Change_Room(m_iCurrentRoomIndex + 1);
        
        Change_Room(m_iCurrentRoomIndex + 1);
    }
    else if (_type == EVENT_STAGE_END)
    {
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
    Safe_Release(m_pLoadingEX);
    CScene::Free();
    CFontMgr::GetInstance()->Clear_RenderFont();
}