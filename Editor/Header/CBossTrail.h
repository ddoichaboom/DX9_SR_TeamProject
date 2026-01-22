#pragma once
#include "CParticleEmitter.h"

class CBossTrail :
    public CParticleEmitter
{
public:
	CBossTrail(IDirect3DDevice9* device);
	virtual ~CBossTrail();

	HRESULT		Ready_GameObject() override;
	_int		Update_GameObject(const _float& fTimeDelta);
	void		Render_GameObject() override;
	void		SetPreRenderState() override;
	void		SetPostRenderState() override;

public:
	_uint		GetTextureCnt() override { return 1; }
protected:
	HRESULT		Add_Component() override;
	void		ResetParticle(Particle* particle) override;
public:
	static CBossTrail* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static TextureSource& GetTextureSources()
	{
		return m_TextureSource;
	}
protected:
	void		Free() override;

protected:
	static TextureSource m_TextureSource;
	int			m_iBatchSize = 0;

};

