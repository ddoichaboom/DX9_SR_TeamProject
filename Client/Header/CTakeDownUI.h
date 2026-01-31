#pragma once
#include "CParticleEmitter.h"

class CTakeDownUI :
	public Engine::CParticleEmitter
{
public:
	CTakeDownUI(IDirect3DDevice9* device);
	virtual ~CTakeDownUI();

	HRESULT		Ready_GameObject() override;
	_int		Update_GameObject(const _float& fTimeDelta);
	void		Render_GameObject() override;
	void		SetPreRenderState() override;
	void		SetPostRenderState() override;

public:
	void		SetPos(_vec3 _pos) override;
	void		Reset() override;
	//void		Deactivate() override;
	_uint		GetTextureCnt() override { return 1; }
protected:
	HRESULT		Add_Component() override;
	void		ResetParticle(Particle* particle) override;
public:
	static CTakeDownUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static TextureSource GetTextureSource()
	{
		return m_TextureSource;
	}
protected:
	void		Free() override;
protected:
	static TextureSource m_TextureSource;
	int			m_iBatchSize = 0;
	_float		m_fEffectTime = 0.3f;
	_float		m_fTime;

	enum { COLOR_CNT = 3 };
	const D3DXCOLOR	m_RGBs[COLOR_CNT] =
	{
		{1,0,0,1}, {0,1,0,1},{0,0,1,1}
	};

	D3DXCOLOR	m_startColor{};
	D3DXCOLOR	m_endColor{};

	_int		m_iColorIdx = 0;

};

