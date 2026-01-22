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
	void				Activate() override;
	void				Deactivate() override;

	virtual void		Reset();
	virtual void		ResetParticle(Particle* pParticle) PURE;
	virtual void		AddParticle();
	
	virtual void		SetPreRenderState();
	virtual void		SetPostRenderState();

	virtual _uint		GetTextureCnt() PURE;
	void				ChangeState(_uint _state);
public:
	bool				IsEmpty();
	bool				IsDead() override;

protected:
	virtual void		RemoveDeadParticles();
	virtual	HRESULT		Add_Component() PURE;
	void				Free() override;

	//UV 기준 offset 이동
	virtual void		SetNextUV(Particle* pParticle);
	//Frame 기준 이동 
	//Padding이 있는 텍스쳐는 아래 함수로 직접 넘겨주기 
	bool				SetNextFrame(Particle* pParticle, _vec2& _Idx);
public:
	void				SetPos(_vec3 _pos) { m_vPos = _pos; }
	_vec3				GetPos() { return m_vPos; }

	void				SetSize(_vec2 _size) { m_vSize = _size; }
	_vec2				GetSize() { return m_vSize; }

	void				SetVelocity(_vec3 _vel) { m_vVelocity = _vel; }
	_vec3				GetVelocity() { return m_vVelocity; }

	void				SetLifeTime(_float _lifeTime) { m_fLifeTime = _lifeTime; }
	_float				GetLifeTime() { return m_fLifeTime; }

	void				SetColor(D3DXCOLOR _color) { m_color = _color; }
	D3DXCOLOR			GetColor() { return m_color; }

	void				SetLoop(_bool _loop) { m_bLoop = _loop; }
	_bool				GetLoop() { return m_bLoop; }

	void				SetAnimSpeed(_float _speed) { m_fAnimSpeed = _speed; }
	_float				GetAnimSpeed() { return m_fAnimSpeed; }

	_int				GetState() { return m_iState; }

protected:
	vector<Particle>	m_Particles;			// 파티클 속성 리스트 
	list<Particle*>		m_ActiveList;

	D3DXVECTOR3			m_vOrigin;
	BoundingBox			m_pBoundingBox;			// 파티클의 경계상자
	int					m_iMaxParticle;			// 최대 파티클 수 

	CDVIBuffer*			m_pBufferCom;
	CTexture*			m_pTextureCom;
	TextureDesc*		m_pTextureDesc;

protected:
	//Particle Editor 용 
	_uint				m_iState;
	_vec3				m_vPos {};
	_vec2				m_vSize = { 1.f,1.f };
	_float				m_fAnimSpeed = 1.f;
	_vec3				m_vVelocity {};
	_float				m_fLifeTime = 1.f;
	D3DXCOLOR			m_color{};
	bool				m_bLoop = false;


};

END