#pragma once
#include "CParticleEmitter.h"
class CFlare :
    public CParticleEmitter
{
public:
	CFlare(IDirect3DDevice9* device);
	virtual ~CFlare();

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
	static CFlare* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static TextureSource& GetTextureSource()
	{
		return m_TextureSource;
	}
protected:
	void		Free() override;

protected:
	static TextureSource m_TextureSource;
	int			m_iBatchSize = 0;

	D3DXCOLOR	m_StartColor = { 1.0f, 0.2f, 0.0f, 1.0f };
	D3DXCOLOR	m_EndColor = { 0.2f, 0.2f, 0.2f, 0.0f };
	_float		m_fFlareTime;

};

