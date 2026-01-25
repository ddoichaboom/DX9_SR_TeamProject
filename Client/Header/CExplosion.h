#pragma once
#include "CParticleEmitter.h"

//Size 조정 : size를 비율로 잡음. 1이 기본, 비율로 생각하고 SetSize하기 
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
	void		Deactivate() override;
	void		Reset() override;
	_uint		GetTextureCnt() override { return 1; }
protected:
	HRESULT		Add_Component() override;
	void		AddParticle() override;
	void		ResetParticle(Particle* particle, int idx);
	void		ResetParticle(Particle* particle) override;
public:
	static CExplosion* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static TextureSource& GetTextureSource()
	{
		return m_TextureSource;
	}
protected:
	void		Free() override;

protected:
	static TextureSource m_TextureSource;
	int			m_iBatchSize = 0;

	D3DXCOLOR	m_RedColor = { 0.9f, 0.3f, 0.1f, 0.1f };
	//D3DXCOLOR	m_GrayColor = { 0.18f, 0.18f, 0.18f, 0.75f };
	D3DXCOLOR	m_GrayColor = { 0.15f, 0.15f, 0.15f, 0.75f };

	_vec2 vRedPosXOffset = { -5.f, 5.f };
	_vec2 vRedPosYOffset = { -5.f, 5.f };
	_vec2 vRedSizeXOffset = { 10.f, 20.f };
	_vec2 vRedSpeedOffset = { 0.7f, 1.5f};

	_vec2 vGrayPosXOffset = { -8.f, 8.f };
	_vec2 vGrayPosYOffset = { -8.f, 8.f };
	_vec2 vGraySizeXOffset = { 15.f, 25.f };
	_vec2 vGraySpeedOffset = { 2.f, 3.f };

};

