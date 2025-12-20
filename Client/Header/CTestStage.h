#pragma once
#include "CStage.h"

class CTestStage : public CStage
{
protected:
	explicit					CTestStage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual						~CTestStage();

public:
	HRESULT						Ready_Scene() override;
	_int						Update_Scene(const _float& fTimeDelta) override;
	void						LateUpdate_Scene(const _float& fTimeDelta) override;
	void						Render_Scene() override;

public:
	static CTestStage*			Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	HRESULT						Ready_Environment_Layer(const _tchar* pLayerTag) override;
	HRESULT						Ready_GameLogic_Layer(const _tchar* pLayerTag) override;
	HRESULT						Ready_Prototype() override;
private:
	virtual void Free();

};

