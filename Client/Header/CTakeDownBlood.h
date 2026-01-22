#pragma once
#include "CParticleEmitter.h"
class CTakeDownBlood :
	public CParticleEmitter
{
	enum BloodType { BLOOD1, BLOOD2, BLOOD_END };
public:
	CTakeDownBlood(IDirect3DDevice9* device);
	virtual ~CTakeDownBlood();

	HRESULT		Ready_GameObject() override;
	_int		Update_GameObject(const _float& fTimeDelta);
	void		Render_GameObject() override;
	void		SetPreRenderState() override;
	void		SetPostRenderState() override;

public:
	void		Deactivate() override;
	_uint		GetTextureCnt() override { return 1; }
	void		Reset() override;
protected:
	void		SetNextUV(Particle* pParticle) override;
	HRESULT		Add_Component() override;
	void		ResetParticle(Particle* particle) override;
public:
	static CTakeDownBlood* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static TextureSource GetTextureSource()
	{
		return m_TextureSource;
	}
protected:
	void		Free() override;

protected:
	static TextureSource m_TextureSource;
	int			m_iBatchSize = 0;

	const _vec2 m_StartFrame[BLOOD_END] = { {0,0} , {0,3} };
	const _vec2 m_EndFrame[BLOOD_END] = { {2,2} , {5,2} };

	_vec3 m_vDiffOffset = { 6,3,0 };
	_vec3 m_vDiffSum = { 0,0,0 };
	//_vec3 m_vBlood2PosOffset = { 30.f, -10.f,0.f };
	//_vec3 m_vSecondPosOffset = { 50.f,50.f ,0.f };

	_vec3 m_vBlood2PosOffset = { 30.f, -30.f,0.f };
	_vec3 m_vSecondPosOffset = { -100.f,60.f ,0.f };

	bool m_bSecondHit = false;

	_float m_fTime = 0.f;
	_float m_fSecondTime = 0.5f;
	_int m_iBloodType = BLOOD1;
};

