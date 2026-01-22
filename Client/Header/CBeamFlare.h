#pragma once
#include "CParticleEmitter.h"
class CBeamFlare :
	public CParticleEmitter
{
public:
	CBeamFlare(IDirect3DDevice9* device);
	virtual ~CBeamFlare();

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
	static CBeamFlare* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static TextureSource& GetTextureSource()
	{
		return m_TextureSource;
	}
protected:
	void		Free() override;

protected:
	static TextureSource m_TextureSource;
	int			m_iBatchSize = 0;
	_float		m_fSpeed = 15.f;
};

