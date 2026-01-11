#pragma once
#include "CStage.h"

class CMapStage : public CStage
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
    HRESULT     Ready_Prototype() override;

    // TODO:상위 클래스에 담아서 스테이지마다 다르게 해도 될듯함 - 호준
    HRESULT     Ready_ObjectPool();
    HRESULT     Ready_PlayerProto();
    HRESULT     Ready_MonsterProto();
    HRESULT     Ready_TerrainProto();

protected:
    CLayer* m_pEnvironment_Layer;
    CLayer* m_pGameLogic_Layer;

private:
    virtual void Free();
};

