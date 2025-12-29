#pragma once
#include "CCharacter.h"

namespace Engine
{
	class CAnimation;
}

class CMonster : public CCharacter
{
protected:
	enum MONSTER_STATE : _byte { MS_IDLE = 0, MS_TRACE = 1, MS_FOUND = 2, MS_ATTACK = 5, MS_WALK = 6, MS_HIT = 7, MS_DEAD = 8, MS_END };
	enum MONSTER_STATE_SUB : _byte { SUB_NONE, SUB_BEGIN, SUB_END, MON_SUB_END };
	//Sub를 쓰지않을 때 = Sub가 0일 때
	static _uint GetStateID(MONSTER_STATE _state, MONSTER_STATE_SUB _subState)
	{
		return ((_uint)_subState << 8 ) | (_uint)_state;
	}

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
protected:
	virtual void	Free();

protected:
	Engine::CAnimation* m_pAnimationCom;

	_float m_fPerceiveDist;
	CCharacter* m_pTarget;
};

