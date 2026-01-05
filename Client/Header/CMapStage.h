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

protected:
    CLayer* m_pEnvironment_Layer;
    CLayer* m_pGameLogic_Layer;

    CObjectPool<CWhiteMan>* m_pWhiteManPool;
    CObjectPool<CTerrain>* m_pTerrainPool;
    CObjectPool<CBullet>* m_pBulletPool;

private:
    virtual void Free();
};

