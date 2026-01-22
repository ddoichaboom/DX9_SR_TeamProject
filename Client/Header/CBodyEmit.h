#pragma once
#include "CParticleEmitter.h"
class CBodyEmit :
	public CParticleEmitter
{
public:
	enum PART_TYPE { MON_PART, GLASS_PART, END_PART };
public:
	CBodyEmit(IDirect3DDevice9* device);
	virtual ~CBodyEmit();

	HRESULT		Ready_GameObject() override;
	_int		Update_GameObject(const _float& fTimeDelta);
	void		Render_GameObject() override;
	void		SetPreRenderState() override;
	void		SetPostRenderState() override;

public:
	_uint		GetTextureCnt() override { return 1; }
	void		SetSpeed(_float _speed) { m_fSpeed = _speed; }
protected:
	HRESULT		Add_Component() override;
	void		ResetParticle(Particle* particle) override;
public:
	static CBodyEmit* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static vector<TextureSource>& GetTextureSources()
	{
		return m_TextureSources;
	}
protected:
	void		Free() override;

protected:
	static vector<TextureSource> m_TextureSources;
	int			m_iBatchSize = 0;
	_float		m_fSpeed = 40.f;
	const _float m_fGravity = 5.f;

};

