#pragma once
#include "CStage.h"

class CLoading;
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

    //스레드 전달 함수
    HRESULT     Ready_Prototype() override;
    HRESULT     Ready_Prototype_OnlyTexture();
    // TODO:상위 클래스에 담아서 스테이지마다 다르게 해도 될듯함 - 호준
    HRESULT     Ready_ObjectPool();
    //

    HRESULT     Ready_PlayerTextureProto();
    HRESULT     Ready_MonsterTextureProto();
    HRESULT     Ready_TerrainTextureProto();
    HRESULT     Ready_UITextureProto();

    void        Update_RoomLoading(const _float& fTimeDelta);
    void        Change_Room(_int iNewRoomIndex);

protected:
    CLayer*     m_pEnvironment_Layer;
    CLayer*     m_pGameLogic_Layer;

    wstring     m_wstrCurrentMapFile;       // 현재 맵 파일 경로 
    _int        m_iCurrentRoomIndex;        // 현재 방 번호
    set<_int>   m_setLoadedRooms;           // 로드된 방 번호 집합 (중복 X) 

    CLoading*   m_pLoading;

    HRESULT	    m_BaseResult;
    HRESULT	    m_TextureResult;
    HRESULT	    m_ObjectPoolResult;
    HRESULT	    m_ReadyEnvResult;
    HRESULT	    m_ReadyGameResult;

private:
    virtual void Free();

};

