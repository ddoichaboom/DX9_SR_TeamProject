#pragma once
#include "CStage.h"
#include "CEventMgr.h"

class CSniperPlayer;

class CSniperStage :
    public CStage, public IListener
{
protected:
    explicit        CSniperStage(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual         ~CSniperStage();

public:
    HRESULT         Ready_Scene() override;
    _int            Update_Scene(const _float& fTimeDelta) override;
    void            LateUpdate_Scene(const _float& fTimeDelta) override;
    void            Render_Scene() override;

public:
    static CSniperStage* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    void                OnEvent(EVENT_TYPE _type, EventData* _pData) override;

protected:
    HRESULT         Ready_Environment_Layer(const _tchar* pLayerTag) override;
    HRESULT         Ready_GameLogic_Layer(const _tchar* pLayerTag) override;

    //스레드 전달 함수
    HRESULT        Ready_Prototype() override;

    HRESULT        Remove_PrevObjectPool() override;

    HRESULT        Ready_ObjectPool_Character()    override;
    HRESULT        Ready_ObjectPool_Terrain()      override;
    HRESULT        Ready_ObjectPool_UI()           override;
    HRESULT        Ready_ObjectPool_Effect()       override;


    HRESULT        Ready_CharacterTextureProto()   override;
    HRESULT        Ready_TerrainTextureProto()     override;
    HRESULT        Ready_UITextureProto()          override;
    HRESULT        Ready_EffectTextureProto()      override;

    void           Check_Collision() override;
    void           SpawnMonster();
private:
    virtual void Free();

private:
    CSniperPlayer* m_pPlayer;

    queue<_vec3> m_qMonsterSpawnPoses;
    _int        m_iFileIndex = 4;

    _float      m_fTime = 0.f;
    _float      m_fEndTime = 0.f;
    _float      m_fSpawnTime = 3.f;
   _float       m_fOriginSpawnTime = 5.f;
    _float      m_fOffsetTime = 0.25f;
    _float      m_fMinTime = 2.0f;
    _int        m_iKillCount = 0;
    _int        m_iSpawnCount = 0;

    _bool       m_bFirstSpawn = true;
    _bool       m_bStartSound = false;

    _bool       m_bEndingPos = false;
};

