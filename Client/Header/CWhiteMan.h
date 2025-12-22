#pragma once
#include "CMonster.h"

namespace Engine
{
	class CCollider;
	class CCalculator;
}

class CWhiteMan :
    public CMonster
{
protected:
	explicit		CWhiteMan(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit		CWhiteMan(const CWhiteMan& rhs);
	virtual			~CWhiteMan();

protected:
	HRESULT			Ready_GameObject() override;
	_int			Update_GameObject(const _float& fTimeDelta) override;
	void			LateUpdate_GameObject(const _float& fTimeDelta) override;
	void			Render_GameObject() override;

protected:
	void			ChangeState(MONSTER_STATE nextState) override;
	HRESULT			Add_Component() override;
protected:
	virtual void	Free();
	void			OnCollision(CollisionInfo info);

public:
	static CWhiteMan* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	void UpdateState();
	//void Trace();



protected:
	CCollider* m_pCollider;
	CCalculator* m_pCalculatorCom;
};

