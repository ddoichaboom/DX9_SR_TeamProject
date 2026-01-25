#include "pch.h"
#include "CBossStage.h"
#include "CProtoMgr.h"
#include "CMapLoader.h"
#include "CPoolMgr.h"
#include "CManagement.h"
#include "CDInputMgr.h"
#include "CVideoMgr.h"
#include "CSoundMgr.h"

// 환경 오브젝트 (필터링용, 실제 생성은 CMapLoader가 담당)
#include "CFloor.h"
#include "CDynamicFloor.h"
#include "CCeiling.h"
#include "CDynamicCeiling.h"
#include "CWall.h"
#include "CDynamicWall.h"
#include "CObstacle.h"
#include "CRoomTrigger.h"
#include "CSlopeFloor.h"

// 게임 로직 오브젝트
#include "CPlayer.h"
#include "CFirstCamera.h"
#include "CBoss.h"
#include "CBossBullet.h"
#include "CRocket.h"
//풀 삭제용
#include "CWhiteMan.h"
#include "CBeamMon.h"
#include "CFlyMon.h"
//Player
#include "CLeftPart.h"
#include "CRightPart.h"
#include "CMiddlePart.h"
#include "CPistol.h"
#include "CKatana.h"

#include "CLoading.h"
#include "CBackGround.h"
#include "CMapCollider.h"


#include "CUIManager.h"
#include "CCursor.h"

//Effect
#include "CBlood.h"
#include "CTrail.h"
#include "CFlare.h"
#include "CExplosion.h"
#include "CBeamFlare.h"
#include "CBodyEmit.h"
#include "CHitUI.h"
#include "CToonFlash.h"
#include "CToonFog.h"
#include "CBossTrail.h"
#include "CBossHPUI.h"

#include "CLoadingEX.h"

CBossStage::CBossStage(LPDIRECT3DDEVICE9 pGraphicDev) 
    : CStage(pGraphicDev)
{
}

CBossStage::~CBossStage()
{
}

HRESULT CBossStage::Ready_Scene()
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

    CEventMgr::GetInstance()->Subscribe(EVENT_ENDING, this);

    return S_OK;
}

_int CBossStage::Update_Scene(const _float& fTimeDelta)
{
    if (m_pLoadingEX->IsEnd() == false)
    {
        m_pBackGround->Update_GameObject(fTimeDelta);
        m_pLoadingEX->Update_Loading();
        if (m_pLoadingEX->IsEnd())
        {
            if (FAILED(CVideoMgr::GetInstance()->ReadyVideo(g_hWnd, m_BossVideoName.c_str())))
            {
                CVideoMgr::GetInstance()->SetPlayFlag(false);
            }
            else
            {
                CVideoMgr::GetInstance()->Play();
                CSoundMgr::GetInstance()->PlaySFXSound(m_BossSoundName.c_str());
            }
        }
        return RET_NONE;
    }
    else if (CVideoMgr::GetInstance()->IsPlaying())
    {
        if (CDInputMgr::GetInstance()->Key_Down(DIK_P) || CVideoMgr::GetInstance()->IsFinished())
        {
            CVideoMgr::GetInstance()->SetPlayFlag(false);
            CVideoMgr::GetInstance()->Cleanup();
            //CSoundMgr::GetInstance()->StopAll();
        }
        else return 0;
    }

    if (m_bStageEnd)
    {
        return RET_DEAD;
    }

    int iExit = CStage::Update_Scene(fTimeDelta);

    //UI 업데이트
    CUIManager::GetInstance()->Update_GameObject(fTimeDelta);


    return iExit;
}

void CBossStage::LateUpdate_Scene(const _float& fTimeDelta)
{
    if (CVideoMgr::GetInstance()->IsPlaying() || m_pLoadingEX->IsEnd() == false) return;
    CStage::LateUpdate_Scene(fTimeDelta);

    //UI 업데이트
    CUIManager::GetInstance()->LateUpdate_GameObject(fTimeDelta);
    Check_Collision();
}

void CBossStage::Render_Scene()
{
    if (CVideoMgr::GetInstance()->IsPlaying()) return;
    if (m_pLoadingEX->IsEnd() == false)
    {
        m_pBackGround->Render_GameObject();
    }
}

HRESULT CBossStage::Ready_Environment_Layer(const _tchar* pLayerTag)
{
    CLayer* pLayer = CLayer::Create();
    if (nullptr == pLayer)
        return E_FAIL;

    const vector<wstring>& vecMapFiles = CMapLoader::GetInstance()->Get_MapFiles();

    if (!vecMapFiles.empty())
        m_wstrCurrentMapFile = vecMapFiles[2];      // Map/BossStage.json으로 저장
    else
    {
        MSG_BOX("vecMapFiles is empty");
        return E_FAIL;
    }

    // 보스 스테이지는 0번 방만 존재
    if (FAILED(CMapLoader::GetInstance()->Load_Room(
        m_wstrCurrentMapFile,
        0,
        pLayer,
        m_pGraphicDev,
        pLayerTag)))
    {
        MessageBox(nullptr, L"BossStage (Environment) Load Failed", L"Error", MB_OK);
        return E_FAIL;
    }

    m_mapLayer.insert({ pLayerTag, pLayer });
    m_pEnvironment_Layer = pLayer;
    return S_OK;
}

HRESULT CBossStage::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
    CGameObject* pGameObject = nullptr;
    CLayer* pLayer = CLayer::Create();
    if (nullptr == pLayer)
        return E_FAIL;

    // 보스 스테이지는 0번 방만 존재 
    if (FAILED(CMapLoader::GetInstance()->Load_Room(
        m_wstrCurrentMapFile,
        0,
        pLayer,
        m_pGraphicDev,
        pLayerTag)))
    {
        MessageBox(nullptr, L"BossStage (GameLogic) Load Failed", L"Erro", MB_OK);
        return E_FAIL;
    }

    //CPlayer* pPlayer = nullptr;
    //_vec3 pPlayerSpawnPos = { 0,0,0 };

    //pGameObject = pPlayer = CPlayer::Create(m_pGraphicDev, pPlayerSpawnPos);

    //if (nullptr == pGameObject)
    //    return E_FAIL;

    //if (FAILED(pLayer->Add_GameObject(pGameObject)))
    //    return E_FAIL;

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


    // 커서 생성
    pGameObject = CCursor::Create(m_pGraphicDev);

    if (pGameObject == nullptr)
        return E_FAIL;

    if (FAILED(pLayer->Add_GameObject(pGameObject)))
        return E_FAIL;


    m_mapLayer.insert({ pLayerTag, pLayer });
    m_pGameLogic_Layer = pLayer;
    return S_OK;

}


HRESULT CBossStage::Ready_Prototype()
{
	return S_OK;
}

HRESULT CBossStage::Remove_PrevObjectPool()
{
    CPoolMgr::GetInstance()->DeletePool<CWhiteMan>();
    CPoolMgr::GetInstance()->DeletePool<CBeamMon>();
    CPoolMgr::GetInstance()->DeletePool<CFlyMon>();
    CPoolMgr::GetInstance()->DeletePool<CBullet>();
    //Effect Pool
    CPoolMgr::GetInstance()->DeletePool<CBeamFlare>();
    CPoolMgr::GetInstance()->DeletePool<CBodyEmit>();

    // TODO : CMapStage에서 오브젝트 풀 생성했던거 여기서 안쓰면 해제하기 (아직 미정)
    return S_OK;
}

HRESULT CBossStage::Ready_ObjectPool_Character()
{
    if (!CPoolMgr::GetInstance()->HasPool<CBossBullet>())
    {
        if (FAILED(CPoolMgr::GetInstance()->CreatePool<CBossBullet>(m_pGraphicDev)))
        {
            MSG_BOX("Bullet Pool Create Failed");
            return E_FAIL;
        }
    }

    if (!CPoolMgr::GetInstance()->HasPool<CRocket>())
    {
        if (FAILED(CPoolMgr::GetInstance()->CreatePool<CRocket>(m_pGraphicDev)))
        {
            MSG_BOX("Rocket Pool Create Failed");
            return E_FAIL;
        }
    }

    return S_OK;
}

HRESULT CBossStage::Ready_ObjectPool_Terrain()
{
    if (!Engine::CPoolMgr::GetInstance()->HasPool<CFloor>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CFloor>(m_pGraphicDev)))
        {
            MSG_BOX("Floor Pool Create Failed");
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

    if (!Engine::CPoolMgr::GetInstance()->HasPool<CMapCollider>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CMapCollider>(m_pGraphicDev)))
        {
            MSG_BOX("MapCollider Pool Create Failed");
            return E_FAIL;
        }
    }


    return S_OK;
}

HRESULT CBossStage::Ready_ObjectPool_UI()
{
    return S_OK;
}

HRESULT CBossStage::Ready_ObjectPool_Effect()
{
    return S_OK;
}

HRESULT CBossStage::Ready_CharacterTextureProto()
{
    CTexture* pCom_Texture = nullptr;

    //BOSS Texture
    pCom_Texture = CTexture::Create(m_pGraphicDev, CBoss::GetTextureSources());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BossTexture", pCom_Texture)))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BossAnimation", CAnimation::Create(m_pGraphicDev, pCom_Texture, CBoss::GetAnimSources()))))
        return E_FAIL;

    //Boss Bullet Texture
    pCom_Texture = CTexture::Create(m_pGraphicDev, CBossBullet::GetTextureSource());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BossBulletTexture", pCom_Texture)))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BossBulletAnimation", CAnimation::Create(m_pGraphicDev, pCom_Texture, CBossBullet::GetAnimSource()))))
        return E_FAIL;

    //Boss Rocket Texture
    pCom_Texture = CTexture::Create(m_pGraphicDev, CRocket::GetTextureSource());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RocketTexture", pCom_Texture)))
        return E_FAIL;

    //Boss ToonFlash Texture
    pCom_Texture = CTexture::Create(m_pGraphicDev, CToonFlash::GetTextureSource());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_ToonFlash_Texture", pCom_Texture)))
        return E_FAIL;

    //Boss ToonFog Texture
    pCom_Texture = CTexture::Create(m_pGraphicDev, CToonFog::GetTextureSource());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_ToonFog_Texture", pCom_Texture)))
        return E_FAIL;

    //BossTrail Texture
    pCom_Texture = CTexture::Create(m_pGraphicDev, CBossTrail::GetTextureSource());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_BossTrail_Texture", pCom_Texture)))
        return E_FAIL;

    //BossHPUI Texture
    pCom_Texture = CTexture::Create(m_pGraphicDev, CBossHPUI::GetTextureSource());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_BOSSHPUI_Texture", pCom_Texture)))
        return E_FAIL;

    return S_OK;
}


HRESULT CBossStage::Ready_TerrainTextureProto()
{
    CTexture* pCom_Texture = nullptr;

    //// Floor Proto 
    //pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CFloor::GetTextureSources());
    //if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Static_FloorTexture", pCom_Texture)))
    //    return E_FAIL;

    //pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CWall::GetTextureSources());
    //if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Static_WallTexture", pCom_Texture)))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CBossStage::Ready_UITextureProto()
{
    CTexture* pCom_Texture = nullptr;
    return S_OK;
}

HRESULT CBossStage::Ready_EffectTextureProto()
{
    CTexture* pCom_Texture = nullptr;
    return S_OK;
}

void CBossStage::Check_Collision()
{
    auto iter_Map_Col = m_mapLayer[L"Environment_Layer"]->Get_Objects(OBJ_COL);
    auto iter_Map_Bullet = m_mapLayer[L"GameLogic_Layer"]->Get_Objects(OBJ_BULLET);
    CGameObject* player = m_mapLayer[L"GameLogic_Layer"]->Get_Object(OBJ_PLAYER);

    CCollider* pPlayerCollider = nullptr;
    if (player)
    {
        CCharacter* cPlayer = static_cast<CCharacter*>(player);
        pPlayerCollider = cPlayer->GetCollider(); // Main Collider 만 받아옴 
    }

    //Player- Bullet 충돌
    for (multimap<OBJ_ID, CGameObject*>::iterator it_bullet = iter_Map_Bullet.first; it_bullet != iter_Map_Bullet.second; it_bullet++)
    {
        CCollision* mapBul_Collision = static_cast<CCollision*>(it_bullet->second->Get_Component(ID_DYNAMIC, L"Com_Collision"));
        CCollider* mapCollider = mapBul_Collision->GetCollider();
        if (!mapCollider) continue;

        CCollision::Collision_Base(pPlayerCollider, mapCollider);
    }

    //Player, Monster - 맵 콜라이더 충돌
    for (multimap<OBJ_ID, CGameObject*>::iterator it_col = iter_Map_Col.first; it_col != iter_Map_Col.second; it_col++)
    {
        //맵에 있는 콜라이더
        CCollision* mapCol_Collision = static_cast<CCollision*>(it_col->second->Get_Component(ID_DYNAMIC, L"Com_Collision"));
        CCollider* mapCollider = mapCol_Collision->GetCollider();
        if (!mapCollider) continue;

        //플레이어 
        if (pPlayerCollider) CCollision::Collision_Diff(pPlayerCollider, mapCollider);

        //총알
        for (multimap<OBJ_ID, CGameObject*>::iterator it_bullet = iter_Map_Bullet.first; it_bullet != iter_Map_Bullet.second; it_bullet++)
        {
            CCollision* mapBul_Collision = static_cast<CCollision*>(it_bullet->second->Get_Component(ID_DYNAMIC, L"Com_Collision"));
            CCollider* bulletCollider = mapBul_Collision->GetCollider();
            if (!bulletCollider) continue;

            CCollision::Collision_Base(mapCollider, bulletCollider);
        }

    }


}

void CBossStage::OnEvent(EVENT_TYPE _type, EventData* _pData)
{
    if (_type == EVENT_ENDING)
    {
        m_bStageEnd = true;
    }
}

CBossStage* CBossStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CBossStage* pBossStage = new CBossStage(pGraphicDev);

    if (FAILED(pBossStage->Ready_Scene()))
    {
        Safe_Release(pBossStage);
        MSG_BOX("Boss Stage Create Failed");
        return nullptr;
    }

    return pBossStage;
}


void CBossStage::Free()
{
    Safe_Release(m_pBackGround);
    Safe_Release(m_pLoadingEX);
    CScene::Free();
    //CVideoMgr::GetInstance()->Cleanup();
}