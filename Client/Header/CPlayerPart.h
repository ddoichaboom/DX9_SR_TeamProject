#pragma once
#include "CCharacter.h"
#include "Engine_Define.h"

namespace Engine
{
	class CAnimation;
}


class CPlayerPart : public CCharacter   
{


protected :
	explicit	CPlayerPart(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CPlayerPart(const CPlayerPart& rhs);
	virtual		~CPlayerPart();

public :
	virtual		void	Change_State(_uint iStateNum) PURE;
	void	Set_WeaponState(_uint eState);
	bool	IsAnimationEnd();

public :
	virtual		HRESULT	Ready_GameObject() PURE;
	virtual		_int	Update_GameObject(const _float& fTimeDelta) PURE;
	virtual		void	LateUpdate_GameObject(const _float& fTimeDelta) PURE;
	virtual		void	Render_GameObject() PURE;

public :
	virtual		HRESULT	Add_Component()	PURE;
	
protected :
	virtual		void	Free() PURE;

protected :
	_uint m_eWeaponState;
	Engine::CAnimation* m_pAnimationCom;
	_vec3	m_vStartPos;



};

