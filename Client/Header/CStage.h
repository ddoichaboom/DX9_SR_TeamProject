#pragma once
#include "CScene.h"

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
	//HRESULT		Ready_UI_Layer(const _tchar* pLayerTag);
	//HRESULT		Ready_Light();
	virtual HRESULT				Ready_Prototype() PURE;


private:
	virtual void Free();

};

