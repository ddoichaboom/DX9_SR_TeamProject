#include "pch.h"
#include "CMainApp.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CPoolMgr.h"
#include "CTestStage.h"
#include "CMapStage.h"
#include "CMapLoader.h"

#include <ctime>

//TODO : 맵로더 구현되면 제거하기. poolSize 세팅 목적  
#include "CBullet.h"
#include "CWhiteMan.h"
#include "CTerrain.h"
#include "CPlayer.h"

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
	m_pDeviceClass->Render_Begin(D3DXCOLOR(1.f,1.f, 1.f, 1.f));
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

	//TODO : 맵로더 구현되면 제거하기
	CPoolMgr::GetInstance()->SetPoolSize<CBullet>(30);
	CPoolMgr::GetInstance()->SetPoolSize<CWhiteMan>(5);
	CPoolMgr::GetInstance()->SetPoolSize<CTerrain>(3);

	return S_OK;
}

HRESULT CMainApp::Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev)
{
	//Engine::CScene* pInitScene = CTestStage::Create(pGraphicDev);
	Engine::CScene* pInitScene = CMapStage::Create(pGraphicDev);

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
	CProtoMgr::DestroyInstance();
	CFrameMgr::DestroyInstance();
	CTimerMgr::DestroyInstance();
	CManagement::DestroyInstance();
	//TODO : 한번에 해제하도록 수정하기 
	CDataMgr<CPlayer>::DestroyInstance();
	CDataMgr<CWhiteMan>::DestroyInstance();

	CPoolMgr::DestroyInstance();
	m_pDeviceClass->DestroyInstance();
}
