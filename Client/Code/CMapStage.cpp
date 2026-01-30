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
#include "CRoomTrigger.h"
#include "CSlopeFloor.h"
#include "CDoor.h"
#include "CDoorLeft.h"
#include "CDoorRight.h"
#include "CVendingMachine.h"
#include "CDisplayObject.h"
#include "CWindow.h"
#include "CDisplayCubeObject.h"

// 게임 로직 오브젝트
#include "CPlayer.h"
#include "CFirstCamera.h"
#include "CWhiteMan.h"
#include "CBeamMon.h"
#include "CFlyMon.h"
#include "CBullet.h"
#include "CBeam.h"

#include "CSoda.h"
#include "CAxe.h"
#include "CExtinguisher.h"


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
#include "CTakeDownBlood.h"
#include "CSodaUI.h"

#include "CLoadingEX.h"

#include "CCursor.h"
#include "CTextBG.h"
#include "CTextUI.h"

#include "CSoundMgr.h"

CMapStage::CMapStage(LPDIRECT3DDEVICE9 pGraphicDev) 
    : CStage(pGraphicDev)
    , m_iFileIndex(0)
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

    const vector<wstring>& vecMapFiles = CMapLoader::GetInstance()->Get_MapFiles();

    if (!vecMapFiles.empty())
        m_wstrCurrentMapFile = vecMapFiles[m_iFileIndex];
    else
        return E_FAIL;

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
    CEventMgr::GetInstance()->Subscribe(EVENT_ROOM_CHANGE, this);
    CEventMgr::GetInstance()->Subscribe(EVENT_STAGE_END, this);
    CEventMgr::GetInstance()->Subscribe(EVENT_NEXT_STAGE, this);

    return S_OK;
}

_int CMapStage::Update_Scene(const _float& fTimeDelta)
{
    if (m_pLoadingEX->IsEnd() == false)
    {
        m_pBackGround->Update_GameObject(fTimeDelta);
        m_pLoadingEX->Update_Loading(fTimeDelta);
        return 0;
    }
    int iExit = CStage::Update_Scene(fTimeDelta);
    //UI 업데이트
    CUIManager::GetInstance()->Update_GameObject(fTimeDelta);

    if (m_bStageEnd || (CDInputMgr::GetInstance()->Key_Down(DIK_P)))
    {
        return RET_DEAD;
    }


    if (CDInputMgr::GetInstance()->Get_DIKeyState(DIK_K))
    {
        CUIManager::GetInstance()->Change_UIState(UI_TAKEDOWN);
    }

    if (CDInputMgr::GetInstance()->Mouse_Down(DIM_LB))
    {
        //TODO : 사운드 매니저 예시. 제거하기 
        //주소, 볼륨 - 현재 삽입하는 채널그룹의 전체 볼륨이 다같이 조정됨 
        //SoundMgr::GetInstance()->PlayPlayerSound(szPlayerShoot.c_str(), 0.8f);
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

    if (!Engine::CPoolMgr::GetInstance()->HasPool<CSoda>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CSoda>(m_pGraphicDev)))
        {
            MSG_BOX("Soda Pool Create Failed");
            return E_FAIL;
        }
    }

    if (!Engine::CPoolMgr::GetInstance()->HasPool<CAxe>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CAxe>(m_pGraphicDev)))
        {
            MSG_BOX("Axe Pool Create Failed");
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

    if (!Engine::CPoolMgr::GetInstance()->HasPool<CDynamicWall>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CDynamicWall>(m_pGraphicDev)))
        {
            MSG_BOX("Dynamic Wall Pool Create Failed");
            return E_FAIL;
        }
    }

    if (!Engine::CPoolMgr::GetInstance()->HasPool<CVendingMachine>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CVendingMachine>(m_pGraphicDev)))
        {
            MSG_BOX("VendingMachine Pool Create Failed");
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

    if (!Engine::CPoolMgr::GetInstance()->HasPool<CRoomTrigger>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CRoomTrigger>(m_pGraphicDev)))
        {
            MSG_BOX("RoomTrigger Pool Create Failed");
            return E_FAIL;
        }
    }

    if (!Engine::CPoolMgr::GetInstance()->HasPool<CDoorLeft>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CDoorLeft>(m_pGraphicDev)))
        {
            MSG_BOX("DoorLeft Pool Create Failed");
            return E_FAIL;
        }
    }

    if (!Engine::CPoolMgr::GetInstance()->HasPool<CDoorRight>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CDoorRight>(m_pGraphicDev)))
        {
            MSG_BOX("DoorRight Pool Create Failed");
            return E_FAIL;
        }
    }

    if (!Engine::CPoolMgr::GetInstance()->HasPool<CDoor>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CDoor>(m_pGraphicDev)))
        {
            MSG_BOX("Door Pool Create Failed");
            return E_FAIL;
        }
    }


    if (!Engine::CPoolMgr::GetInstance()->HasPool<CDisplayObject>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CDisplayObject>(m_pGraphicDev)))
        {
            MSG_BOX("DisplayObject Pool Create Failed");
            return E_FAIL;
        }
    }

    if (!Engine::CPoolMgr::GetInstance()->HasPool<CDisplayCubeObject>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CDisplayCubeObject>(m_pGraphicDev)))
        {
            MSG_BOX("DisplayCubeObject Pool Create Failed");
            return E_FAIL;
        }
    }

    if (!Engine::CPoolMgr::GetInstance()->HasPool<CExtinguisher>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CExtinguisher>(m_pGraphicDev)))
        {
            MSG_BOX("Extinguisher Pool Create Failed");
            return E_FAIL;
        }
    }

    if (!Engine::CPoolMgr::GetInstance()->HasPool<CWindow>())
    {
        if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CWindow>(m_pGraphicDev)))
        {
            MSG_BOX("Window Pool Create Failed");
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


    //Soda Texture
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CSoda::GetTextureSources());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_SodaTexture", pCom_Texture)))
        return E_FAIL;

    //Axe Texture
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CAxe::GetTextureSources());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_AxeTexture", pCom_Texture)))
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

    CCubeTexture* pCom_Cube_Texture = nullptr;

    // Obstacle(VendingMachine) Proto 
    pCom_Cube_Texture = Engine::CCubeTexture::Create(m_pGraphicDev, CVendingMachine::GetTextureSources());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_VendingMachine_Texture", pCom_Cube_Texture)))
        return E_FAIL;

    //Display
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CDisplayObject::GetTextureSources());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_DisplayTexture", pCom_Texture)))
        return E_FAIL;

    // DisplayCube
    pCom_Cube_Texture = Engine::CCubeTexture::Create(m_pGraphicDev, CDisplayCubeObject::GetTextureSources());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_DisplayCubeObject_Texture", pCom_Cube_Texture)))
        return E_FAIL;

    // Extinguisher
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CExtinguisher::GetTextureSources());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_ExtinguisherTexture", pCom_Texture)))
        return E_FAIL;

    // DoorLeft Texture Proto 
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CDoorLeft::GetTextureSources());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_DoorLeft_Texture_Door", pCom_Texture)))
        return E_FAIL;

    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CDoorRight::GetTextureSources());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_DoorRight_Texture_Door", pCom_Texture)))
        return E_FAIL;

    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CWindow::GetTextureSources());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_WindowTexture_Door", pCom_Texture)))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_WindowAnimation",
        Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CWindow::GetAnimSources()))))
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

    //SodaUI
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CSodaUI::GetTextureSource());
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_SodaUI_Texture", pCom_Texture)))
    {
        MSG_BOX("Proto SodaUI Ready Failed");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CMapStage::Ready_Environment_Layer(const _tchar* pLayerTag)
{
    CLayer* pLayer = CLayer::Create();
    if (nullptr == pLayer)
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

    CTransform* pTransform = static_cast<CTransform*>(pPlayerObj->Get_Component(ID_DYNAMIC, L"Com_Transform"));

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
   auto iter_Map_Door = m_mapLayer[L"Environment_Layer"]->Get_Objects(OBJ_DOOR);  // 추가
   auto iter_Map_Vending = m_mapLayer[L"Environment_Layer"]->Get_Objects(OBJ_VENDINGMACHINE);  // 추가
   auto iter_Map_Bullet = m_mapLayer[L"GameLogic_Layer"]->Get_Objects(OBJ_BULLET);
   auto iter_Map_Mon = m_mapLayer[L"GameLogic_Layer"]->Get_Objects(OBJ_MONSTER);
   auto iter_Map_Item = m_mapLayer[L"GameLogic_Layer"]->Get_Objects(OBJ_ITEM);
   CGameObject* pLayer = m_mapLayer[L"GameLogic_Layer"]->Get_Object(OBJ_PLAYER);

   //vector<CCollider*> PlayerColliders;
   CCollider* pPlayerCollider = nullptr;
   if (pLayer) 
   {
       CCharacter * cPlayer = static_cast<CCharacter*>(pLayer);
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
           CCollision::Collision_Diff(monCollider, mapCollider, eMapColliderTag);
       }
       //플레이어 
       //플레이어한테만 태그 전달 
       if(pPlayerCollider) CCollision::Collision_Diff(pPlayerCollider, mapCollider, eMapColliderTag);

       //총알
       for (multimap<OBJ_ID, CGameObject*>::iterator it_bullet = iter_Map_Bullet.first; it_bullet != iter_Map_Bullet.second; it_bullet++)
       {
           CCollision* mapBul_Collision = static_cast<CCollision*>(it_bullet->second->Get_Component(ID_DYNAMIC, L"Com_Collision"));
           CCollider* bulletCollider = mapBul_Collision->GetCollider();
           if (!bulletCollider) continue;

           CCollision::Collision_Base(mapCollider, bulletCollider);
       }
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

   //Player - ITEM 충돌
   for (multimap<OBJ_ID, CGameObject*>::iterator it_Item = iter_Map_Item.first; it_Item != iter_Map_Item.second; it_Item++)
   {
       CCollision* mapItem_Collision = static_cast<CCollision*>(it_Item->second->Get_Component(ID_DYNAMIC, L"Com_Collision"));
       CCollider* mapCollider = mapItem_Collision->GetCollider();
       if (!mapCollider) continue;

       CCollision::Collision_Base(pPlayerCollider, mapCollider);
   }
   
   // Door 충돌 처리 
   // Player의 Collision 컴포넌트 가져오기
   CCollision* pPlayerCollision = nullptr;
   CCollider* pPlayerKickCollider = nullptr;
   if (pLayer)
   {
       pPlayerCollision = static_cast<CCollision*>(
           pLayer->Get_Component(ID_DYNAMIC, L"Com_Collision"));

       if (pPlayerCollision)
       {
           pPlayerKickCollider = pPlayerCollision->GetCollider(L"ColKick");
       }
   }


   for (multimap<OBJ_ID, CGameObject*>::iterator it_door = iter_Map_Door.first; it_door != iter_Map_Door.second; it_door++)
   {
       CCollision* mapItem_Collision = static_cast<CCollision*>(it_door->second->Get_Component(ID_DYNAMIC, L"Com_Collision"));
       CCollider* mapCollider = mapItem_Collision->GetCollider();
       if (!mapCollider) continue;

       CCollision::Collision_Base(pPlayerKickCollider, mapCollider);
   }


   for (multimap<OBJ_ID, CGameObject*>::iterator it_mon = iter_Map_Mon.first; it_mon != iter_Map_Mon.second; it_mon++)
   {
       CCollision* mapMon_Collision = static_cast<CCollision*>(it_mon->second->Get_Component(ID_DYNAMIC, L"Com_Collision"));
       CCollider* pMonCollider = mapMon_Collision->GetCollider();
       if (!pMonCollider)
           continue;

      

       for (multimap<OBJ_ID, CGameObject*>::iterator it_door = iter_Map_Door.first; it_door != iter_Map_Door.second; it_door++)
       {
           CCollision* pDoorCollision = static_cast<CCollision*>(
               it_door->second->Get_Component(ID_DYNAMIC, L"Com_Collision"));

           if (!pDoorCollision)
               continue;

           CCollider* pDoorCollider = pDoorCollision->GetCollider();
           if (!pDoorCollider)
               continue;

           CCollision::Collision_Base(pMonCollider, pDoorCollider);
       }

       
       for (multimap<OBJ_ID, CGameObject*>::iterator it_vend = iter_Map_Vending.first; it_vend != iter_Map_Vending.second; it_vend++)
       {
           CCollision* pVendCollision = static_cast<CCollision*>(
               it_vend->second->Get_Component(ID_DYNAMIC, L"Com_Collision"));
       
           if (!pVendCollision)
               continue;
       
           CCollider* pVenCollider = pVendCollision->GetCollider();
           if (!pVenCollider)
               continue;
       
           CCollision::Collision_Diff(pMonCollider, pVenCollider , TAG_ELECTRIC);
       }
       
   }

  
}

void CMapStage::OnEvent(EVENT_TYPE _type, EventData* _pData)
{
    if (_type == EVENT_ROOM_CHANGE)
    {
        Change_Room(m_iCurrentRoomIndex + 1);
    }
    else if (_type == EVENT_NEXT_STAGE)
    {
        const vector<wstring>& vecMapFiles = CMapLoader::GetInstance()->Get_MapFiles();

        if (m_iFileIndex == 0)
        {
            set<_int> roomsToUnload;
            for (_int iLoadedRoom : m_setLoadedRooms)
            {
                roomsToUnload.insert(iLoadedRoom);
            }

            for (_int iRoomToUnload : roomsToUnload)
            {
                // Environment_Layer 언로드
                CMapLoader::GetInstance()->Unload_Room(
                    m_wstrCurrentMapFile,
                    iRoomToUnload,
                    m_pEnvironment_Layer);

                // GameLogic_Layer 언로드
                CMapLoader::GetInstance()->Unload_Room(
                    m_wstrCurrentMapFile,
                    iRoomToUnload,
                    m_pGameLogic_Layer);

                m_setLoadedRooms.erase(iRoomToUnload);
            }

            m_iCurrentRoomIndex = 0;
            m_iFileIndex++;
            m_wstrCurrentMapFile = vecMapFiles[m_iFileIndex];

            for (_int iRoomToLoad = 0; iRoomToLoad < 2; iRoomToLoad++)
            {
                _bool bSuccess = true;

                if (FAILED(CMapLoader::GetInstance()->Load_Room(
                    m_wstrCurrentMapFile,
                    iRoomToLoad,
                    m_pEnvironment_Layer,
                    m_pGraphicDev,
                    L"Environment_Layer")))
                {
                    bSuccess = false;
                }

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

            CGameObject* pPlayer = Get_Layer(L"GameLogic_Layer")->Get_Object(OBJ_PLAYER);

            if (pPlayer == nullptr)
                return;

            static_cast<CPlayer*>(pPlayer)->Change_Weapon(WEAPON_KATANA);
            CManagement::GetInstance()->Set_FloorNumber();
            CManagement::GetInstance()->Reset_CountTime();
            CManagement::GetInstance()->Set_CountTime(true);
        }
        else
            m_bStageEnd = true;
    }
    else if (_type == EVENT_STAGE_END)
    {
        auto iter_Map_Mon = m_mapLayer[L"GameLogic_Layer"]->Get_Objects(OBJ_MONSTER);
        for (multimap<OBJ_ID, CGameObject*>::iterator it_mon = iter_Map_Mon.first; it_mon != iter_Map_Mon.second; it_mon++)
        {            
            it_mon->second->ReturnToPool();            
        }
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