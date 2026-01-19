#include "pch.h"
#include "CBaseUI.h"

CBaseUI::CBaseUI(LPDIRECT3DDEVICE9 pGraphicDev)
    :   CGameObject(pGraphicDev)
    ,   m_fX(0.f), m_fY(0.f), m_fSizeX(0.f), m_fSizeY(0.f)
{
}

CBaseUI::CBaseUI(const CBaseUI& rhs)
    :   CGameObject(rhs)
    ,   m_fX(rhs.m_fX), m_fY(rhs.m_fY), m_fSizeX(rhs.m_fSizeX), m_fSizeY(rhs.m_fSizeY)
{
}

CBaseUI::~CBaseUI()
{
}

HRESULT CBaseUI::Ready_GameObject()
{
    return E_FAIL;
}

_int CBaseUI::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    return iExit;
}

void CBaseUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

HRESULT CBaseUI::Add_Component()
{
    return S_OK;
}

void CBaseUI::Free()
{
    CGameObject::Free();
}

_bool CBaseUI::MousePicking()
{
    RECT    rcUI =
    {
        m_fX - m_fSizeX * 0.5f,
        m_fY - m_fSizeY * 0.5f,
        m_fX + m_fSizeX * 0.5f,
        m_fY + m_fSizeY * 0.5f
    };

    POINT   ptMouse = {};

    GetCursorPos(&ptMouse);
    ScreenToClient(g_hWnd, &ptMouse);

    return PtInRect(&rcUI, ptMouse);
}
