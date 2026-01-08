#pragma once
#include "CCharacter.h"

class CLeftPart;
class CRightPart;
class CMiddlePart;
class CWeapon;

class CPlayer :
	public CCharacter
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
	explicit		CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit		CPlayer(const CPlayer& rhs);
	virtual			~CPlayer();

public:
	static _uint GetStateID(_byte _state, _byte _subState)
	{
		return ((_uint)_subState << 4) | (_uint)_state;
	}

	_byte Get_WeaponState() { return (_byte)m_eWeaponState; }



public:
	HRESULT			Ready_GameObject() override;
	_int			Update_GameObject(const _float& fTimeDelta) override;
	void			LateUpdate_GameObject(const _float& fTimeDelta) override;
	void			Render_GameObject() override;

public:
	HRESULT			Add_Component() override;
	HRESULT			Add_PlayerPart();


	void			Key_Input(const _float& fTimeDelta);

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot = { 0.f, 0.f, 1.f }, _vec3 vScale = { 6.f, 6.f, 1.f });


	void			Set_LeftPart(CLeftPart* pLeft);
	void			Set_RightPart(CRightPart* pRight);
	void			Set_MiddlePart(CMiddlePart* pMiddle);

	void			Add_Weapon(_byte eWeaponTag, CWeapon* pWeapon);


protected:
	virtual void	Free();
	void			OnCollision(CollisionInfo info);
	void			CheckPickedMonster();

private:
	void Move_Input(const _float& fTimeDelta, const _vec3& vRight, const _vec3& vLook);
	void Action_Input(const _float& fTimeDelta, const _vec3& vLook);


public:
	void				Fire();
	void				Reload();

	


private:
	CLeftPart* m_pLeftPart;
	CRightPart* m_pRightPart;
	CMiddlePart* m_pMiddlePart;


	STATE_WEAPON	m_eWeaponState;
	unordered_map<STATE_WEAPON, CWeapon*> m_mapWeapon;

	_float	m_fMoveSpeed;

	
	_float	m_fAtk = 6.f;
	_float  m_JumpPower = 40.f;
};

