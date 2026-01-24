#pragma once
#include "CParticleEmitter.h"
class CExplosion :
	public CParticleEmitter
{
public:
	CExplosion(IDirect3DDevice9* device);
	virtual ~CExplosion();

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
	void		AddParticle() override;
	void		ResetParticle(Particle* particle, int idx);
	void		ResetParticle(Particle* particle) override;
public:
	static CExplosion* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static TextureSource& GetTextureSource()
	{
		return m_TextureSource;
	}
protected:
	void		Free() override;

protected:
	static TextureSource m_TextureSource;
	int			m_iBatchSize = 0;

	D3DXCOLOR	m_RedColor = { 0.9f, 0.3f, 0.1f, 0.1f };
	//D3DXCOLOR	m_GrayColor = { 0.18f, 0.18f, 0.18f, 0.75f };
	D3DXCOLOR	m_GrayColor = { 0.15f, 0.15f, 0.15f, 0.75f };
};

