#pragma once
#include "CCamera.h"
#include "Engine_Define.h"
#include "CEventMgr.h"

namespace Engine
{
	class CTransform;
}


class CFirstCamera : public CCamera, public IListener
{
private:
	explicit CFirstCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CFirstCamera(const CFirstCamera& rhs);
	virtual ~CFirstCamera();

public : 
	HRESULT		Set_Transform(INFO eInfo, _vec3* pVector);

	void		Set_CamSetting();
	void		OnEvent(EVENT_TYPE _type, EventData* _pData) override;
	void		SetRot(ROTATION _Axis, float _degree) override;

public:
	HRESULT		Ready_GameObject(const _vec3* pEye,
		const _vec3* pAt,
		const _vec3* pUp,
		const _float& fFov,
		const _float& fAspect,
		const _float& fNear,
		const _float& fFar);

	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject() {}

private:
	HRESULT		Add_Component();

	void		Key_Input(const _float& fTimeDelta);
	void		Mouse_Move();
	void		Mouse_Fix();

	void		Cam_Shake(const _float& fTimeDelta);

public:
	static CFirstCamera* Create(LPDIRECT3DDEVICE9 pGraphicDev,
		const _vec3* pEye,
		const _vec3* pAt,
		const _vec3* pUp,
		const _float& fFov = D3DXToRadian(60.f),
		const _float& fAspect = (_float)WINCX / WINCY,
		const _float& fNear = 0.1f,
		const _float& fFar = 1000.f);

	void Set_BossStage(_bool bBoss) { m_bBossStage = bBoss; }

private:
	virtual void Free();


private :
	Engine::CTransform* m_pTransformCom;

private:
	_float		m_fSpeed;
	_bool		m_bFix;
	_bool		m_bCheck;
	_bool		m_bStage;


	_float		m_fPitch;
	_float		m_fYaw;

	_float		m_fShakeTime;
	_float		m_fShakeSpeed;
	_float		m_fShakePower;
	_bool		m_bBossStage;
};

