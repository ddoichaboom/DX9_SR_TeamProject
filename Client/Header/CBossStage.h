#pragma once
#include "CStage.h"
#include "CEventMgr.h"

class CLoading;

class CBossStage : public CStage, public IListener
{
protected:
    explicit CBossStage(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CBossStage();

public:
    HRESULT     Ready_Scene() override;
    _int        Update_Scene(const _float& fTimeDelta) override;
    void        LateUpdate_Scene(const _float& fTimeDelta) override;
    void        Render_Scene() override;

public:
    static CBossStage* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
    HRESULT     Ready_Environment_Layer(const _tchar* pLayerTag) override;
    HRESULT     Ready_GameLogic_Layer(const _tchar* pLayerTag) override;

    //스레드 전달 함수
    HRESULT     Ready_Prototype() override;

    HRESULT     Remove_PrevObjectPool() override;

    HRESULT     Ready_ObjectPool_Character()    override;
    HRESULT     Ready_ObjectPool_Terrain()      override;
    HRESULT     Ready_ObjectPool_UI()           override;
    HRESULT     Ready_ObjectPool_Effect()       override;


    HRESULT     Ready_CharacterTextureProto()   override;
    HRESULT     Ready_TerrainTextureProto()     override;
    HRESULT     Ready_UITextureProto()          override;
    HRESULT     Ready_EffectTextureProto()      override;

    void        Check_Collision() override;

    //메세지 Mgr에 구독한 이벤트에 대한 기능 정의 
    void        OnEvent(EVENT_TYPE _type, EventData* _pData) override;


private:
    virtual void Free();

private:
    wstring m_BossVideoName = L"EnterBoss.wmv";
    wstring m_BossSoundName = L"EnterBossSound.wav";
};

