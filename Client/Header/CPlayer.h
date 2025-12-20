#pragma once
#include "CCharacter.h"

class CPlayer :
    public CCharacter
{
protected:
	explicit		CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit		CPlayer(const CPlayer& rhs);
	virtual			~CPlayer();

public:
	HRESULT			Ready_GameObject() override;
	_int			Update_GameObject(const _float& fTimeDelta) override;
	void			LateUpdate_GameObject(const _float& fTimeDelta) override;
	void			Render_GameObject() override;

protected:
	HRESULT			Add_Component() override;
	void			Key_Input(const _float& fTimeDelta);

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free();
};

