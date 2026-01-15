#pragma once
#include "CGameObject.h"

class CPlayerPart;

class CWeapon : public CGameObject
{
public:
	enum STATE_MAIN : _byte
	{
		IDLE = 1,
		RELOAD = 2,
		ATTACK = 3,
		KICK = 5,
		DRINK = 6,
		SLIDE = 7,
		INTRO = 8,
		MAIN_END
	};

	enum STATE_WEAPON : _byte
	{
		SW_NONE = 0,
		SW_PISTOL = 1,
		SW_SHOTGUN = 2,
		SW_KATANA = 3,
		SW_END
	};

protected:
	explicit		CWeapon(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit		CWeapon(const CWeapon& rhs);
	virtual			~CWeapon();

public:
	STATE_WEAPON Get_WeaponState() const { return m_eWeaponState; }
	_bool	Get_IsSelect() const { return m_bSelect; }
	_bool	Get_IsEmpty() const { return m_bIsEmpty; }
	_bool	Get_IsShootAble() const { return m_bShootAble; }
	_int	Get_NowBullet() const { return m_iNowBullet; }
	_int	Get_MaxBullet() const { return m_iMaxBullet; }
	_int	Get_Power() const { return m_fPower; }
	_float	Get_CoolTime() const { return m_fCoolTime; }
	_float	Get_Range() const { return m_fRange; }

	void	Set_WeaponState(_byte eState) { m_eWeaponState = (STATE_WEAPON)eState; }
	void	Set_Select(_bool  bSelect) { m_bSelect = bSelect; }
	void	Set_Empty(_bool  bIsEmpty) { m_bIsEmpty = bIsEmpty; }
	void	Set_ShootAble(_bool bAble) { m_bShootAble = bAble; }
	void	Set_NowBullet(_int	iNowBullet) { m_iNowBullet = iNowBullet; }
	void	Set_MaxBullet(_int	iMaxBullet) { m_iMaxBullet = iMaxBullet; }
	void	Set_Power(_float	iPower) { m_fPower = iPower; }
	void	Set_CoolTime(_float	fCoolTime) { m_fCoolTime = fCoolTime; }
	void	Set_Range(_float	fRange) { m_fRange = fRange; }

	void	Set_Parent(CPlayerPart* pParent);


public:
	virtual		HRESULT		Ready_GameObject();
	virtual		_int		Update_GameObject(const _float& fTimeDelta);
	virtual		void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual		void		Render_GameObject();

	virtual		void		Activate();
	virtual		void		Deactivate();


public:
	virtual		_bool		Can_Fire() PURE;
	virtual		void		Fire() {}
	virtual		void		Reload() {}

protected:
	virtual		HRESULT				Add_Component();
	virtual		void				Free();

protected:
	CPlayerPart* m_pParentPart;
	STATE_WEAPON m_eWeaponState;
	_bool	m_bSelect;
	_bool	m_bIsEmpty;
	_bool	m_bShootAble;
	_int	m_iNowBullet;
	_int	m_iMaxBullet;
	_float	m_fPower;
	_float	m_fCoolTime;
	_float	m_fRange;
	_float	m_fTime;
};

