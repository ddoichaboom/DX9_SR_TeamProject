#include "pch.h"
#include "CSniperStage.h"
#include "CProtoMgr.h"
#include "CMapLoader.h"
#include "CPoolMgr.h"
#include "CManagement.h"
#include "CDInputMgr.h"
#include "CCursor.h"
#include "CFontMgr.h"
#include "CRenderer.h"
#include "CLoadingEX.h"
#include "CBackGround.h"
#include "CUIManager.h"
#include "CSoundMgr.h"

//Character
#include "CWhiteMan.h"
#include "CSniperPlayer.h"
#include "CSniperWhiteMan.h"
#include "CBullet.h"
#include "CLeftPart.h"
#include "CSRightHand.h"
#include "CBeam.h"

//UI
#include "CSniperUI.h"
#include "CPhoneBG.h"
#include "CTargetUI.h"

//Effect
#include "CBlood.h"
#include "CExplosion.h"
#include "CHitUI.h"


CSniperStage::CSniperStage(LPDIRECT3DDEVICE9 pGraphicDev) 
	: CStage(pGraphicDev), m_pPlayer(nullptr)
{
}

CSniperStage::~CSniperStage()
{
}

HRESULT CSniperStage::Ready_Scene()
{
	m_pBackGround = CBackGround::Create(m_pGraphicDev);
	m_pLoadingEX = CLoadingEX::Create(m_pGraphicDev);

	const vector<wstring>& vecMapFiles = CMapLoader::GetInstance()->Get_MapFiles();
	if (!vecMapFiles.empty())
		m_wstrCurrentMapFile = vecMapFiles[3];
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

	
	CEventMgr::GetInstance()->Subscribe(EVENT_MONSTER_DEAD, this);

	return S_OK;
}

_int CSniperStage::Update_Scene(const _float& fTimeDelta)
{
	if (m_pLoadingEX->IsEnd() == false)
	{
		m_pBackGround->Update_GameObject(fTimeDelta);
		m_pLoadingEX->Update_Loading(fTimeDelta);
		return 0;
	}
	else if (!m_bStartSound)
	{
		CSoundMgr::GetInstance()->StopAll();
		CSoundMgr::GetInstance()->PlayBGM(CSniperPlayer::szSniperMapBGM.c_str(), 0.4f);
		m_bStartSound = true;
	}
	
	if (m_bStageEnd || (CDInputMgr::GetInstance()->Key_Down(DIK_P)))
	{
		CUIManager::GetInstance()->Clear_SniperUI();
		return RET_DEAD;
	}
	
	int iExit = CStage::Update_Scene(fTimeDelta);
	CUIManager::GetInstance()->Update_GameObject(fTimeDelta);

	
	//스폰할 몬스터도 없고 맵에도 몬스터가 없다면 종료 
	if (m_qMonsterSpawnPoses.empty() && m_pGameLogic_Layer->Get_Object(OBJ_MONSTER) == nullptr)
	{
		m_bStageEnd = true;
		return 0;
	}

	m_fTime += fTimeDelta;
	if (m_fTime > m_fSpawnTime)
	{
		SpawnMonster();

		m_fTime = 0.f;
		if (m_bFirstSpawn)
		{
			m_fSpawnTime = m_fOriginSpawnTime;
			m_bFirstSpawn = false;
		}
		if (m_fSpawnTime > m_fMinTime)
		{
			m_fSpawnTime -= m_fOffsetTime;
			if (m_fSpawnTime < m_fMinTime) m_fSpawnTime = m_fMinTime;
		}

	}

	return iExit;
}

void CSniperStage::LateUpdate_Scene(const _float& fTimeDelta)
{
	CStage::LateUpdate_Scene(fTimeDelta);
	CUIManager::GetInstance()->LateUpdate_GameObject(fTimeDelta);
}

void CSniperStage::Render_Scene()
{
}

HRESULT CSniperStage::Ready_Environment_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

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

	m_mapLayer.insert({ pLayerTag, pLayer });
	m_pEnvironment_Layer = pLayer;

	return S_OK;
}

HRESULT CSniperStage::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)	return E_FAIL;

	//플레이어 생성 
	m_pPlayer = CSniperPlayer::Create(m_pGraphicDev);
	m_pPlayer->SetPos({ 0,720,0 });

	if (m_pPlayer == nullptr) return E_FAIL;
	if(FAILED(pLayer->Add_GameObject(m_pPlayer))) return E_FAIL;

	//Monster Spawn 위치 저장 
	if (FAILED(CMapLoader::GetInstance()->Get_MonsterSpawnPoses(
		m_wstrCurrentMapFile,
		0, pLayer, m_pGraphicDev, pLayerTag, m_qMonsterSpawnPoses)))
	{
		MSG_BOX("Sniper Map - Load Monster Spawn Pos Failed");
		return E_FAIL;
	}

	m_mapLayer.insert({ pLayerTag, pLayer });
	m_pGameLogic_Layer = pLayer;

	return S_OK;

}

HRESULT CSniperStage::Ready_Prototype()
{
	return S_OK;
}

HRESULT CSniperStage::Remove_PrevObjectPool()
{
	return S_OK;
}

HRESULT CSniperStage::Ready_ObjectPool_Character()
{
	if (!Engine::CPoolMgr::GetInstance()->HasPool<CSniperWhiteMan>())
	{
		if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CSniperWhiteMan>(m_pGraphicDev)))
		{
			MSG_BOX("SniperWhiteMan Pool Create Failed");
			return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CSniperStage::Ready_ObjectPool_Terrain()
{
	return S_OK;
}

HRESULT CSniperStage::Ready_ObjectPool_UI()
{
	CUIManager::GetInstance()->Set_VecTargetUI(m_pGraphicDev);
	return S_OK;
}

HRESULT CSniperStage::Ready_ObjectPool_Effect()
{
	if (!Engine::CPoolMgr::GetInstance()->HasPool<CBlood>())
	{
		if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CBlood>(m_pGraphicDev)))
		{
			MSG_BOX("Effect Blood Pool Create Failed");
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
	return S_OK;
}

HRESULT CSniperStage::Ready_CharacterTextureProto()
{
	CTexture* pCom_Texture = nullptr;

	////Beam Texture
	//pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CBeam::GetTextureSources());
	//if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BeamTexture", pCom_Texture)))
	//	return E_FAIL;

	//Sniper WhiteMan
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CSniperWhiteMan::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_SniperWhiteManTexture", pCom_Texture)))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_SniperManAnimation", Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CSniperWhiteMan::GetAnimSources()))))
		return E_FAIL;
	
	// RightHand
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CSRightHand::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_SniperRightTexture", pCom_Texture)))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_SniperRightAnimation", Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CSRightHand::GetAnimSources()))))
		return E_FAIL;

	//Left Hand 는 Main에 위치

	return S_OK;
}

HRESULT CSniperStage::Ready_TerrainTextureProto()
{
	return S_OK;
}

HRESULT CSniperStage::Ready_UITextureProto()
{
	CTexture* pCom_Texture = nullptr;
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CSniperUI::GetTextureSource());
	if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_SniperUITexture", pCom_Texture)))
	{
		MSG_BOX("Sniper UI Ready Failed");
		return E_FAIL;
	}

	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CTargetUI::GetTextureSource());
	if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TargetUITexture", pCom_Texture)))
	{
		MSG_BOX("Sniper UI Ready Failed");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSniperStage::Ready_EffectTextureProto()
{
	CTexture* pCom_Texture = nullptr;
	return S_OK;
}

void CSniperStage::Check_Collision()
{
}

void CSniperStage::SpawnMonster()
{
	if (m_qMonsterSpawnPoses.empty()) return;

	_vec3 spawnPos = m_qMonsterSpawnPoses.front(); m_qMonsterSpawnPoses.pop();
	CSniperWhiteMan* man = CPoolMgr::GetInstance()->Get_Object<CSniperWhiteMan>();
	if (!man) return;

	man->SetPos(spawnPos);
	if (m_pGameLogic_Layer) m_pGameLogic_Layer->Add_GameObject(man);
	else man->ReturnToPool();
}

CSniperStage* CSniperStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSniperStage* pSniperStage = new CSniperStage(pGraphicDev);

	if (FAILED(pSniperStage->Ready_Scene()))
	{
		Safe_Release(pSniperStage);
		MSG_BOX("Sniper Stage Create Failed");
		return nullptr;
	}

	return pSniperStage;
}

void CSniperStage::OnEvent(EVENT_TYPE _type, EventData* _pData)
{
	if (_type == EVENT_MONSTER_DEAD)
	{
		CUIManager::GetInstance()->Add_TargetUI();
	}
}

void CSniperStage::Free()
{
	Safe_Release(m_pBackGround);
	Safe_Release(m_pLoadingEX);
	CScene::Free();
	CFontMgr::GetInstance()->Clear_RenderFont();
}