#pragma once
#include "CParticleEmitter.h"

class CBossTrail :
    public CParticleEmitter
{
public:
	CBossTrail(IDirect3DDevice9* device);
	virtual ~CBossTrail();

	HRESULT		Ready_GameObject() override;
	_int		Update_GameObject(const _float& fTimeDelta);
	void		Render_GameObject() override;
	void		SetPreRenderState() override;
	void		SetPostRenderState() override;

public:
	void		Reset() override;
	_uint		GetTextureCnt() override { return 1; }
	void		SetOwnerTransform(CTransform* _ownerTrans)
	{
		m_pOwnerTransform = _ownerTrans;
	}

protected:
	HRESULT		Add_Component() override;
	void		ResetParticle(Particle* particle) override;

public:
	static CBossTrail* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static TextureSource& GetTextureSource()
	{
		return m_TextureSource;
	}
protected:
	void		Free() override;

protected:
	static TextureSource m_TextureSource;
	CTransform* m_pOwnerTransform;
	_float		m_fCreateTime = 0.f;
	_float		m_fTime = 0.f;
	int			m_iBatchSize = 0;

	_float		m_fFadeSpeed;
	_vec3 m_vDebugPos = { 0,5,0 };

	D3DXCOLOR m_Colors[3] =
	{
		{0.8f, 0.f,0.f,1.f},{0.f, 0.8f,0.f,1.f},{0.f, 0.f,0.8f,1.f}
	};
	int m_iIdx = 0;

};

