#pragma once
#include "CBase.h"
#include "Engine_Define.h"

struct IGraphBuilder;
struct IMediaControl;
struct IMediaEvent;
struct IVMRWindowlessControl9;
struct IBaseFilter;
struct ICaptureGraphBuilder2;

BEGIN(Engine)

class ENGINE_DLL CVideoMgr : public CBase
{
    DECLARE_SINGLETON(CVideoMgr)

private:
    CVideoMgr() {}
    ~CVideoMgr() { Free(); }

public:
    HRESULT ReadyVideo(HWND hwnd, const TCHAR* filename);
    void Play();
    bool IsFinished();
    void Cleanup();
    bool IsPlaying() { return m_bIsPlaying; }
    void SetPlayFlag(bool _isPlaying) { m_bIsPlaying = _isPlaying; }
protected:
    void Free() override;

private:
    bool m_bIsPlaying = false;
    IGraphBuilder* pGraph = nullptr;                // 필터 그래프 매니저
    IMediaControl* pControl = nullptr;              // 재생/정지 제어
    IMediaEvent* pEvent = nullptr;                  // 이벤트(재생 끝남 등) 확인
    IVMRWindowlessControl9* pWindowless = nullptr;  // 윈도우 크기/위치 제어

};

END