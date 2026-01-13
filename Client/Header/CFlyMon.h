#pragma once
#include "CMonster.h"
namespace Engine
{
	class CCollider;
}

class CFlyMon :
    public CMonster
{

protected:
	explicit		CFlyMon(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit		CFlyMon(const CFlyMon& rhs);
	virtual			~CFlyMon();

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

	static CFlyMon* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	HRESULT			Ready_GameObject() override;
	_int			Update_GameObject(const _float& fTimeDelta) override;
	void			LateUpdate_GameObject(const _float& fTimeDelta) override;
	void			Render_GameObject() override;

protected:
	void			ChangeState(_uint nextStateID) override;
	HRESULT			Add_Component() override;

protected:
	void			OnAnimationChange(_float _animAspect);
	void			OnBodyCollision(CollisionInfo info);
	void			TracePlayer(const _float& fTimeDelta);
	virtual void	Free();

protected:
	void			Idle();
	void			Attack_Idle();
	void			Attack();
	void			Launch();
	void			Dead();

public:
	void			Activate() override;
	//void			Deactivate() override;

protected:
	static vector<TextureSource> m_vTextureSource;
	static vector<AnimationSource> m_vAnimSource;

	CCollider* m_pBodyCollider;
	const _tchar* m_szBodyColliderName = L"ColBody";

	_float	m_fAttackDist = 10.f;
	_float	m_fTraceSpeed = 30.f;

	_bool	m_fNear;

};

