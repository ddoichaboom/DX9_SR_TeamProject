#pragma once
#include "Engine_Define.h"
class CRenderStateGuard
{
private:
    IDirect3DDevice9* m_pGraphicDev;
    _matrix    m_matOldView, m_matOldProj;
    DWORD       m_dwOldZE, m_dwOldZW, m_dwOldAlpha;

public:
    explicit CRenderStateGuard(const CRenderStateGuard& rhs) = delete;
    CRenderStateGuard& operator =(const CRenderStateGuard&) = delete;

    explicit CRenderStateGuard(IDirect3DDevice9* pGraphicDev)
        : m_pGraphicDev(pGraphicDev)
    {
        m_pGraphicDev->AddRef();
        m_pGraphicDev->GetTransform(D3DTS_VIEW, &m_matOldView);
        m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &m_matOldProj);
        m_pGraphicDev->GetRenderState(D3DRS_ZENABLE, &m_dwOldZE);
        m_pGraphicDev->GetRenderState(D3DRS_ZWRITEENABLE, &m_dwOldZW);
        m_pGraphicDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &m_dwOldAlpha);
    }


    ~CRenderStateGuard()
    {
        m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matOldView);
        m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matOldProj);
        m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, m_dwOldZE);
        m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, m_dwOldZW);
        m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, m_dwOldAlpha);
        Safe_Release(m_pGraphicDev);
    }
};