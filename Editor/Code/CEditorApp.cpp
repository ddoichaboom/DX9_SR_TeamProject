#include "pch.h"
#include "CEditorApp.h"

#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CTransform.h"
#include "CRcTex.h"
#include "CRcTexUp.h"
#include "CTexture.h"
#include "CCubeTexture.h"

#include "CEditorScene.h"
#include "CMainMenuBar.h"
#include "CToolBar.h"
#include "CHierarchy.h"
#include "CInspector.h"
#include <CFileIO.h>

#include "CEditorFloor.h"
#include "CEditorCeiling.h"
#include "CEditorWall.h"
#include "CEditorDynamicFloor.h"
#include "CEditorSlopeFloor.h"
#include "CEditorDynamicWall.h"
#include "CEditorDoor.h"
#include "CEditorInteractObject.h"
#include "CEditorDisplayObject.h"
#include "CEditorDisplayCubeObject.h"
#include "CEditorWindow.h"

#include "CEffectScene.h"
#include "CEffectToolBar.h"
#include "CBlood.h"
#include "CTrail.h"
#include "CFlare.h"
#include "CExplosion.h"
#include "CBeamFlare.h"
#include "CBodyEmit.h"
#include "CHitUI.h"
#include "CTakeDownBlood.h"
#include "CToonFlash.h"
#include "CToonFog.h"
#include "CBossTrail.h"
#include "CSodaUI.h"
#include "CBossHPUI.h"

CEditorApp::CEditorApp()
    : m_pGraphicDev(nullptr)
    , m_pDeviceClass(nullptr)
    , m_pManagementClass(Engine::CManagement::GetInstance())
    , m_bShowDemo(true)
    , m_vClearColor(0.45f, 0.55f, 0.60f, 1.00f)
    , m_pCurrentScene(nullptr)
    , m_pMainMenuBar(nullptr)
    , m_pToolBar(nullptr)
    , m_pHierarchy(nullptr)
    , m_pInspector(nullptr)
{
}

CEditorApp::~CEditorApp()
{
}

HRESULT CEditorApp::Ready_Editor()
{
    // 1. Engine 초기화 (CMainApp::Ready_MainApp 참고)
    if (FAILED(Ready_DefaultSetting(&m_pGraphicDev, g_hWnd, g_hInst)))
        return E_FAIL;

    // 2. ImGui 초기화 (Editor 전용)
    if (FAILED(Ready_ImGui(g_hWnd)))
        return E_FAIL;

    // 3. Prototype 등록
    if (FAILED(Ready_Prototype(m_pGraphicDev)))
        return E_FAIL;

    // 4. Scene 생성
    if (FAILED(Ready_Scene()))
        return E_FAIL;

    return S_OK;
}

void CEditorApp::Update_Editor(const _float& fTimeDelta)
{
    Engine::CDInputMgr::GetInstance()->Update_InputDev();

    // Scene 업데이트 
    if (m_pCurrentScene)
        m_pCurrentScene->Update_Scene(fTimeDelta);
}

void CEditorApp::LateUpdate_Editor(const _float& fTimeDelta)
{
    if (m_pCurrentScene)
        m_pCurrentScene->LateUpdate_Scene(fTimeDelta);
}

void CEditorApp::Render_Editor()
{
    // 1. ImGui 프레임 시작
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // 2. DockSpace 설정 (Unity/Unreal 스타일 전체 화면 도킹)
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    // 전체 화면 크기의 보이지 않는 윈도우 생성
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    // 윈도우 플래그 (타이틀바, 리사이즈 등 없음)
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    window_flags |= ImGuiWindowFlags_NoBackground;  // 배경 투명

    // 패딩 제거 (전체 화면 활용)
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    // 보이지 않는 DockSpace 윈도우
    ImGui::Begin("DockSpaceWindow", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    // DockSpace 생성
    ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::End();

    //3. UI 렌더링(순서 중요)
    if (m_pMainMenuBar)
        m_pMainMenuBar->Render_MenuBar();

    if (m_pToolBar)
        m_pToolBar->Render_ToolBar();

    if (m_pHierarchy)
        m_pHierarchy->Render_Hierarchy();

    if (m_pInspector)
        m_pInspector->Render_Inspector();

    if (m_pEffectToolBar)
        m_pEffectToolBar->Render_ToolBar();

    //// ImGui Demo Window (테스트용)
    //if (m_bShowDemo)
    //    ImGui::ShowDemoWindow(&m_bShowDemo);

    // 4. 렌더링 시작 (CMainApp::Render_MainApp 참고)
    ImGui::EndFrame();
    m_pDeviceClass->Render_Begin(D3DXCOLOR(m_vClearColor.x, m_vClearColor.y,
        m_vClearColor.z, m_vClearColor.w));

    // 5. Scene 렌더 (CRenderer 사용)
    if (m_pCurrentScene)
    {
        // Scene 자체 렌더링 (Grid, Camera 기즈모 등)
        m_pCurrentScene->Render_Scene();

        // CRenderer를 통한 GameObject 렌더링 (Client와 동일)
        Engine::CRenderer::GetInstance()->Render_GameObject(m_pGraphicDev);
    }

    // 6. ImGui 렌더
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

    // 7. 렌더링 종료
    m_pDeviceClass->Render_End();
}

HRESULT CEditorApp::Ready_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev, HWND hWnd, HINSTANCE hInst)
{
    // CMainApp::Ready_DefaultSetting과 거의 동일

    // 1. GraphicDev 초기화
    if (FAILED(Engine::CGraphicDev::GetInstance()->Ready_GraphicDev(
        hWnd, MODE_WIN, WINCX, WINCY, &m_pDeviceClass)))
    {
        MSG_BOX("GraphicDev Ready Failed");
        return E_FAIL;
    }

    m_pDeviceClass->AddRef();

    (*ppGraphicDev) = m_pDeviceClass->Get_GraphicDev();
    (*ppGraphicDev)->AddRef();

    // 2. 렌더 스테이트 설정
    (*ppGraphicDev)->SetRenderState(D3DRS_LIGHTING, FALSE);

    // Editor에서는 양면 렌더링 (오브젝트를 모든 각도에서 볼 수 있어야 함)
    (*ppGraphicDev)->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);


    // 3. InputDev 초기화
    if (FAILED(Engine::CDInputMgr::GetInstance()->Ready_InputDev(hInst, hWnd)))
    {
        MSG_BOX("InputDev Ready Failed");
        return E_FAIL;
    }

    // 4. 샘플러 스테이트 설정
    (*ppGraphicDev)->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    (*ppGraphicDev)->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

    return S_OK;
}

HRESULT CEditorApp::Ready_ImGui(HWND hWnd)
{
    // ImGui 컨텍스트 생성
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    // 도킹 활성화 
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // For multi-monitor support

    // ImGui 스타일 설정
    ImGui::StyleColorsDark();

    // ImGui 백엔드 초기화
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX9_Init(m_pGraphicDev);

    return S_OK;
}

HRESULT CEditorApp::Ready_Prototype(LPDIRECT3DDEVICE9 pGraphicDev)
{
    // Component Prototype 등록 (나중에 확장)

    // Transform
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(
        L"Proto_Transform", Engine::CTransform::Create(pGraphicDev))))
    {
        MSG_BOX("Proto_Transform Create Failed");
        return E_FAIL;
    }

    // RcTex
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(
        L"Proto_RcTex", Engine::CRcTex::Create(pGraphicDev))))
    {
        MSG_BOX("Proto_RcTex Create Failed");
        return E_FAIL;
    }

    // RcTex Up 
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(
        L"Proto_RcTexUp", Engine::CRcTexUp::Create(pGraphicDev))))
    {
        MSG_BOX("Proto_RcTexUp Create Failed");
        return E_FAIL;
    }

    // CubeTex
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(
        L"Proto_CubeTex", Engine::CCubeTex::Create(pGraphicDev))))
    {
        MSG_BOX("Proto_CubeTex Create Failed");
        return E_FAIL;
    }

    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(
        L"Proto_Collision", Engine::CCollision::Create(m_pGraphicDev))))
    {
        MSG_BOX("Proto_Collision Create Failed");
        return E_FAIL;
    }

    if (FAILED(Ready_Texture_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CEditorApp::Ready_Scene()
{
    //// EditorScene 생성
    m_pCurrentScene = CEditorScene::Create(m_pGraphicDev);
    //CEffectScene* scene;
    //m_pCurrentScene = scene = CEffectScene::Create(m_pGraphicDev);

    if (nullptr == m_pCurrentScene)
    {
        MSG_BOX("EditorScene Create Failed");
        return E_FAIL;
    }

    // 추가 UI 컴포넌트 생성

    // 1. Main Menu Bar
    m_pMainMenuBar = CMainMenuBar::Create();
    if (nullptr == m_pMainMenuBar)
    {
        MSG_BOX("MainMenuBar Create Failed");
        return E_FAIL;
    }

    m_pMainMenuBar->Set_Scene(m_pCurrentScene);

    // 2. Tool Bar
    m_pToolBar = CToolBar::Create(m_pCurrentScene->Get_Camera(), m_pCurrentScene->Get_Grid());
    if (nullptr == m_pToolBar)
    {
        MSG_BOX("ToolBar Create Failed");
        return E_FAIL;
    }

    m_pCurrentScene->Set_ToolBar(m_pToolBar);

    // 4. Hierarchy
    m_pHierarchy = CHierarchy::Create(m_pCurrentScene);
    if (nullptr == m_pHierarchy)
    {
        MSG_BOX("Hierarchy Create Failed");
        return E_FAIL;
    }

    m_pCurrentScene->Set_Hierarchy(m_pHierarchy);

    // 5. Inspector
    m_pInspector = CInspector::Create(m_pCurrentScene->Get_Camera(), m_pCurrentScene);
    if (nullptr == m_pInspector)
    {
        MSG_BOX("Inspector Create Failed");
        return E_FAIL;
    }

    //m_pEffectToolBar = CEffectToolBar::Create(scene);
    //if (nullptr == m_pEffectToolBar)
    //{
    //    MSG_BOX("EffectToolBar Create Failed");
    //    return E_FAIL;
    //}


    return S_OK;
}

HRESULT CEditorApp::Ready_Texture_Prototype()
{
    Engine::CTexture* pCom_Texture = nullptr;

    // Floor 텍스처  
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CEditorFloor::GetTextureSources());
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Static_FloorTexture", pCom_Texture)))
    {
        MSG_BOX("Proto_Static_FloorTexture Ready Failed");
        return E_FAIL;
    }

    // Slope Floor 텍스처  
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CEditorSlopeFloor::GetTextureSources());
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Slope_FloorTexture", pCom_Texture)))
    {
        MSG_BOX("Proto_Slope_FloorTexture Ready Failed");
        return E_FAIL;
    }

    // DynamicFloor Texture
    pCom_Texture = Engine::CScrollTexture::Create(m_pGraphicDev, CEditorDynamicFloor::GetTextureSources(), 0.2f);
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Dynamic_FloorTexture", pCom_Texture)))
    {
        MSG_BOX("Proto_Dynamic_FloorTexture Ready Failed");
        return E_FAIL;
    }

    //  Ceiling 텍스처  
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CEditorCeiling::GetTextureSources());
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Static_CeilingTexture", pCom_Texture)))
    {
        MSG_BOX("Proto_Static_CeilingTexture Ready Failed");
        return E_FAIL;
    }

    //  Wall 텍스처  
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CEditorWall::GetTextureSources());
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Static_WallTexture", pCom_Texture)))
    {
        MSG_BOX("Proto_Static_WallTexture Ready Failed");
        return E_FAIL;
    }

    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CEditorDynamicWall::GetTextureSources());
    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Dynamic_WallTexture", pCom_Texture)))
        return E_FAIL;

    if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_WallAnimation",
        Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CEditorDynamicWall::GetAnimSources()))))
        return E_FAIL;

    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CEditorDoor::GetTextureSources());
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Door_Texture", pCom_Texture)))
    {
        MSG_BOX("Proto_Static_WallTexture Ready Failed");
        return E_FAIL;
    }


    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CEditorInteractObject::GetTextureSources());
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_InteractObject_Texture", pCom_Texture)))
    {
        MSG_BOX("Proto_InteractObject_Texture Ready Failed");
        return E_FAIL;
    }

    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CEditorDisplayObject::GetTextureSources());
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_DisplayObject_Texture", pCom_Texture)))
    {
        MSG_BOX("Proto_DisplayObject_Texture Ready Failed");
        return E_FAIL;
    }

    Engine::CCubeTexture* pCom_Cube_Texture = nullptr;

    pCom_Cube_Texture = Engine::CCubeTexture::Create(m_pGraphicDev, CEditorDisplayCubeObject::GetTextureSources());
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_DisplayCubeObject_Texture", pCom_Cube_Texture)))
    {
        MSG_BOX("Proto_DisplayCubeObject_Texture Ready Failed");
        return E_FAIL;
    }

    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CEditorWindow::GetTextureSources());
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_WindowTexture", pCom_Texture)))
    {
        MSG_BOX("Proto_WindowTexture Ready Failed");
        return E_FAIL;
    }

    //Blood Texture
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CBlood::GetTextureSources());
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_Blood_Texture", pCom_Texture)))
    {
        MSG_BOX("Proto Blood Ready Failed");
        return E_FAIL;
    }

    //Trail Texture
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CTrail::GetTextureSource());
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_Trail_Texture", pCom_Texture)))
    {
        MSG_BOX("Proto Trail Ready Failed");
        return E_FAIL;
    }

    //Flare Texture
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CFlare::GetTextureSource());
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_Flare_Texture", pCom_Texture)))
    {
        MSG_BOX("Proto Flare Ready Failed");
        return E_FAIL;
    }

    //Explore Texture
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CExplosion::GetTextureSources());
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_EXP_Texture", pCom_Texture)))
    {
        MSG_BOX("Proto Explosion Ready Failed");
        return E_FAIL;

    }

    //BeamFlare Texture
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CBeamFlare::GetTextureSource());
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_BeamFlare_Texture", pCom_Texture)))
    {
        MSG_BOX("Proto BeamFlare Ready Failed");
        return E_FAIL;
    }

    //BodyEmit Texture
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CBodyEmit::GetTextureSources());
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_BodyEmit_Texture", pCom_Texture)))
    {
        MSG_BOX("Proto BodyEmit Ready Failed");
        return E_FAIL;
    }

    //HitUI Texture
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CHitUI::GetTextureSource());
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_HitUI_Texture", pCom_Texture)))
    {
        MSG_BOX("Proto HitUI Ready Failed");
        return E_FAIL;
    }

    //BossTrail Texture
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CBossTrail::GetTextureSource());
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_BossTrail_Texture", pCom_Texture)))
    {
        MSG_BOX("Proto BossTrail Ready Failed");
        return E_FAIL;
    }

    //TakeDownBlood Texture
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CTakeDownBlood::GetTextureSource());
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_TakeDownBlood_Texture", pCom_Texture)))
    {
        MSG_BOX("Proto TakeDownBlood Ready Failed");
        return E_FAIL;
    }

    //ToonFlash Texture
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CToonFlash::GetTextureSource());
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_ToonFlash_Texture", pCom_Texture)))
    {
        MSG_BOX("Proto ToonFlash Ready Failed");
        return E_FAIL;
    }

    //ToonFog Texture
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CToonFog::GetTextureSource());
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_ToonFog_Texture", pCom_Texture)))
    {
        MSG_BOX("Proto ToonFog Ready Failed");
        return E_FAIL;
    }

    //SodaUI Texture
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CSodaUI::GetTextureSource());
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_SodaUI_Texture", pCom_Texture)))
    {
        MSG_BOX("Proto SodaUI Ready Failed");
        return E_FAIL;
    }

    //SodaUI Texture
    pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CBossHPUI::GetTextureSource());
    if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_BOSSHPUI_Texture", pCom_Texture)))
    {
        MSG_BOX("Proto BOSSHPUI Ready Failed");
        return E_FAIL;
    }

    return S_OK;
}

CEditorApp* CEditorApp::Create(HWND hWnd, HINSTANCE hInst)
{
    CEditorApp* pInstance = new CEditorApp;

    if (FAILED(pInstance->Ready_Editor()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorApp Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CEditorApp::Free()
{
    // 먼저 CRenderer의 render group 정리 (매우 중요!)
    // Scene의 객체들이 삭제되기 전에 CRenderer의 참조를 먼저 해제해야 함
    if (Engine::CRenderer::GetInstance())
        Engine::CRenderer::GetInstance()->Clear_RenderGroup();

    // 2. Scene 정리
    Safe_Release(m_pCurrentScene);

    // 3. UI 컴포넌트 정리
    Safe_Release(m_pEffectToolBar);
    Safe_Release(m_pInspector);
    Safe_Release(m_pHierarchy);
    Safe_Release(m_pToolBar);
    Safe_Release(m_pMainMenuBar);

    // 4. ImGui 정리
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    // 5. Engine 정리
    Safe_Release(m_pGraphicDev);
    Safe_Release(m_pDeviceClass);

    CFileIO::DestroyInstance();
    
    // 6. 싱글톤 정리
    Engine::CDInputMgr::DestroyInstance();
    Engine::CRenderer::DestroyInstance();
    Engine::CProtoMgr::DestroyInstance();
    Engine::CManagement::DestroyInstance();
    Engine::CGraphicDev::DestroyInstance();

}