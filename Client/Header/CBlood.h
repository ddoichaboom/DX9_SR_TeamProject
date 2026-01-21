#pragma once
#include "CParticleEmitter.h"
class CBlood :
    public CParticleEmitter
{
    enum BLOOD_TYPE { BLOOD1, BLOOD2, BLOOD3, BLOOD_END };
public:
	CBlood(IDirect3DDevice9* device);
	virtual ~CBlood();

	HRESULT		Ready_GameObject() override;
	_int		Update_GameObject(const _float& fTimeDelta);
	void		Render_GameObject() override;
	void		SetPreRenderState() override;
	void		SetPostRenderState() override;

public:
	void		Deactivate() override;
	_uint		GetTextureCnt() override
	{
		return (_uint)m_TextureSources.size();
	}
protected:
	HRESULT		Add_Component() override;
	void		ResetParticle(Particle* particle) override;
public:
	static CBlood* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static vector<TextureSource>& GetTextureSources()
	{
		return m_TextureSources;
	}
	static Particle m_ParticleInfo[BLOOD_END];
protected:
	void		Free() override;

protected:
	static vector<TextureSource> m_TextureSources;
	int			m_iBatchSize = 0;
	_float		m_fTime = 0.f;

};

