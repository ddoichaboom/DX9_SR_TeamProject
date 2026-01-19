#pragma once
#include "CStage.h"
#include "CEventMgr.h"

class CLoading;
class CMapStage : public CStage, public IListener
{
protected:
    explicit CMapStage(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CMapStage();

public:
    HRESULT     Ready_Scene() override;
    _int        Update_Scene(const _float& fTimeDelta) override;
    void        LateUpdate_Scene(const _float& fTimeDelta) override;
    void        Render_Scene() override;

public:
    static CMapStage* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
    HRESULT     Ready_Environment_Layer(const _tchar* pLayerTag) override;
    HRESULT     Ready_GameLogic_Layer(const _tchar* pLayerTag) override;

    //스레드 전달 함수
    HRESULT     Ready_Prototype() override;
    HRESULT     Ready_Prototype_OnlyTexture() override;
    HRESULT     Ready_ObjectPool() override;
    //

    HRESULT     Ready_MonsterTextureProto();
    HRESULT     Ready_TerrainTextureProto();
    HRESULT     Ready_UITextureProto();
    HRESULT     Ready_EffectTextureProto();

    void        Check_Collision() override;

    //메세지 Mgr에 구독한 이벤트에 대한 기능 정의 
    void        OnEvent(EVENT_TYPE _type, EventData* _pData) override;


private:
    virtual void Free();

};

