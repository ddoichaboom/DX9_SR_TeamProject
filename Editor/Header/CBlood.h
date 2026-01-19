#pragma once
#include "CParticleEmitter.h"

class CBlood :
	public Engine::CParticleEmitter
{
public:
	CBlood(IDirect3DDevice9* device, _vec3* _origin, int numParticles);
	virtual ~CBlood();

	HRESULT		Ready_GameObject() override;
	_int		Update_GameObject(const _float& fTimeDelta);
	void		Render_GameObject() override;
	void		SetPreRenderState() override;
	void		SetPostRenderState() override;

protected:
	HRESULT		Add_Component() override;
	void		ResetParticle(Particle* particle) override;
public:
	static CBlood* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3* _origin, int numParticles);
	static TextureSource& GetTextureSource()
	{
		return m_TextureSource;
	}
protected:
	void		Free() override;

protected:
	static TextureSource m_TextureSource;
	int			m_iBatchSize = 0;
};

