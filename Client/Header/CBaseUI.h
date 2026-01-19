#pragma once
#include "CGameObject.h"

class CBaseUI : public CGameObject    
{
protected :
	explicit	CBaseUI(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CBaseUI(const CBaseUI& rhs);
	virtual		~CBaseUI();

public :
	virtual		HRESULT		Ready_GameObject()	override;
	virtual		_int		Update_GameObject(const _float& fTimeDelta)	override;
	virtual		void		LateUpdate_GameObject(const _float& fTimeDelta)	override;
	virtual		void		Render_GameObject()	PURE;

protected :
	virtual		HRESULT		Add_Component();
	virtual		void		Free();

public :
	virtual void            Rotate(ROTATION eType, const _float& fAngle) {};
	virtual void            SetPos(_vec3 _pos) {};
	virtual void			Set_On() {};
	virtual void			Set_Off() {};

protected :
	_bool					MousePicking();

public :
	_uint		Get_Order() const { return m_iOrder; }
	void		Set_Order(_uint iOrder) { m_iOrder = iOrder; }


protected :
	_float		m_fX;
	_float		m_fY;
	_float		m_fSizeX;
	_float		m_fSizeY;

	_uint		m_iOrder;

};

