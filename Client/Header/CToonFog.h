#pragma once
#include "CParticleEmitter.h"
class CToonFog :
	public CParticleEmitter
{
public:
	CToonFog(IDirect3DDevice9* device);
	virtual ~CToonFog();

	HRESULT		Ready_GameObject() override;
	_int		Update_GameObject(const _float& fTimeDelta);
	void		Render_GameObject() override;
	void		SetPreRenderState() override;
	void		SetPostRenderState() override;

public:
	void		Reset() override;
	_uint		GetTextureCnt() override { return 1; }
	void		SetDirection(_vec3 _dir)
	{
		m_vVelocity = _dir;
	}
	void		SetOwnerTransform(CTransform* _ownerTrans)
	{
		m_pOwnerTransform = _ownerTrans;
	}
	void		SetFogCreateTime(_float _time)
	{
		m_fFogCreateTime = _time;
	}
protected:
	void		Set_Pos(_vec3 _pos);
	HRESULT		Add_Component() override;
	void		ResetParticle(Particle* particle) override;
public:
	static CToonFog* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static TextureSource& GetTextureSource()
	{
		return m_TextureSource;
	}
protected:
	void		Free() override;

protected:
	CTransform* m_pOwnerTransform;
	static TextureSource m_TextureSource;
	_float		m_fFogCreateTime = 0.f;
	_float		m_fTime = 0.f;
	int			m_iBatchSize = 0;

	_vec3 m_vDebugPos = { 0,5,0 };

};

