#pragma once
#include "CStage.h"
#include "CEventMgr.h"
#include "CRoom.h"

class CLoading;
class CMapCollider;
class CRoadStage : public CStage, public IListener
{
protected:
    explicit        CRoadStage(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual         ~CRoadStage();

public:
    HRESULT         Ready_Scene() override;
    _int            Update_Scene(const _float& fTimeDelta) override;
    void            LateUpdate_Scene(const _float& fTimeDelta) override;
    void            Render_Scene() override;

public:
    static CRoadStage* Create(LPDIRECT3DDEVICE9 pGraphicDev);

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

    void            Check_Collision() override;
    void            OnEvent(EVENT_TYPE _type, EventData* _pData) override;

    void            Move_EnvObjects(const _float& fTimeDelta);
    void           SpawnMonster();

private:
    virtual void Free();

private :
    
    _bool               m_bStartSound = false;

public :
    static  wstring     szRoadMapBGM;
    _int                m_iFileIndex;
    enum                { ROOM_CNT = 2 };
    CRoom*              m_pRoom[ROOM_CNT];

    _float              m_fSpeed = -100.f;
    const _float        m_fZHalfRadius = 160.f;
    const _vec3        m_vEndColliderZPos = { 0, -10.f, -400.f };
    CMapCollider*       m_pEndMapCollider;

    queue<int>          m_qRoomOrder;

   // const _int m_iRoomCnt = 2;

    _float  m_fTime;

    vector<_vec3> m_vecSpawnPoses;
    //_float      m_fTime = 0.f;   
    _float      m_fSpawnTime = 2.f;
    _float       m_fOriginSpawnTime = 5.f;
    _float      m_fOffsetTime = 0.25f;
    _float      m_fMinTime = 2.0f;

    _bool       m_bFirstSpawn = true;
};

