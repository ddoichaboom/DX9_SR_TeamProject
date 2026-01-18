#pragma once
#include "Engine_Define.h"
#include "CGameObject.h"

BEGIN(Engine)

class CDVIBuffer;
class CTransform;
class CTexture;

class ENGINE_DLL CParticleEmitter : public CGameObject
{
public:
	CParticleEmitter(IDirect3DDevice9* device, _int _maxParticle);
	virtual ~CParticleEmitter();

	virtual HRESULT		Ready_GameObject();
	virtual _int		Update_GameObject(const _float& fTimeDelta);
	virtual void		Render_GameObject();

public:
	virtual void		Reset();
	virtual void		ResetParticle(Particle* pParticle) PURE;
	virtual void		AddParticle();
	
	virtual void		SetPreRenderState();
	virtual void		SetPostRenderState();

public:
	bool				IsEmpty();
	bool				IsDead() override;

protected:
	virtual void		RemoveDeadParticles();
	virtual	HRESULT		Add_Component() PURE;
	void				Free() override;

protected:
	vector<Particle>	m_Particles;		// 파티클 속성 리스트 
	list<Particle*>		m_ActiveList;

	D3DXVECTOR3			m_vOrigin;
	//_float m_fEmitRate;				// 새로운 파티클이 추가되는 비율
	float				m_fSize;
	BoundingBox			m_pBoundingBox;			// 파티클의 경계상자
	int					m_iMaxParticle;				// 최대 파티클 수 

	CDVIBuffer*			m_pBufferCom;
	CTexture*			m_pTextureCom;

};

END