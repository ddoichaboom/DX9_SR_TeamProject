#pragma once
#include "CParticleEmitter.h"

enum HP_TYPE { HP_GOOD, HP_NORMAL, HP_WARNING, HP_DANAGER, HP_DEAD, HP_END };

class CPhoneHPUI :
	public Engine::CParticleEmitter
{

public:
	CPhoneHPUI(IDirect3DDevice9* device, CGameObject* _pOwner);
	virtual ~CPhoneHPUI();

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
	static CPhoneHPUI* Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* _owner);

protected:
	void		Free() override;
	HP_TYPE		GetHPType(_float _hp);
protected:
	int			m_iBatchSize = 0;
	CGameObject* m_pOwner = nullptr;

	bool		m_bDecrease = false;
	_float		m_fOwnerHP = 0.f;
	_float		m_fMaxHP = 0.f;

	_float		m_fEffectTime = 3.f;
	_float		m_fTime = 0.f;
	HP_TYPE		m_type = HP_END;
	_float		m_fRatio = 1.f;

	_float		m_HPMinRatio[HP_END] = { 80.f, 50.f,30.f, 0.f, -1.f };
	D3DXCOLOR	m_HPColors[HP_END] =
	{
		{0.3f, 1.0f, 0.2f, 0.5f},
		{0.8f, 1.0f, 0.4f, 0.5f},
		{0.9f, 1.0f, 0.45f,0.5f},
		{1.0f, 0.7f, 0.4f, 0.5f},
		{1.0f, 0.2f, 0.2f, 0.5f},
	};

};

