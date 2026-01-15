#pragma once
#include "CWeapon.h"

class CPlayerPart;

class CPistol : public CWeapon
{
protected:
	explicit		CPistol(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit		CPistol(const CPistol& rhs);
	virtual			~CPistol();

public:
	virtual		HRESULT		Ready_GameObject();
	virtual		_int		Update_GameObject(const _float& fTimeDelta);
	virtual		void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual		void		Render_GameObject();

	virtual		void		Activate() override;
	virtual		void		Deactivate() override;

public:
	static		CPistol* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual		_bool		Can_Fire() override;
	virtual		void		Fire() override;
	virtual		void		Reload() override;

protected:
	virtual		HRESULT		Add_Component();
	virtual		void		Free();
};

