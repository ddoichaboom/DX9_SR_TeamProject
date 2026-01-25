#include "CVideoMgr.h"
#include <dshow.h>
#include <vmr9.h>

#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "quartz.lib")


IMPLEMENT_SINGLETON(CVideoMgr)

HRESULT CVideoMgr::ReadyVideo(HWND hwnd, const TCHAR* filename)
{
    if (pGraph != nullptr) Cleanup();

    CoInitialize(NULL);
    HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
        IID_IGraphBuilder, (void**)&pGraph);

    if (FAILED(hr)) return E_FAIL;

    ICaptureGraphBuilder2* pBuilder = nullptr;
    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void**)&pBuilder);
    if (FAILED(hr)) return E_FAIL;

    pBuilder->SetFiltergraph(pGraph);

    IBaseFilter* pVmr = nullptr;
    hr = CoCreateInstance(CLSID_VideoMixingRenderer9, NULL, CLSCTX_INPROC_SERVER,
        IID_IBaseFilter, (void**)&pVmr);

    pGraph->AddFilter(pVmr, L"VMR9");

    IVMRFilterConfig9* pConfig = nullptr;
    pVmr->QueryInterface(IID_IVMRFilterConfig9, (void**)&pConfig);
    pConfig->SetRenderingMode(VMR9Mode_Windowless); //창모드 안씀 
    pConfig->Release();

    pVmr->QueryInterface(IID_IVMRWindowlessControl9, (void**)&pWindowless);
    pWindowless->SetVideoClippingWindow(hwnd);

    IBaseFilter* pSource = nullptr;

    static TCHAR szCurPath[128] = L"../Bin/Resource/Video/";
    TCHAR szFullPath[256] = L"";

    lstrcpy(szFullPath, szCurPath);
    lstrcat(szFullPath, filename); 

    hr = pGraph->AddSourceFilter(szFullPath, L"Source", &pSource);
    if (FAILED(hr))
    {
        pBuilder->Release();
        pVmr->Release();
        return E_FAIL;
    }

    //오디오는 사운드 매니저로 관리하고(싱크 목적) 영상만 랜더함 
    //핀 카테고리, 미디어 타입, 출발지, 경유지, 도착지
    hr = pBuilder->RenderStream(NULL, &MEDIATYPE_Video, pSource, NULL, pVmr);

    pSource->Release();
    pVmr->Release();
    pBuilder->Release();

    if (FAILED(hr)) return E_FAIL;

    pGraph->QueryInterface(IID_IMediaControl, (void**)&pControl);
    pGraph->QueryInterface(IID_IMediaEvent, (void**)&pEvent);

    return S_OK;
}
void CVideoMgr::Play()
{
    if (!pControl || !pWindowless) return;

    m_bIsPlaying = true;

    RECT destRect;
    destRect.left = 0;
    destRect.top = 0;
    destRect.right = WINCX;
    destRect.bottom = WINCY;

    pWindowless->SetVideoPosition(NULL, &destRect);
    pControl->Run(); // 재생 시작
}

bool CVideoMgr::IsFinished()
{
    if (!pEvent) return true;

    long eventCode;
    LONG_PTR param1, param2;

    if (pEvent->GetEvent(&eventCode, &param1, &param2, 0) == S_OK)
    {
        pEvent->FreeEventParams(eventCode, param1, param2);

        // 영상이 끝났다면(EC_COMPLETE) true 반환
        if (eventCode == EC_COMPLETE)
        {
            m_bIsPlaying = false;
            return true;
        }
    }
    return false;
}

void CVideoMgr::Cleanup()
{
    if (pControl)
    {
        pControl->Stop(); // 정지
        pControl->Release();
        pControl = nullptr;
    }

    // 역순 해제
    if (pWindowless)
    {
        pWindowless->Release(); 
        pWindowless = nullptr;
    }
    if (pEvent)
    {
        pEvent->Release(); 
        pEvent = nullptr;
    }
    if (pControl)
    {
        pControl->Release(); 
        pControl = nullptr;
    }
    if (pGraph)
    {
        pGraph->Release(); 
        pGraph = nullptr;
    }
    m_bIsPlaying = false;
    CoUninitialize();
}

void CVideoMgr::Free()
{
    Cleanup();
}
