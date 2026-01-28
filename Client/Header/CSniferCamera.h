#pragma once
#include "CCamera.h"
#include "Engine_Define.h"

namespace Engine
{
	class CTransform;
}
class CSniferCamera : public CCamera
{
private:
	explicit CSniferCamera(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* _owner);
	virtual ~CSniferCamera();

public:
	HRESULT						Ready_GameObject(const _vec3* pEye,
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
	HRESULT						Add_Component();

public:
	_float						Get_Rot(ROTATION rot);
	void						Set_Rot(ROTATION rot, _float _angle);
	void						Set_AngleLimit(ROTATION rot, _float minAngle, _float maxAngle);
public:
	static CSniferCamera*		Create(LPDIRECT3DDEVICE9 pGraphicDev,
		CGameObject* _owner,
		const _vec3* pEye,
		const _vec3* pAt,
		const _vec3* pUp,
		const _float& fFov = D3DXToRadian(60.f),
		const _float& fAspect = (_float)WINCX / WINCY,
		const _float& fNear = 0.1f,
		const _float& fFar = 1000.f);
private:
	virtual void				Free();

private:
	CGameObject*		m_pOwner;
	Engine::CTransform* m_pTransformCom;
	Engine::CTransform* m_pOwnerTransformCom;
	_float				m_fSpeed;

	_vec2				m_vAngleLimit[ROT_END]{};
};

