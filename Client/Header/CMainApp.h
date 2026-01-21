#pragma once

#include "CBase.h"
#include "CGraphicDev.h"
#include "CTimerMgr.h"
#include "CFrameMgr.h"
#include "CManagement.h"

class CMainApp : public CBase
{
public:
    enum SCENE_TYPE { SCENE_NONE, SCENE_MENU, SCENE_TUTORIAL, SCENE_BATTLE, SCENE_BOSS, SCENE_END };

private:
    explicit CMainApp();
    virtual ~CMainApp();

public:
    HRESULT			        Ready_MainApp();
    int				        Update_MainApp(const float& fTimeDelta);
    void			        LateUpdate_MainApp(const float& fTimeDelta);
    void			        Render_MainApp();

private:
    HRESULT		            Ready_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev);
    HRESULT                 Ready_DefaultProto();
    HRESULT		            Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev);
    HRESULT                 Ready_ObjectPool();

private:
    HRESULT                 SetNextScene();
private:
    LPDIRECT3DDEVICE9		m_pGraphicDev;
    CGraphicDev*            m_pDeviceClass;
    CManagement*            m_pManagementClass;

public:
    static CMainApp*        Create();

private:
    virtual void		    Free();

private:
    SCENE_TYPE              m_eCurSceneType;

};
