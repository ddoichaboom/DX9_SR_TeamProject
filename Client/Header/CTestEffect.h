#pragma once

#include "CParticleEmitter.h"

class CTestEffect :
    public CParticleEmitter
{
public:
	CTestEffect(IDirect3DDevice9* device, _vec3* _origin, int numParticles);
	virtual ~CTestEffect();

	HRESULT		Ready_GameObject() override;
	_int		Update_GameObject(const _float& fTimeDelta);
	void		Render_GameObject() override;

	void		SetPreRenderState() override;

	HRESULT Add_Component() override;
	static CTestEffect* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3* _origin, int numParticles);
	void ResetParticle(Particle* particle) override;

protected:
	void		Free() override;
	int			m_iBatchSize = 0;
};

