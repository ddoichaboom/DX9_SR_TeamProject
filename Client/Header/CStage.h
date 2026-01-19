#pragma once
#include "CScene.h"

class CBackGround;
class CLoading;

class CStage : public CScene
{
protected:
	explicit CStage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStage();

public:
	virtual			HRESULT		Ready_Scene() PURE;
	virtual			_int		Update_Scene(const _float& fTimeDelta);
	virtual			void		LateUpdate_Scene(const _float& fTimeDelta);
	virtual			void		Render_Scene();

protected:
	virtual HRESULT				Ready_Environment_Layer(const _tchar* pLayerTag) PURE;
	virtual HRESULT				Ready_GameLogic_Layer(const _tchar* pLayerTag) PURE;
	//HRESULT					Ready_UI_Layer(const _tchar* pLayerTag);
	//HRESULT					Ready_Light();

	virtual HRESULT				Ready_Prototype() PURE;
	virtual	HRESULT				Ready_Prototype_OnlyTexture() PURE;
	virtual	HRESULT				Ready_ObjectPool() PURE;

protected:
	virtual	void				Check_Collision() {} ;

protected:
//	void						Update_RoomLoading(const _float& fTimeDelta);
	void						Change_Room(_int iNewRoomIndex);

private:
	virtual void Free();

protected:
	CBackGround*				m_pBackGround;
	CLayer*						m_pEnvironment_Layer;
	CLayer*						m_pGameLogic_Layer;

	wstring						m_wstrCurrentMapFile;       // 현재 맵 파일 경로 
	_int						m_iCurrentRoomIndex;        // 현재 방 번호
	set<_int>					m_setLoadedRooms;           // 로드된 방 번호 집합 (중복 X) 

protected:
	CLoading*					m_pLoading;

	//Loading 결과값 
	HRESULT						m_BaseResult;
	HRESULT						m_TextureResult;
	HRESULT						m_ObjectPoolResult;
	HRESULT						m_ReadyEnvResult;
	HRESULT						m_ReadyGameResult;

};

