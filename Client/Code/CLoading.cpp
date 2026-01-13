#include "pch.h"
#include "CLoading.h"
#include "CProtoMgr.h"

CRITICAL_SECTION	CLoading::m_Crt_Base;
CRITICAL_SECTION	CLoading::m_Crt_Texture;

CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphicDev)
    : m_pGraphicDev(pGraphicDev)
{
    ZeroMemory(m_szLoading, sizeof(m_szLoading));
    m_pGraphicDev->AddRef();
}

CLoading::~CLoading()
{

}



HRESULT CLoading::Ready_Loading(function<void()> _protoBaseFunc, function<void()> _protoTextureFunc,
    function<void()> _objectPoolFunc, function<void()> _readyEnvFunc, function<void()> _readyGameFunc)
{
    InitializeCriticalSection(&m_Crt_Base);
    InitializeCriticalSection(&m_Crt_Texture);

    m_ProtoBaseFunc = _protoBaseFunc;
    m_ProtoTextureFunc = _protoTextureFunc;
    m_ObjectPoolFunc = _objectPoolFunc;
    m_ReadyEnvFunc = _readyEnvFunc;
    m_ReadyGameFunc = _readyGameFunc;

    //텍스쳐 제외 컴포넌트 
    m_hThread[0] = (HANDLE)_beginthreadex(NULL,    // 보안속성(핸들의 상속 여부, null인 경우 상속에서 제외)
                                        0,      // 디폴트 스택 사이즈(1 바이트)
                                        Thread_Proto_Base,      // 쓰레드 함수
                                        &m_ProtoBaseFunc,   // 쓰레드 함수를 이용하여 가공할 데이터 주소    
                                        0,      // 쓰레드 생성 및 실행을 조정하기 옵션(flag)
                                        NULL);  // 쓰레드 id

    //텍스쳐(+애니) 컴포넌트
    m_hThread[1] = (HANDLE)_beginthreadex(NULL,
                                          0,
                                          Thread_Proto_Texture,
                                          &m_ProtoTextureFunc,
                                          0,
                                          NULL);

    return S_OK;
}

bool CLoading::Update_Loading()
{
    if (m_bEnd) return true;

    DWORD Result = WaitForMultipleObjects(2, m_hThread, TRUE, 0);

    m_fPercent = 50.f;

    //아직 0,1번 스레드가 끝나지않았다면  
    if (Result == WAIT_TIMEOUT) return false;
    else if (Result == WAIT_FAILED) return true; 
    
    //끝났다면
    if (m_hThread[0])
    {
        CloseHandle(m_hThread[0]);
        m_hThread[0] = NULL;
    }
    if (m_hThread[1])
    {
        CloseHandle(m_hThread[1]);
        m_hThread[1] = NULL;
    }

    m_ObjectPoolFunc();
    m_fPercent = 70.f;

    m_ReadyEnvFunc();
    m_fPercent = 90.f;
    m_ReadyGameFunc();
    m_fPercent = 100.f;

    m_bEnd = true;
    return m_bEnd;

}

unsigned int CLoading::Thread_Proto_Base(void* pArg)
{
    _uint iFlag(0);
    if (!pArg) return iFlag;
    
    EnterCriticalSection(&m_Crt_Base);

    auto* pFunc = (function<void()>*)pArg;
    if(pFunc) (*pFunc)();

    LeaveCriticalSection(&m_Crt_Base);
    return iFlag;

}

unsigned int CLoading::Thread_Proto_Texture(void* pArg)
{
    _uint iFlag(0);
    if (!pArg) return iFlag;

    EnterCriticalSection(&m_Crt_Texture);

   auto* pFunc = (function<void()>*)pArg;
    if (pFunc)  (*pFunc)();

    LeaveCriticalSection(&m_Crt_Texture);
    return iFlag;
}

CLoading* CLoading::Create(LPDIRECT3DDEVICE9 pGraphicDev, function<void()> _protoBaseFunc, function<void()> _protoTextureFunc, function<void()> _objectPoolFunc
    , function<void()> _readyEnvFunc, function<void()> _readyGameFunc)
{
    CLoading* pLoading = new CLoading(pGraphicDev);

    if (FAILED(pLoading->Ready_Loading(_protoBaseFunc, _protoTextureFunc, _objectPoolFunc, _readyEnvFunc , _readyGameFunc)))
    {
        Safe_Release(pLoading);
        MSG_BOX("Loading Create Failed");
        return nullptr;
    }

    return pLoading;
}

void CLoading::Free()
{
    DeleteCriticalSection(&m_Crt_Base);
    DeleteCriticalSection(&m_Crt_Texture);

    Safe_Release(m_pGraphicDev);
}

