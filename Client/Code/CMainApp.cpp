#include "pch.h"
#include "CMainApp.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CPoolMgr.h"
#include "CMapStage.h"
#include "CBossTestStage.h"
#include "CMapLoader.h"

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

#include "CLeftPart.h"
#include "CRightPart.h"
#include "CMiddlePart.h"
#include "CKatana.h"
#include "CBackGround.h"


CMainApp::CMainApp() : m_pDeviceClass(nullptr), m_pGraphicDev(nullptr)
, m_pManagementClass(CManagement::GetInstance())
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
	m_pManagementClass->Update_Scene(fTimeDelta);
		
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

	return S_OK;
}

HRESULT CMainApp::Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev)
{
	//Engine::CScene* pInitScene = CTestStage::Create(pGraphicDev);
	Engine::CScene* pInitScene = CMapStage::Create(pGraphicDev);
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
	// 모든 맵 파일 경로 수집
	vector<wstring> vecMapFiles;
	vecMapFiles.push_back(L"../../Map/Tutorial.json");		// 현재 사용중인 파일
	//vecMapFiles.push_back(L"../../Map/TutorialStage.json");
	//vecMapFiles.push_back(L"../../Map/Stage01.json");
	//vecMapFiles.push_back(L"../../Map/BossStage.json");

	for (auto& wstrFile : vecMapFiles)
	{
		if (FAILED(CMapLoader::GetInstance()->Preload_AllMapData(wstrFile)))
		{
			MSG_BOX("Map Preload Failed");
			return E_FAIL;
		}
	}

	// 타입별 최대값 초기화
	_uint iMaxFloor(0), iMaxDynamicFloor(0),iMaxCeiling(0), iMaxDynamicCeiling(0), iMaxWall(0), iMaxDynamicWall(0);
	_uint iMaxObstacle(0);
	_uint iBulletCount = 30;
	_uint iBossBulletCount = 50;
	_uint iBossRocketCount = 20;
	_uint iWhiteManCount = 5;
	_uint iBeamMonCount = 3;
	_uint iFlyMonCount = 6;

	for (auto& wstrFile : vecMapFiles)
	{
		// 각 파일에서 타입별 최대 개수 추출 
		iMaxFloor = max(iMaxFloor,
			CMapLoader::GetInstance()->Get_MaxObjectCount(wstrFile, "Floor"));
		iMaxDynamicFloor = max(iMaxDynamicFloor,
			CMapLoader::GetInstance()->Get_MaxObjectCount(wstrFile, "DynamicFloor"));

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
	}

	// 풀 크기 설정 
	CPoolMgr::GetInstance()->SetPoolSize<CFloor>(iMaxFloor);
	CPoolMgr::GetInstance()->SetPoolSize<CDynamicFloor>(iMaxDynamicFloor);

	CPoolMgr::GetInstance()->SetPoolSize<CCeiling>(iMaxCeiling);
	CPoolMgr::GetInstance()->SetPoolSize<CDynamicCeiling>(iMaxDynamicCeiling);

	CPoolMgr::GetInstance()->SetPoolSize<CWall>(iMaxWall);
	CPoolMgr::GetInstance()->SetPoolSize<CDynamicWall>(iMaxDynamicWall);

	CPoolMgr::GetInstance()->SetPoolSize<CObstacle>(iMaxObstacle);

	CPoolMgr::GetInstance()->SetPoolSize<CBullet>(iBulletCount);
	CPoolMgr::GetInstance()->SetPoolSize<CBossBullet>(iBossBulletCount);
	CPoolMgr::GetInstance()->SetPoolSize<CBossBullet>(iBossRocketCount);
	CPoolMgr::GetInstance()->SetPoolSize<CWhiteMan>(iWhiteManCount);
	CPoolMgr::GetInstance()->SetPoolSize<CBeamMon>(iBeamMonCount);
	CPoolMgr::GetInstance()->SetPoolSize<CFlyMon>(iFlyMonCount);

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


	m_pDeviceClass->DestroyInstance();
}
