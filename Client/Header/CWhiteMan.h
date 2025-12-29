#pragma once
#include "CMonster.h"

namespace Engine
{
	class CCollision;
	class CCalculator;
}

class CWhiteMan :
	public CMonster
{
protected:
	explicit		CWhiteMan(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit		CWhiteMan(const CWhiteMan& rhs);
	virtual			~CWhiteMan();
public:
	//아래 정적 함수들은 캐릭터 클래스마다 정의하기
	static void		CreateStateData();
	static vector<TextureSource>& GetTextureSources()
	{
		return m_vTextureSource;
	}
	static vector<AnimationSource>& GetAnimSources()
	{
		return m_vAnimSource;
	}

	static CWhiteMan* Create(LPDIRECT3DDEVICE9 pGraphicDev);


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

protected:
	//State Function 
	void Begin_Idle();

	void Begin_Attack();
	void Attack();
	void End_Attack();

	void Begin_Hit();
	void Hit();

protected:
	CCollision* m_pCollisionCom;
	CCalculator* m_pCalculatorCom;

	static vector<TextureSource> m_vTextureSource;
	static vector<AnimationSource> m_vAnimSource;

};

