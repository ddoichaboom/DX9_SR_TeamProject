#pragma once

#include "CBase.h"
#include "CGraphicDev.h"
#include "CTimerMgr.h"
#include "CFrameMgr.h"
#include "CManagement.h"

class CMainApp : public CBase
{
private:
    explicit CMainApp();
    virtual ~CMainApp();

public:
    HRESULT			Ready_MainApp();
    int				Update_MainApp(const float& fTimeDelta);
    void			LateUpdate_MainApp(const float& fTimeDelta);
    void			Render_MainApp();

private:
    HRESULT		Ready_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev);
    HRESULT		Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    LPDIRECT3DDEVICE9		m_pGraphicDev;
    CGraphicDev* m_pDeviceClass;
    CManagement* m_pManagementClass;

public:
    static CMainApp* Create();

private:
    virtual void		Free();

};
