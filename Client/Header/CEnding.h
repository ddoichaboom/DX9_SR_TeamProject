#pragma once
#include "CScene.h"
class CEnding :
	public CScene
{
protected:
	explicit CEnding(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEnding();

public:
	HRESULT		Ready_Scene() override;
	_int		Update_Scene(const _float& fTimeDelta) override;
	void		Render_Scene() override {};

public:
	static CEnding* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	void Free() override;
private:
	wstring m_endingVideoName = L"EndingBoss.wmv";
	wstring m_endingSoundName = L"EndBossSound.wav";
};

