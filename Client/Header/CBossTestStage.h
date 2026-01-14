#pragma once
#include "CStage.h"

class CWhiteMan;
class CBullet;

class CBossTestStage : public CStage
{
protected:
	explicit					CBossTestStage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual						~CBossTestStage();

public:
	HRESULT						Ready_Scene() override;
	_int						Update_Scene(const _float& fTimeDelta) override;
	void						LateUpdate_Scene(const _float& fTimeDelta) override;
	void						Render_Scene() override;

public:
	static CBossTestStage*			Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	HRESULT						Ready_ObjectPool();
	HRESULT						Ready_Environment_Layer(const _tchar* pLayerTag) override;
	HRESULT						Ready_GameLogic_Layer(const _tchar* pLayerTag) override;
	HRESULT						Ready_Prototype() override;
protected:
	virtual void				Free();

protected:
	CLayer* m_pEnvironment_Layer;
	CLayer* m_pGameLogic_Layer;

};

