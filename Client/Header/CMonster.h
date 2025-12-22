#pragma once
#include "CCharacter.h"

namespace Engine
{
	class CAnimation;
}

class CMonster : public CCharacter
{
protected:
	enum MONSTER_STATE{ MS_IDLE =0, MS_TRACE = 1,MS_FOUND=2, MS_ATTACK =5, Walk =6, MS_HIT =7, MS_DEAD=8 , MS_END};

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
	virtual void	ChangeState(MONSTER_STATE nextState) PURE;

	HRESULT			Add_Component() override;
protected:
	virtual void	Free();

protected:
	Engine::CAnimation* m_pAnimationCom;
	MONSTER_STATE m_EState = MS_END;

	_float m_fPerceiveDist;
	CCharacter* m_pTarget;
};

