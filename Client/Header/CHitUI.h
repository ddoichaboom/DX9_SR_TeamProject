#pragma once
#include "CParticleEmitter.h"

class CHitUI :
	public Engine::CParticleEmitter
{
public:
	CHitUI(IDirect3DDevice9* device);
	virtual ~CHitUI();

	HRESULT		Ready_GameObject() override;
	_int		Update_GameObject(const _float& fTimeDelta);
	void		Render_GameObject() override;
	void		SetPreRenderState() override;
	void		SetPostRenderState() override;

public:
	void		Reset() override;
	_uint		GetTextureCnt() override { return 1; }
protected:
	HRESULT		Add_Component() override;
	void		ResetParticle(Particle* particle) override;
public:
	static CHitUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static TextureSource GetTextureSource()
	{
		return m_TextureSource;
	}
protected:
	void		Free() override;

protected:
	static TextureSource m_TextureSource;
	int			m_iBatchSize = 0;
	_vec2		m_vCurFrame;
};

