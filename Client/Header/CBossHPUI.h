#pragma once
#include "CParticleEmitter.h"

class CBossHPUI :
	public Engine::CParticleEmitter
{
public:
	CBossHPUI(IDirect3DDevice9* device, CGameObject* _pOwner);
	virtual ~CBossHPUI();

	HRESULT		Ready_GameObject() override;
	_int		Update_GameObject(const _float& fTimeDelta);
	void		Render_GameObject() override;
	void		SetPreRenderState() override;
	void		SetPostRenderState() override;

public:
	void		Reset() override;
	//void		Deactivate() override;
	_uint		GetTextureCnt() override { return 1; }
protected:
	HRESULT		Add_Component() override;
	void		ResetParticle(Particle* particle) override;
public:
	static CBossHPUI* Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* _owner);
	static TextureSource GetTextureSource()
	{
		return m_TextureSource;
	}
protected:
	void		Free() override;
protected:

	_vec2		m_vFrame = { 0,0 };
	CGameObject* m_pOwner = nullptr;
	static TextureSource m_TextureSource;
	int			m_iBatchSize = 0;

	bool		m_bDecrease = false;
	_float		m_fOwnerHP = 0.f;;
	_float		m_fMaxHP = 0.f;
	_float		m_DistHp = 0.f;

	_int 		m_CurCnt = 15;

	_float		m_fEffectTime = 0.15;
	_float		m_fTime = 0.f;

	_vec3 randMove{};

};

