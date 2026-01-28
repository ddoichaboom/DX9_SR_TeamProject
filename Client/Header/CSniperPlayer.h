#pragma once
#include "CCharacter.h"
//#include "CEventMgr.h"

class CHitUI;
class CSniferCamera;
class CSRightHand;
class CSLeftHand;

namespace Engine
{
	class CAnimation;
}

class CSniperPlayer :
    public CCharacter
{
protected:
	enum SNIFER_STATE { SN_NONE, SN_INTRO, SN_IDLE, SN_ATTACK, SN_DEAD, SN_END};
public:
	explicit		CSniperPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit		CSniperPlayer(const CSniperPlayer& rhs);
	virtual			~CSniperPlayer();

public:
	HRESULT			Ready_GameObject() override;
	_int			Update_GameObject(const _float& fTimeDelta) override;
	void			LateUpdate_GameObject(const _float& fTimeDelta) override;
	void			Render_GameObject() override;

protected:
	HRESULT			Add_Component() override;
	void			Key_Input(const _float& fTimeDelta);

protected:
	static void		CreateStateData();
public:
	static CSniperPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CSniperPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

public:
	bool			CanInput() { return m_bCanInput; }
	void			EnableInput() { m_bCanInput = true; }
	void			DisableInput() { m_bCanInput = false; }

protected:
	void			Change_State(_uint eState);

	void			Intro_Begin();
	void			Intro();

	void			Idle();

	void			ZoomIn();
	void			Attack();
	void			ZoomOut();

protected:
	void			Free() override;
	void			OnCollision(CollisionInfo info);

protected:
	CSRightHand*	m_pRightHand = nullptr;
	CSLeftHand*		m_pLeftHand = nullptr;
protected:
	_float			m_fMouseSpeed = 0.003f;
	CSniferCamera*	m_pCamera;
	SNIFER_STATE	m_eCurState = SN_NONE;
	CCollider*		m_pMainCollider;
	const	_tchar* m_szMainColliderName = L"ColMain";

	bool			m_bCanInput = false;
	
//Zoom	
	bool			m_bZoom = false;


protected:
	CHitUI* m_pHitUI;
};

