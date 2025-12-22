#pragma once
#include "CCharacter.h"

class CPlayerPart;

class CPlayer :
	public CCharacter
{
public :
	enum PLAYER_STATE
	{
		PLAYER_UNACTIVE = 999,
		PLAYER_IDLE		= 0,
		PLAYER_ATTACK	= 1,
		PLAYER_RELOAD	= 2,
		PLAYER_KICK		= 3,
		PLAYER_DRINK	= 4,
		PLAYER_SLIDE
	};
	enum WEAPON_STATE
	{
		WS_PISTOL = 0,
		WS_SHOTGUN,
		WS_KATANA
	};

protected:
	explicit		CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit		CPlayer(const CPlayer& rhs);
	virtual			~CPlayer();

public :
	void			Change_State(PLAYER_STATE eState);
	void			Set_WeaponState(WEAPON_STATE eState);

public:
	HRESULT			Ready_GameObject() override;
	_int			Update_GameObject(const _float& fTimeDelta) override;
	void			LateUpdate_GameObject(const _float& fTimeDelta) override;
	void			Render_GameObject() override;

protected:
	HRESULT			Add_Component() override;
	void			Key_Input(const _float& fTimeDelta);

private:
	HRESULT			Add_PlayerPart();

	void			Check_AnimationState();

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free();

private:
	CPlayerPart* m_pLeftPart;
	CPlayerPart* m_pRightPart;
	CPlayerPart* m_pMiddlePart;


	_bool		m_bCheck;

	PLAYER_STATE	m_eNowState;
	WEAPON_STATE m_eWeaponState;

};

