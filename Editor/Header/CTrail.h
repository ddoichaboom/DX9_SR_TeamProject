#pragma once
#include "CParticleEmitter.h"
class CTrail :
    public CParticleEmitter
{
public:
	CTrail(IDirect3DDevice9* device);
	virtual ~CTrail();

	HRESULT		Ready_GameObject() override;
	_int		Update_GameObject(const _float& fTimeDelta);
	void		Render_GameObject() override;
	void		SetPreRenderState() override;
	void		SetPostRenderState() override;

public:
	_uint		GetTextureCnt() override { return 1; }
	_vec3		GetStartPos() { return m_vStartPos; }
	_vec3		GetEndPos() { return m_vEndPos; }
	void		SetTrailPos(_vec3 _start, _vec3 _end);
protected:
	HRESULT		Add_Component() override;
	void		ResetParticle(Particle* particle) override;
public:
	static CTrail* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static TextureSource& GetTextureSource()
	{
		return m_TextureSource;
	}
protected:
	void		Free() override;

protected:
	static TextureSource m_TextureSource;
	int			m_iBatchSize = 0;

	_vec3 m_vStartPos{};
	_vec3 m_vEndPos{};
};

