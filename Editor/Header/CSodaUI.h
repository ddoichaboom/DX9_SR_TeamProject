#pragma once
#include "CParticleEmitter.h"

class CSodaUI :
	public Engine::CParticleEmitter
{
public:
	CSodaUI(IDirect3DDevice9* device);
	virtual ~CSodaUI();

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
	static CSodaUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static TextureSource GetTextureSource()
	{
		return m_TextureSource;
	}
protected:
	void		Free() override;

protected:
	static TextureSource m_TextureSource;
	int			m_iBatchSize = 0;

	_float		m_fTime = 0.f;
	_float		m_fSpeed = 0.f;
	_float		m_fLerpStartTime = 0.f;
	_float		m_fCreateTime = 0.f;
	D3DXCOLOR	m_vStartColor = { 1,1,1,1 };
	D3DXCOLOR	m_vEndColor = { 0,1,0, 0.f};

	_int		m_iIdx = 0;
};

