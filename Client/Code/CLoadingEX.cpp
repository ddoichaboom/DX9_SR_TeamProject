#include "pch.h"
#include "CLoadingEX.h"
#include "CProtoMgr.h"

CLoadingEX::CLoadingEX(LPDIRECT3DDEVICE9 pGraphicDev)
    :m_pGraphicDev(pGraphicDev), m_fPercent(0.f)
    , m_iCurTaskCount(0), m_iEndTaskCount(0), m_eCurLevel(LEVEL_END)
   
{
    m_pGraphicDev->AddRef();
}

CLoadingEX::~CLoadingEX()
{
}

HRESULT CLoadingEX::Ready_Loading()
{
    //Red Loading Bar
    //m_pLoadingBar = CLoadingBar::Create(m_pGraphicDev, { 1,0,0,1 });
    //m_pLoadingBar->SetPercent(0.f);
    return S_OK;
}

void CLoadingEX::Update_Loading(const _float& fTimeDelta)
{
  //  if(m_pLoadingBar) m_pLoadingBar->Update_GameObject(fTimeDelta);
    if (m_bEnd) return;

    if (m_eCurLevel == LEVEL_END || m_iCurTaskCount == 0)
    {
        if (m_eCurLevel == LEVEL_END) m_eCurLevel = Lv1_INIT;
        else  m_eCurLevel = (ELoadingLevel)((_int)m_eCurLevel + 1);

        if (m_eCurLevel == LEVEL_END)
        {
            m_bEnd = true;
       //     m_pLoadingBar->SetPercent(1.f);
            return;
        }

        for (int i = 0; i < (_int)m_vecTasks[m_eCurLevel].size(); i++)
        {
            m_hThread[i] = (HANDLE)_beginthreadex(NULL, 0, Thread_Func, &m_vecTasks[m_eCurLevel][i], 0, NULL);
            m_iCurTaskCount++;
        }
        return;
    }

    _int result = WaitForMultipleObjects(m_iCurTaskCount, m_hThread, TRUE, 0);
    
    //스레드가 모두 완료됐다면 
    if (result == WAIT_OBJECT_0)
    {
        m_fCurGauge += m_iCurTaskCount;
        m_fPercent = m_fCurGauge / m_fTotalGauge;
       // m_pLoadingBar->SetPercent(m_fPercent);

        //스레드 초기화 
        for (int i = 0; i < m_iCurTaskCount; i++)
        {
            if (m_hThread[i])
            {
                CloseHandle(m_hThread[i]);
                m_hThread[i] = NULL;
            }
        }
        //다음 레벨로 전환
        m_iCurTaskCount = 0;
        m_eCurLevel = (ELoadingLevel)((_int)m_eCurLevel + 1);
       
        //모든 레벨 완료
        if (m_eCurLevel == LEVEL_END)
        {
            m_bEnd = true;
            return;
        }

        for (int i = 0; i < (_int)m_vecTasks[m_eCurLevel].size(); i++)
        {
            m_hThread[i] = (HANDLE)_beginthreadex(NULL, 0, Thread_Func, &m_vecTasks[m_eCurLevel][i], 0, NULL);
            m_iCurTaskCount++;
        }
    }

    return;
}

void CLoadingEX::AddTask(_int level, Task _task)
{
    if (m_vecTasks.size() <= level)
    {
        m_vecTasks.resize(level + 1);
    }
    m_vecTasks[level].push_back(_task);
    m_fTotalGauge++;
}

bool CLoadingEX::IsEnd()
{
    //if (m_bEnd && m_pLoadingBar && m_pLoadingBar->IsBarEnd()) return true;
    //else return false;
    return m_bEnd;
}


unsigned int CLoadingEX::Thread_Func(void* pArg)
{
    ThreadArg* pThreadArg = (ThreadArg*)pArg;
    if (pThreadArg->task) pThreadArg->task();
    return 0;
}

CLoadingEX* CLoadingEX::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CLoadingEX* pLoading = new CLoadingEX(pGraphicDev);
    if (FAILED(pLoading->Ready_Loading()))
    {
        Safe_Release(pLoading);
        MSG_BOX("LoadingEX Create Failed");
        return nullptr;
    }
    return pLoading;
}

void CLoadingEX::Free()
{
    Safe_Release(m_pGraphicDev);
   // Safe_Release(m_pLoadingBar);
}

