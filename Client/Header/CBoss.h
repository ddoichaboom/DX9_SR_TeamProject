#pragma once
#include "CMonster.h"
#include <random>

namespace Engine
{
	class CCollider;
}

class CBeam;
class CBossTrail;
class CBossHPUI;

class CBoss :
    public CMonster
{
	enum MON_HAND { MON_LEFT_HAND, MON_RIGHT_HAND, MON_END_HAND};
protected:
	explicit		CBoss(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit		CBoss(const CBoss& rhs);
	virtual			~CBoss();
public:
	static void		CreateStateData();
	static vector<TextureSource>& GetTextureSources()
	{
		return m_vTextureSource;
	}
	static vector<AnimationSource>& GetAnimSources()
	{
		return m_vAnimSource;
	}

	static CBoss* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CBoss* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);


protected:
	HRESULT			Ready_GameObject() override;
	_int			Update_GameObject(const _float& fTimeDelta) override;
	void			LateUpdate_GameObject(const _float& fTimeDelta) override;
	void			Render_GameObject() override;

protected:
	void			ChangeState(_uint nextStateID) override;
	HRESULT			Add_Component() override;
	void			Collision_Beam();
protected:
	virtual void	Free();
	void			OnBodyCollision(CollisionInfo info);
	void			OnAnimationChange(_float _animAspect);

protected:
	void			Move(const _float& fTimeDelta, _float& _dirAngle, _float ratio = 1.f);
	void			SetAngle(_float _degree)
	{
		//m_fDirAngle = D3DXToRadian(_degree);
		m_fDirAngle = _degree;
	}

	void			Idle_Begin();
	void			Idle();

	void			Dash_Begin();
	void			Dash();

	void			Attack_Idle();

	//Attack Beam
	void			Attack_Beam();
	void			Reset_Beam();
	void			Run_Beam(_float _ratio);

	_vec3			GetHandWorldPos(MON_HAND _eHand);
	_vec3			GetRocketWorldPos(MON_HAND _eHand);
	//Attack Bullet
	void			Attack_Bullet();

	//Attack Rocket
	void			Attack_Rocket_Begin();
	void			Attack_Rocket();

	void			Guard();
	void			Dead();

	void			ReverseDir()
	{
		m_fDirOffset *= -1.f;
	}

public:
	void			Activate() override;
	//void			Deactivate() override;

protected:
	static vector<TextureSource> m_vTextureSource;
	static vector<AnimationSource> m_vAnimSource;

	CCollider* m_pBodyCollider;
	const _tchar* m_szBodyColliderName = L"ColBody";

protected:
	bool			m_bBeamCollision = false;
	_float			m_fMapRadius;
	//방향 세팅값 -1 / 1
	_float			m_fDirOffset;
	//bool			m_bDash;

	_float			m_fDashSpeed = 800.f;
	_float			m_fIdleSpeed = 200.f;
	_float			m_fBaseSpeed = 50.f;

	_vec3			m_vScale;
	_float			m_fDirAngle = 0.f;
	//높이 제한 
	//const _vec2		m_vHeightRange = { 30.f, 200.f };
	const _vec2		m_vHeightRange = { 100.f, 250.f };
	const _vec2		m_vAngleABSRange = { 10.f, 30.f };

	_float			m_fStateRatio = 1.f;
	_float			m_fSubTime = 0.f;

	//Idle
	_float			m_fIdle_Time = 1.f;

	//Dash
	_float			m_fDash_Time = 0.4f;

	//Attack Bullet
	_float			m_fAttack_Bullet_Time = 2.5f;
	_float			m_fShoot_time = 0.15f;

	//Attack Beam
	_float			m_fAttack_Beam_Time = 1.8f;
	CBeam*			m_pBeam[MON_END_HAND];
	_vec3			m_vBeamStartPos[MON_END_HAND];
	_vec3			m_vBeamEndPos[MON_END_HAND];

	_vec3			m_vHandPos[MON_END_HAND];
	_vec3			m_vWorldHandPos[MON_END_HAND];

	//Attack Rocket
	_vec3			m_vRocektPos[MON_END_HAND];
	_vec3			m_vWorldRocketPos[MON_END_HAND];
	_float			m_fAttack_Rocket_Time = 2.f;
	_float			m_fRocketShoot_time = 0.25f;

	//Random
	random_device	rd;
	mt19937			gen;
	uniform_int_distribution<_int> dis;
	uniform_real_distribution<_float> floatDis;
	static _vec2	m_vRandomRange;

	//Effect
	CBossTrail*		m_pBossTrail;
	CBossHPUI*		m_pBossHPUI;

};

