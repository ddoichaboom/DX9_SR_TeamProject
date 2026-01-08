#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CStateComponent;
	class CCollision;
}

class CCharacter :
    public CGameObject
{
protected:
	explicit			CCharacter(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit			CCharacter(const CCharacter& rhs);
	virtual				~CCharacter();

public:
	HRESULT				Ready_GameObject() override;
	_int				Update_GameObject(const _float& fTimeDelta) override;
	void				LateUpdate_GameObject(const _float& fTimeDelta) override;
	void				Render_GameObject() PURE;



protected:
	HRESULT				Add_Component() override;
	//void				Set_OnTerrain();

protected:
	void				Free() override;
	virtual void		ChangeState(_uint nextStateID) {};

public:
	void				Activate() override;
	void				Deactivate() override;
	void				SetPos(_vec3 _pos) override;
	void				Rotate(ROTATION _Axis, _float _degree)  override;

public:

	void				Set_Jump(_bool bJump) { m_bJump = bJump; }
	void				Set_Velocity(_float fVelocity) { m_fVelocity = fVelocity; }
	void				Set_JumpTime(_float fJumpTime) { m_fJumpTime = fJumpTime; }

	_bool				Get_Jump() { return m_bJump; }
	_float				Get_Velocity() { return m_fVelocity; }
	_float				Get_JumpTime() { return m_fJumpTime; }

protected :
	void				Gravity(const _float& fTimeDelta);
	void				Set_OnFloor(const _float& fTimeDelta);
	_bool				Get_OnFloor();

	void				Update_Jump(const _float& fTimeDelta);
	void				Update_Dash(const _float& fTimeDelta);


protected:
	Engine::CRcTex*		m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture*	m_pTextureCom;
	Engine::CStateComponent* m_pStateCom;
	Engine::CCollision* m_pCollisionCom;

protected:
	float m_fTime;

	_bool	m_bFall;
	_float	m_fVelocity;
	
	_bool	m_bJump;
	_float	m_fJumpStartY;
	_float	m_fJumpTime;
	_float	m_fJumpDuration;
	_float	m_fJumpHeight;

	_bool	m_bDash;
	_vec3	m_vDashStart;
	_vec3	m_vDashDir;
	_float	m_fDashTime;
	_float	m_fDashDuration;
	_float	m_fDashDistance;

};

