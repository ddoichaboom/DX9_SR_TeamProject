#pragma once
#include "CCharacter.h"
#include "Engine_Define.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CAnimation;
}


class CPlayer;

class CPlayerPart : public CGameObject
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
	explicit	CPlayerPart(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CPlayerPart(const CPlayerPart& rhs);
	virtual		~CPlayerPart();

public:
	static _uint GetStateID(_byte _state, _byte _subState)
	{
		return ((_uint)_subState << 4) | (_uint)_state;
	}

public:
	virtual void	ChangeState(_uint nextStateID) PURE;
	virtual _bool	Get_ActionAble()	PURE;


public:
	virtual		HRESULT	Ready_GameObject()	override;
	virtual		_int	Update_GameObject(const _float& fTimeDelta) override;
	virtual		void	LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual		void	Render_GameObject() override;

public:
	void	SetParent(CPlayer* pPlayer);
	void	SetPos(_vec3 _pos);
	void	SetWeapon(_byte eState) { m_eWeaponState = (STATE_WEAPON)eState; }
	void	Set_Rendering(_bool bRender) { m_bRendering = bRender; }
	_bool	Get_Rendering()	const { return m_bRendering; }

protected:
	virtual HRESULT	Add_Component();
	virtual void	Free();

protected:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CAnimation* m_pAnimationCom;
	Engine::CStateComponent* m_pStateCom;

protected:
	_float m_fTime;
	_bool	m_bRendering;
	CPlayer* m_pPlayer;

	STATE_WEAPON	m_eWeaponState;
};

