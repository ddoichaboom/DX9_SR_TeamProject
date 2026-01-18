#pragma once
#include "CCharacter.h"
#include "CEventMgr.h"

namespace Engine
{
	class CCollider;
}


class CLeftPart;
class CRightPart;
class CMiddlePart;
class CWeapon;

// 바꿔야할거같긴함
class CShopBG;

class CPlayer : public CCharacter, public IListener
{
public:

protected:
	explicit		CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit		CPlayer(const CPlayer& rhs);
	virtual			~CPlayer();

public:
	static _uint GetStateID(_byte _state, _byte _subState)
	{
		return ((_uint)_subState << 4) | (_uint)_state;
	}
	_byte Get_WeaponState() { return (_byte)m_eWeaponState; }

protected :
	void        OnEvent(EVENT_TYPE _type, EventData* _pData) override;

public:
	HRESULT			Ready_GameObject() override;
	_int			Update_GameObject(const _float& fTimeDelta) override;
	void			LateUpdate_GameObject(const _float& fTimeDelta) override;
	void			Render_GameObject() override;

private:
	HRESULT				Add_Component() override;
	HRESULT				Add_PlayerPart();
	void				Key_Input(const _float& fTimeDelta);
	void				CheckPickedMonster();
	void				CheckKickedMonster(COLLIDER_TAG eTag, _float fAttack);
	void				CheckEnterCollider();
	
	void				Move_Input(const _float& fTimeDelta, const _vec3& vRight, const _vec3& vLook);
	void				Action_Input(const _float& fTimeDelta, const _vec3& vLook);
	void				Gravity(const _float& fTimeDelta);
	_bool				CheckOnFloor(_float* pHeight);
	void				Set_OnFloor(const _float& fTimeDelta);
	_bool				Get_OnFloor();

	void				Update_Jump(const _float& fTimeDelta);
	void				Update_Dash(const _float& fTimeDelta);

	void				Move_ByCollision(COL_DIR& dir, _vec3 _diff) override;
public:	
	void				Intro_Func();
	void				Fire_Func();
	void				Katana_Func();
	void				Reload_Func();
	void				Kick_Func();
	void				Slide_Func();
	void				Shop_Func();
	void				Drink_Func();

public :
	void				Change_State(_uint eState);
	
private :
	void				State_Enter();
	void				State_Update(const _float& fTimeDelta);
	void				State_LateUpdate(const _float& fTimeDelta);
	void				State_Exit();

private :
	void				Intro_Enter();
	void				Intro_Update(const _float& fTimeDelta);
	void				Intro_LateUpdate(const _float& fTimeDelta);
	void				Intro_Exit();

	void				Idle_Enter();
	void				Idle_Update(const _float& fTimeDelta);
	void				Idle_LateUpdate(const _float& fTimeDelta);
	void				Idle_Exit();

	void				Reload_Enter();
	void				Reload_Update(const _float& fTimeDelta);
	void				Reload_LateUpdate(const _float& fTimeDelta);
	void				Reload_Exit();

	void				Attack_Enter();
	void				Attack_Update(const _float& fTimeDelta);
	void				Attack_LateUpdate(const _float& fTimeDelta);
	void				Attack_Exit();

	void				Kick_Enter();
	void				Kick_Update(const _float& fTimeDelta);
	void				Kick_LateUpdate(const _float& fTimeDelta);
	void				Kick_Exit();

	void				Drink_Enter();
	void				Drink_Update(const _float& fTimeDelta);
	void				Drink_LateUpdate(const _float& fTimeDelta);
	void				Drink_Exit();

	void				Slide_Enter();
	void				Slide_Update(const _float& fTimeDelta);
	void				Slide_LateUpdate(const _float& fTimeDelta);
	void				Slide_Exit();

	void				Shop_Enter();
	void				Shop_Update(const _float& fTimeDelta);
	void				Shop_LateUpdate(const _float& fTimeDelta);
	void				Shop_Exit();


	void				Next_Enter();
	void				Next_Update(const _float& fTimeDelta);
	void				Next_LateUpdate(const _float& fTimeDelta);
	void				Next_Exit();

public:
	static CPlayer*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CPlayer*		Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot = { 0.f, 0.f, 1.f }, _vec3 vScale = { 6.f, 6.f, 1.f });

	void				Add_Weapon(_byte eWeaponTag, CWeapon* pWeapon);
	void				Change_Weapon(_byte eWeaponTag);
	void				Set_LeftPart(CLeftPart* pLeft);
	void				Set_RightPart(CRightPart* pRight);
	void				Set_MiddlePart(CMiddlePart* pMiddle);
	void				Set_Jump(_bool bJump) { m_bJump = bJump; }
	void				Set_Velocity(_float fVelocity) { m_fVelocity = fVelocity; }
	void				Set_JumpTime(_float fJumpTime) { m_fJumpTime = fJumpTime; }

	_bool				Get_Jump() { return m_bJump; }
	_float				Get_Velocity() { return m_fVelocity; }
	_float				Get_JumpTime() { return m_fJumpTime; }

protected:
	virtual void	Free();
	void			OnCollision(CollisionInfo info);

private:
	CLeftPart* m_pLeftPart;
	CRightPart* m_pRightPart;
	CMiddlePart* m_pMiddlePart;

	PLAYER_STATE		m_eNowState;
	WEAPON_STATE	m_eWeaponState;
	unordered_map<WEAPON_STATE, CWeapon*> m_mapWeapon;

	CCollider* m_pMainCollider;
	const	_tchar* m_szMainColliderName = L"ColMain";

	CCollider*			m_pKickCollider;
	const	_tchar*		m_szKickColliderName = L"ColKick";

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


	_float	m_fMoveSpeed;
	_float	m_fKickAttack;	
	_bool	m_bOnCollision;

	map<_uint, _int> m_mapCallCnt = {};
	//_int	m_iCallCnt;	


	//UI
	//CShopBG*	m_pShopBG;
};

