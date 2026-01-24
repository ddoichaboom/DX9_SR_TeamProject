#pragma once
#include "CScene.h"
class CLogo :
    public CScene
{
protected:
	explicit CLogo(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLogo();

public:
	HRESULT		Ready_Scene() override;
	_int		Update_Scene(const _float& fTimeDelta) override;
	void		Render_Scene() override {};

public:
	static CLogo* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	void Free() override;
private:
	wstring m_logoVideoName = L"Opening.wmv";
	wstring m_logoSoundName = L"OpeningSound.wav";
};

