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
	void	SetWeapon(_byte eState) { m_eWeaponState = (WEAPON_STATE)eState; }
	void	Set_Rendering(_bool bRender) { m_bRendering = bRender; }
	_bool	Get_Rendering()	const { return m_bRendering; }
	CPlayer* Get_Player() { if (nullptr != m_pPlayer) return m_pPlayer;  return nullptr; }

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
	_float	m_fTime;
	_float	m_fDelayTime;
	_bool	m_bDelay;
	_bool	m_bRendering;

	CPlayer* m_pPlayer;

	WEAPON_STATE	m_eWeaponState;

	_vec3		m_vConvertPos;
	_vec3		m_vConvertScale;
};

