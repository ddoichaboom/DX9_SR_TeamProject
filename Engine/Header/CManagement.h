#pragma once
#include	"CBase.h"
#include	"CScene.h"

BEGIN(Engine)

class ENGINE_DLL CManagement : public CBase
{
	DECLARE_SINGLETON(CManagement)

private:
	explicit	CManagement();
	virtual		~CManagement();

public:
	CComponent* Get_Component(COMPONENTID eID,
		const _tchar* pLayerTag,
		OBJ_ID _objID,
		const _tchar* pComponentTag);
	CLayer* Get_Layer(const _tchar* pLayerTag);


	_uint					GetPlayerID() { return m_iPlayerID; }

public:
	HRESULT					Set_Scene(CScene* pScene);
	_int					Update_Scene(const _float& fTimeDelta);
	void					LateUpdate_Scene(const _float& fTimeDelta);
	void					Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	CScene*					m_pScene;

public:
	virtual void			Free();

public :
	void					Set_CurrSceneType(SCENE_TYPE eSceneType) { m_eCurrSceneType = eSceneType; }
	SCENE_TYPE				Get_CurrSceneType() { return m_eCurrSceneType; }
	_uint					Get_FloorNumber();

	void					Set_CountTime(bool bCountTime) { m_bCountTime = bCountTime; }
	void					Reset_CountTime() { m_fTime = 0.f; }
	wstring					Convert_PlayTime();
	wstring					Convert_StageInfo();
	void					Update_CountTime(const _float& fTimeDelta);
	
	

protected:
	//Player ID는 0으로 고정 
	_uint			m_iPlayerID = 0;
	SCENE_TYPE		m_eCurrSceneType;

	_float			m_fTime;
	_bool			m_bCountTime;
};

END