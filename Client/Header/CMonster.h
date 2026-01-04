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
	enum MONSTER_STATE : _byte { MS_IDLE, MS_AIM, MS_ATTACK_IDLE, MS_ATTACK, MS_WALK, MS_HIT,MS_LAUNCH, MS_DEAD, MS_END };

protected:
	explicit		CMonster(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit		CMonster(const CMonster& rhs);
	virtual			~CMonster();

protected:
	HRESULT			Ready_GameObject() override;
	_int			Update_GameObject(const _float& fTimeDelta) override;
	void			LateUpdate_GameObject(const _float& fTimeDelta) override;
	void			Render_GameObject() override;

protected:
	HRESULT			Add_Component() override;
	void			SetBillboard();
	Engine::CTransform* GetPlayerTransformCom();
protected:
	virtual void	Free();

//Perceive
protected:
	HRESULT			GetDistVecToPlayer(_vec3& pOutDist);
//State
	virtual	void	Launch(); 
	void			SetLaunched();
protected:
	Engine::CAnimation* m_pAnimationCom;
	Engine::CTransform* m_pPlayerTransformCom = nullptr;

	_float m_fAttackableDist; 
	_vec3 m_vDir;
	_float m_fSpeed;
	_float m_fHP;

	const _float m_fLaunchTime = 0.2f;
	_float m_fLaunchSpeed = 2.f;

	
};

