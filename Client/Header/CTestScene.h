#pragma once
#include "CScene.h"
class CTestScene :
	public CScene
{
protected:
	explicit CTestScene(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTestScene();

public:
	HRESULT		Ready_Scene() override;
	_int		Update_Scene(const _float& fTimeDelta) override;
	void		Render_Scene() override {};

public:
	static CTestScene* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	void Free() override;
};

