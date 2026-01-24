#pragma once
#include "CParticleEmitter.h"
class CToonFlash :
	public CParticleEmitter
{
public:
	CToonFlash(IDirect3DDevice9* device);
	virtual ~CToonFlash();

	HRESULT		Ready_GameObject() override;
	_int		Update_GameObject(const _float& fTimeDelta);
	void		Render_GameObject() override;
	void		SetPreRenderState() override;
	void		SetPostRenderState() override;

public:
	_uint		GetTextureCnt() override { return 1; }
	void		SetFlashPos(_vec3 _pos);
protected:
	HRESULT		Add_Component() override;
	void		ResetParticle(Particle* particle) override;
public:
	static CToonFlash* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static TextureSource& GetTextureSource()
	{
		return m_TextureSource;
	}
protected:
	void		Free() override;

protected:
	static TextureSource m_TextureSource;
	int			m_iBatchSize = 0;

	D3DXCOLOR	m_Color[2] = { {0.8f,0.f,0.f,1.f}, {0.8f,0.8f,0.f,0.8f} };
	_int		m_curColorIdx;

	bool		m_bFlag = false;

};

