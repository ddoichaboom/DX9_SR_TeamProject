#pragma once
#include "CMonster.h"

namespace Engine
{
	class CCollider;
}

class CBeam;
class CBeamMon :
    public CMonster
{
protected:
	explicit		CBeamMon(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit		CBeamMon(const CBeamMon& rhs);
	virtual			~CBeamMon();

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

	static CBeamMon* Create(LPDIRECT3DDEVICE9 pGraphicDev);


protected:
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
	virtual void	Free();
	
protected:
	void			Idle();
	void			Attack_Idle();
	void			Attack();
	void			Dead();
protected:
	void			ResetBeam();
	bool			RunBeam(const _float& fTimeDelta);
	Engine::CTransform* GetPlayerTransform();
public:
	void			Activate() override;
	//void			Deactivate() override;

protected:
	static vector<TextureSource> m_vTextureSource;
	static vector<AnimationSource> m_vAnimSource;

protected:
	_float			m_fAttackDelayTime = 3.f;
	_float			m_fAttackResetTime = 2.f;
	CCollider*		m_pBodyCollider;
	const _tchar*	m_szBodyColliderName = L"ColBody";

	const _float	m_fYScaleOffset = 1.2f;

	CBeam*			m_pBeam;
	bool			m_bShooting = false;
	_float			m_fMaxRotValue = D3DX_PI*0.5f;
	_float			m_fRotValue = 0.f;
	_float			m_fRotSpeed = 0.2f;

	_vec3			m_vShootDir;
	Engine::CTransform* m_pPlayerTransform;

};

