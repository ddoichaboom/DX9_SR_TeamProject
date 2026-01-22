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
	_uint		GetTextureCnt() override
	{
		return (_uint)m_TextureSources.size();
	}
protected:
	HRESULT		Add_Component() override;
	void		AddParticle() override;
	void		ResetParticle(Particle* particle, int idx);
	void		ResetParticle(Particle* particle) override;
public:
	static CExplosion* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static vector<TextureSource>& GetTextureSources()
	{
		return m_TextureSources;
	}
protected:
	void		Free() override;

protected:
	static vector<TextureSource> m_TextureSources;
	int			m_iBatchSize = 0;

	TextureDesc* m_SubTextureDesc = nullptr;
	_int		m_iColorIdx;
	D3DXCOLOR	m_RedColor = { 0.9f, 0.3f, 0.1f, 0.1f };
	D3DXCOLOR	m_GrayColor = { 0.2f, 0.2f, 0.2f, 0.6f };

};

