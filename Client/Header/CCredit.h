#pragma once
#include "CScene.h"
class CCredit :
	public CScene
{
protected:
	explicit CCredit(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCredit();

public:
	HRESULT		Ready_Scene() override;
	_int		Update_Scene(const _float& fTimeDelta) override;
	void		Render_Scene() override {};

public:
	static CCredit* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	void Free() override;
private:
	wstring m_CreditVideoName = L"EndingCredits.wmv";
	wstring m_CreditSoundName = L"SFX_Credits.wav";
};

