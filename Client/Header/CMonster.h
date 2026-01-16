#pragma once
#include "CCharacter.h"
#include "Engine_Enum.h"
namespace Engine
{
	class CAnimation;
}

class CMonster : public CCharacter
{
protected:
	enum MONSTER_STATE : _byte { MS_IDLE, MS_AIM, MS_ATTACK_IDLE, MS_ATTACK, MS_ATTACK2, MS_ATTACK3,
		MS_WALK,MS_GUARD, MS_HIT,MS_LAUNCH,MS_FLYBACK, MS_DEAD, MS_END };

	enum MONSTER_DEAD_TYPE :_byte
	{
		NONE, SLICE, BOMB, ELECT, HEAD, DEAD_END
	};
protected:
	explicit		CMonster(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit		CMonster(const CMonster& rhs);
	virtual			~CMonster();

public:
	HRESULT			Ready_GameObject() override;
	_int			Update_GameObject(const _float& fTimeDelta) override;
	void			LateUpdate_GameObject(const _float& fTimeDelta) override;
	void			Render_GameObject() override;

protected:
	HRESULT			Add_Component() override;
	void			SetBillboard();
protected:
	virtual void	Free();

//Perceive
protected:
	HRESULT			GetDistVecToPlayer(_vec3& pOutDist);
	CTransform*		GetCameraTransform();
	CTransform*		GetPlayerTransform();
	CCollision*		GetPlayerCollision();
//State
	virtual	void	Launch(); 
	void			SetLaunched();

public:
	void			Activate() override;
	void			Deactivate() override;
protected:
	CAnimation*		m_pAnimationCom;
	CTransform*		m_pCameraTransformCom;
	CTransform*		m_pPlayerTransformCom;
	CCollision*		m_pPlayerCollisionCom;

	_float m_fAttackableDist; 
	_vec3 m_vDir;
	_float m_fSpeed;

	const _float m_fLaunchTime = 0.2f;
	_float m_fLaunchSpeed = 2.f;


};

