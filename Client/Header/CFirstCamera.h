#pragma once
#include "CCamera.h"
#include "Engine_Define.h"

namespace Engine
{
	class CTransform;
}


class CFirstCamera : public CCamera
{
private:
	explicit CFirstCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CFirstCamera(const CFirstCamera& rhs);
	virtual ~CFirstCamera();

public : 
	HRESULT		Set_Transform(INFO eInfo, _vec3* pVector);

	void		Set_CamSetting();


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

public:
	static CFirstCamera* Create(LPDIRECT3DDEVICE9 pGraphicDev,
		const _vec3* pEye,
		const _vec3* pAt,
		const _vec3* pUp,
		const _float& fFov = D3DXToRadian(60.f),
		const _float& fAspect = (_float)WINCX / WINCY,
		const _float& fNear = 0.1f,
		const _float& fFar = 1000.f);

private:
	virtual void Free();


private :
	Engine::CTransform* m_pTransformCom;

private:
	_float		m_fSpeed;
	_bool		m_bFix;
	_bool		m_bCheck;


	// Transform 추가해보자
};

