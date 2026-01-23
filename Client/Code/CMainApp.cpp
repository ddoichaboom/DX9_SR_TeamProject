#include "pch.h"
#include "CMainApp.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CPoolMgr.h"
#include "CMapStage.h"
#include "CBossStage.h"
#include "CMapLoader.h"
#include "CEventMgr.h"
#include "CFontMgr.h"

#include <ctime>

#include "CBullet.h"
#include "CWhiteMan.h"
#include "CBeamMon.h"
#include "CFlyMon.h"
#include "CBoss.h"
#include "CBossBullet.h"
#include "CRocket.h"
#include "CTerrain.h"
#include "CPlayer.h"
#include "CFloor.h"
#include "CCeiling.h"
#include "CWall.h"
#include "CDynamicFloor.h"
#include "CDynamicCeiling.h"
#include "CDynamicWall.h"
#include "CObstacle.h"
#include "CSlopeFloor.h"
#include "CDoorTrigger.h"
#include "CMapCollider.h"

#include "CLeftPart.h"
#include "CRightPart.h"
#include "CMiddlePart.h"
#include "CKatana.h"
#include "CBackGround.h"
#include "CTrigger.h"
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

CMainApp::CMainApp() : m_pDeviceClass(nullptr), m_pGraphicDev(nullptr)
, m_pManagementClass(CManagement::GetInstance()), m_eCurSceneType(SCENE_NONE)
{
}

CMainApp::~CMainApp()
{
}

HRESULT CMainApp::Ready_MainApp()
{
	srand(unsigned(time(NULL)));

	if (FAILED(Ready_DefaultSetting(&m_pGraphicDev)))
		return E_FAIL;
	
	if (FAILED(Ready_DefaultProto()))
		return E_FAIL;

	if (FAILED(Ready_ObjectPool()))
		return E_FAIL;

	if (FAILED(Ready_Scene(m_pGraphicDev)))
		return E_FAIL;

	return S_OK;
}

int CMainApp::Update_MainApp(const float& fTimeDelta)
{
	CDInputMgr::GetInstance()->Update_InputDev();
	_int iExit = m_pManagementClass->Update_Scene(fTimeDelta);
	if (iExit == RET_DEAD) SetNextScene();
	return 0;
}

void CMainApp::LateUpdate_MainApp(const float& fTimeDelta)
{
	m_pManagementClass->LateUpdate_Scene(fTimeDelta);
}

void CMainApp::Render_MainApp()
{
	m_pDeviceClass->Render_Begin(D3DXCOLOR(0.f, 0.f, 0.f, 1.f));
	m_pManagementClass->Render_Scene(m_pGraphicDev);
	m_pDeviceClass->Render_End();
}

HRESULT CMainApp::Ready_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev)
{
	if (FAILED(CGraphicDev::GetInstance()->Ready_GraphicDev(g_hWnd, MODE_WIN,
		WINCX, WINCY, &m_pDeviceClass)))
	{
		MSG_BOX("GraphicDev Ready Failed");
		return E_FAIL;
	}

	m_pDeviceClass->AddRef();

	(*ppGraphicDev) = m_pDeviceClass->Get_GraphicDev();
	(*ppGraphicDev)->AddRef();

	//(*ppGraphicDev)->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	(*ppGraphicDev)->SetRenderState(D3DRS_LIGHTING, FALSE);


	if (FAILED(CDInputMgr::GetInstance()->Ready_InputDev(g_hInst, g_hWnd)))
		return E_FAIL;

	(*ppGraphicDev)->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	(*ppGraphicDev)->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);

	(*ppGraphicDev)->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);	
	(*ppGraphicDev)->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	(*ppGraphicDev)->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR); 

	//비등방성 필터링 - 픽셀이 모든 방향으로 크기가 일정하지않을 때, (카메라의 외곽쪽에 가까워져서 텍스쳐가 늘려 보일경우 등) 
	//기울어진 정도를 체크하여 샘플링할건지에 대한 설정 (max = 16) 
	//(*ppGraphicDev)->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	//(*ppGraphicDev)->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 16);

	//텍스쳐를 사용하는 모든 오브젝트는 텍스쳐 공간 변환을 U,V 2차원으로만 한다 
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	return S_OK;
}

HRESULT CMainApp::Ready_DefaultProto()
{
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Transform", Engine::CTransform::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RcTexUp", Engine::CRcTexUp::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RcTex", Engine::CRcTex::Create(m_pGraphicDev))))
		return E_FAIL;

	// CubeTex (CObstacle에서 사용)
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_CubeTex", Engine::CCubeTex::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_StateComponent", CStateComponent::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Collision", CCollision::Create(m_pGraphicDev))))
		return E_FAIL;

	//Loading Proto
	CTexture* pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CBackGround::GetTextureSource());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_LoadingTexture", pCom_Texture)))
		return E_FAIL;

	//Player Proto 
	pCom_Texture = CTexture::Create(m_pGraphicDev, CLeftPart::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_LeftTexture", pCom_Texture)))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_LeftAnimation",
		CAnimation::Create(m_pGraphicDev, pCom_Texture, CLeftPart::GetAnimSources()))))
		return E_FAIL;

	pCom_Texture = CTexture::Create(m_pGraphicDev, CRightPart::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RightTexture", pCom_Texture)))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RightAnimation",
		CAnimation::Create(m_pGraphicDev, pCom_Texture, CRightPart::GetAnimSources()))))
		return E_FAIL;

	pCom_Texture = CTexture::Create(m_pGraphicDev, CMiddlePart::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_MiddleTexture", pCom_Texture)))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_MiddleAnimation",
		CAnimation::Create(m_pGraphicDev, pCom_Texture, CMiddlePart::GetAnimSources()))))
		return E_FAIL;

	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CKatana::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_KatanaTexture", pCom_Texture)))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_KatanaAnimation",
		Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CKatana::GetAnimSources()))))
		return E_FAIL;

	//TakeDownBlood
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CTakeDownBlood::GetTextureSource());
	if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_TakeDownBlood_Texture", pCom_Texture)))
	{
		MSG_BOX("Proto TakeDownBlood Ready Failed");
		return E_FAIL;
	}


	//Font
	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_Number", L"DS-Digital", 40, 40, FW_BOLD, false, true)))
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_Default", L"견명조", 15, 15, FW_HEAVY,false, false)))
		return E_FAIL;


	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_Word", L"Noto Sans KR", 30, 30, FW_HEAVY, false, true)))	
		return E_FAIL;

	if (FAILED(CFontMgr::GetInstance()->Ready_Font(m_pGraphicDev, L"Font_LargeWord", L"Noto Sans KR", 60, 60, FW_HEAVY, true, true)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev)
{
	//Engine::CScene* pInitScene = CTestStage::Create(pGraphicDev);
	Engine::CScene* pInitScene = CMapStage::Create(pGraphicDev);
	m_eCurSceneType = SCENE_BATTLE;
	//Engine::CScene* pInitScene = CBossTestStage::Create(pGraphicDev);

	if (nullptr == pInitScene)
		return E_FAIL;

	if (FAILED(m_pManagementClass->Set_Scene(pInitScene)))
	{
		Safe_Release(pInitScene);
		MSG_BOX("Init Scene Setting Failed");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMainApp::Ready_ObjectPool()
{
	for (auto& wstrFile : CMapLoader::GetInstance()->Get_MapFiles())
	{
		if (FAILED(CMapLoader::GetInstance()->Preload_AllMapData(wstrFile)))
		{
			MSG_BOX("Map Preload Failed");
			return E_FAIL;
		}
	}

	// 타입별 최대값 초기화
	_uint iMaxFloor(0), iMaxDynamicFloor(0),iMaxCeiling(0), iMaxDynamicCeiling(0), iMaxWall(0), iMaxDynamicWall(0), iMaxSlopeFloor(0);
	_uint iMaxObstacle(0);
	_uint iMaxMapCollider(0), iMaxDoorTrigger(0);
	_uint iBulletCount = 30;
	_uint iBossBulletCount = 30;
	_uint iBossRocketCount = 30;
	_uint iWhiteManCount = 6;
	_uint iBeamMonCount = 6;
	_uint iFlyMonCount = 6;
	


	//Effect
	_uint iBloodCount = 6;
	_uint iTrailCount = 4;
	_uint iFlareCount = 3;
	_uint iExplosionCount = 6;
	_uint iBeamFlareCount = 6;
	_uint iBodyEmitCount = 6;
	_uint iHitUICount = 2;

	for (auto& wstrFile : CMapLoader::GetInstance()->Get_MapFiles())
	{
		// 각 파일에서 타입별 최대 개수 추출 
		iMaxFloor = max(iMaxFloor,
			CMapLoader::GetInstance()->Get_MaxObjectCount(wstrFile, "Floor"));
		iMaxDynamicFloor = max(iMaxDynamicFloor,
			CMapLoader::GetInstance()->Get_MaxObjectCount(wstrFile, "DynamicFloor"));
		iMaxSlopeFloor = max(iMaxSlopeFloor,
			CMapLoader::GetInstance()->Get_MaxObjectCount(wstrFile, "SlopeFloor"));

		iMaxCeiling = max(iMaxCeiling,
			CMapLoader::GetInstance()->Get_MaxObjectCount(wstrFile, "Ceiling"));
		iMaxDynamicCeiling = max(iMaxDynamicCeiling,
			CMapLoader::GetInstance()->Get_MaxObjectCount(wstrFile, "DynamicCeiling"));

		iMaxWall = max(iMaxWall,
			CMapLoader::GetInstance()->Get_MaxObjectCount(wstrFile, "Wall"));
		iMaxDynamicWall = max(iMaxDynamicWall,
			CMapLoader::GetInstance()->Get_MaxObjectCount(wstrFile, "DynamicWall"));

		iMaxObstacle = max(iMaxObstacle,
			CMapLoader::GetInstance()->Get_MaxObjectCount(wstrFile, "Cube"));

		iMaxMapCollider = max(iMaxMapCollider,
			CMapLoader::GetInstance()->Get_MaxObjectCount(wstrFile, "MapCollider"));

		iMaxDoorTrigger = max(iMaxDoorTrigger,
			CMapLoader::GetInstance()->Get_MaxObjectCount(wstrFile, "DoorTriggerBox"));
	}

	// 풀 크기 설정 
	CPoolMgr::GetInstance()->SetPoolSize<CFloor>(iMaxFloor);
	CPoolMgr::GetInstance()->SetPoolSize<CDynamicFloor>(iMaxDynamicFloor);
	CPoolMgr::GetInstance()->SetPoolSize<CSlopeFloor>(iMaxSlopeFloor);

	CPoolMgr::GetInstance()->SetPoolSize<CCeiling>(iMaxCeiling);
	CPoolMgr::GetInstance()->SetPoolSize<CDynamicCeiling>(iMaxDynamicCeiling);

	CPoolMgr::GetInstance()->SetPoolSize<CWall>(iMaxWall);
	CPoolMgr::GetInstance()->SetPoolSize<CDynamicWall>(iMaxDynamicWall);

	CPoolMgr::GetInstance()->SetPoolSize<CObstacle>(iMaxObstacle);

	CPoolMgr::GetInstance()->SetPoolSize<CBullet>(iBulletCount);
	CPoolMgr::GetInstance()->SetPoolSize<CBossBullet>(iBossBulletCount);
	CPoolMgr::GetInstance()->SetPoolSize<CRocket>(iBossRocketCount);
	CPoolMgr::GetInstance()->SetPoolSize<CWhiteMan>(iWhiteManCount);
	CPoolMgr::GetInstance()->SetPoolSize<CBeamMon>(iBeamMonCount);
	CPoolMgr::GetInstance()->SetPoolSize<CFlyMon>(iFlyMonCount);

	CPoolMgr::GetInstance()->SetPoolSize<CDoorTrigger>(iMaxDoorTrigger);

	CPoolMgr::GetInstance()->SetPoolSize<CBlood>(iBloodCount);
	CPoolMgr::GetInstance()->SetPoolSize<CTrail>(iTrailCount);
	CPoolMgr::GetInstance()->SetPoolSize<CFlare>(iFlareCount);
	CPoolMgr::GetInstance()->SetPoolSize<CExplosion>(iExplosionCount);
	CPoolMgr::GetInstance()->SetPoolSize<CBeamFlare>(iBeamFlareCount);
	CPoolMgr::GetInstance()->SetPoolSize<CBodyEmit>(iBodyEmitCount);
	CPoolMgr::GetInstance()->SetPoolSize<CHitUI>(iHitUICount);
	return S_OK;
}

HRESULT CMainApp::SetNextScene()
{
	SCENE_TYPE nextSceneType = SCENE_TYPE((_int)m_eCurSceneType + 1);
	if (nextSceneType == SCENE_END) return E_FAIL;
	CScene* nextScene = nullptr;

	switch (nextSceneType)
	{
	case CMainApp::SCENE_NONE: return E_FAIL;
	case CMainApp::SCENE_MENU:
		break;
	case CMainApp::SCENE_TUTORIAL:
		break;
	case CMainApp::SCENE_BATTLE:
		nextScene = CMapStage::Create(m_pGraphicDev);
		break;
	case CMainApp::SCENE_BOSS:
		nextScene = CBossStage::Create(m_pGraphicDev);
		break;
	default:
		return E_FAIL;
	}
	//Event Mgr 구독 전체 초기화
	CEventMgr::GetInstance()->ClearAllSubscribe();
	if (FAILED(CManagement::GetInstance()->Set_Scene(nextScene)))
	{
		Safe_Release(nextScene);
		MSG_BOX("Next Scene Setting Failed");
		return E_FAIL;
	}
	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pMainApp = new CMainApp;

	if (FAILED(pMainApp->Ready_MainApp()))
	{
		Safe_Release(pMainApp);
		MSG_BOX("MainApp Create Failed");
		return nullptr;
	}

	return pMainApp;
}

void CMainApp::Free()
{
	Safe_Release(m_pGraphicDev);
	Safe_Release(m_pDeviceClass);

	CEventMgr::DestroyInstance();
	CMapLoader::DestroyInstance();
	

	CDInputMgr::DestroyInstance();
	CRenderer::DestroyInstance();

	CFrameMgr::DestroyInstance();
	CTimerMgr::DestroyInstance();
	CManagement::DestroyInstance();

	CPoolMgr::DestroyInstance();
	CBaseTexture::ReleaseMap();
	CProtoMgr::DestroyInstance();


	//TODO : 한번에 해제하도록 수정하기 
	
	CDataMgr<CWhiteMan>::DestroyInstance();
	CDataMgr<CBeamMon>::DestroyInstance();
	CDataMgr<CFlyMon>::DestroyInstance();
	CDataMgr<CBoss>::DestroyInstance();

	// Player
	CDataMgr<CLeftPart>::DestroyInstance();
	CDataMgr<CRightPart>::DestroyInstance();
	CDataMgr<CMiddlePart>::DestroyInstance();
	CDataMgr<CKatana>::DestroyInstance();

	// MayBe ?
	CEventMgr::DestroyInstance();
	CUIManager::DestroyInstance();
	CFontMgr::DestroyInstance();

	m_pDeviceClass->DestroyInstance();
}
