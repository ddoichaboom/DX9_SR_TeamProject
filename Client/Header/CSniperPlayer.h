#pragma once
#include "CCharacter.h"

//#include "CEventMgr.h"

class CSniperUI;
class CHitUI;
class CSniferCamera;
class CSRightHand;
class CLeftPart;

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
	bool			Get_IsAimState();

protected:
	void			ChangeState(_uint nextStateID) override;

	void			Intro_Begin();
	void			Intro();

	void			Idle();

	void			ZoomIn();
	void			Attack();
	void			ZoomOut();

	void			Shoot();
protected:
	void			Free() override;
	void			OnCollision(CollisionInfo info);
	void			CheckedPickedMonster();
	void			CheckedPicked(const _tchar* layerName, OBJ_ID eID);
protected:
	CSRightHand*	m_pRightHand = nullptr;
	CLeftPart*		m_pLeftHand = nullptr;

protected:
	_float			m_fZoomFOV = 8.f;

	_float			m_fMouseSpeed = 0.f;;
	_float			m_fBaseMouseSpeed = 0.003f;
	_float			m_fZoomMouseSpeed = 0.003f * 0.25f;

	CSniferCamera*	m_pCamera;
	SNIFER_STATE	m_eCurState = SN_NONE;
	CCollider*		m_pMainCollider;
	const	_tchar* m_szMainColliderName = L"ColMain";

	bool			m_bCanInput = false;
	_bool			m_bShoot = false;
//Zoom	
	bool			m_bZoom = false;


protected:
	CHitUI*			m_pHitUI;
	CSniperUI*		m_pSniperUI;

	bool			m_bRenderStop = false;

public:
	static wstring szReloadSFX;
	static wstring szShotSFX;
	static wstring szSniperMapBGM;
	


};

