#pragma once
#include "CCharacter.h"
#include "CEventMgr.h"

namespace Engine
{
	class CCollider;
	class CRcTexUp;
}


class CLeftPart;
class CRightPart;
class CMiddlePart;
class CWeapon;
class CHitUI;
class CMonster;
class CSodaUI;
// πŸ≤„æﬂ«“∞≈∞∞±‰«‘
//class CShopBG;

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
	//πÊΩ¬»Ò √ﬂ∞°
	bool				CheckTakeDownMonster(CMonster** _pOut, CCollider** _pOutCollider);
	
	void				Move_Input(const _float& fTimeDelta, const _vec3& vRight, const _vec3& vLook);
	void				Action_Input(const _float& fTimeDelta, const _vec3& vLook);
	void				Gravity(const _float& fTimeDelta);
	_bool				CheckOnFloor(const _float& fTimeDelta,_float* pHeight);
	void				Set_OnFloor(const _float& fTimeDelta);

	_bool				Picking_OnFloor(_vec3* pHit, CRcTexUp* pFloorBufferCom, CTransform* pFloorTransformCom);
	_float				Compute_HeightOnFloor(const _vec3* pPos, const _vec3* pFloorVtxPos, const _ulong& dwCntX, const _ulong& dwCntZ);

	void				Update_Jump(const _float& fTimeDelta);
	void				Update_Dash(const _float& fTimeDelta);
	void				Update_SideDash(const _float& fTimeDelta);
	void				Update_TickDamagaed(const _float& fTimeDelta);

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
	void				TakeDown_Func();

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
	void				Set_BossStage(_bool bStage) { m_bBossStage = bStage; }
	_bool				Get_Jump() { return m_bJump; }
	_float				Get_Velocity() { return m_fVelocity; }
	_float				Get_JumpTime() { return m_fJumpTime; }

	void				Add_Item(COLLIDER_TAG eColliderTag);

public:
	_float				Get_HP() const { return m_fHP; }
	_float				Get_MaxHP() const { return m_fMaxHP; }

	wstring				Get_HPText() const 
	{
		_int iHP = static_cast<_int>(m_fHP);
		wstring wHP = to_wstring(iHP);
		return wHP;
	}
	
	wstring				Get_HPPercent() const
	{
		_tchar buffer[64];

		_uint iPercent = static_cast<_uint>((m_fHP / m_fMaxHP) * 100.f);

		swprintf_s(buffer, L"%02d", iPercent);

		return wstring(buffer);

	}


	void				Add_HP(_float fHp)
	{
		if (fHp > 0)
		{
			m_fHP = min(m_fHP + fHp, m_fMaxHP);
			m_fTime = 0.f;
		}
		else
		{
			m_fHP = max(m_fHP + fHp, 0);
			m_fTime = 0.f;
		}
	}

	void			Get_Hit(_float fDamage);


protected:
	virtual void	Free();
	void			OnCollision(CollisionInfo info);

protected:
	void			CreateSoda();
	void			CreateAxe();
	void			CreateExtinguisher();

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


	_float	m_fHP;
	_float	m_fMaxHP;

	_bool	m_bSlope;
	_bool	m_bSideDash;

	_float	m_fTime;
	_float  m_fStageTime;
	_bool	m_bStage;

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

	//πÊΩ¬»Ò √ﬂ∞° ¿Ã∆Â∆Æ
	CHitUI* m_pHitUI;
	CSodaUI* m_pSodaUI;
	//πÊΩ¬»Ò √ﬂ∞° 
	bool m_bTakeDown;
	CMonster* m_pTakeDownObject; 
	CCollider* m_pTakeDownCollider;


	CGameObject* m_pColHitObj;
	_vec3		 m_vDiffDir;

	_bool		m_bMoveStop;
	_bool		m_bAbleTakeDown;

	_bool		m_bDelay;
	_float		m_fDelayTime;

	_bool		m_bBossStage;


	static wstring	szTutorialBGM;
	static wstring	szStageBGM;
	static wstring	szBossBGM;
	static wstring	szClearSFX;
	
};

