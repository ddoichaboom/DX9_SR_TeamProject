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

protected :
	_bool					MousePicking();
	virtual		void		Set_TransformPosition() {};


protected :
	_float		m_fX;
	_float		m_fY;
	_float		m_fSizeX;
	_float		m_fSizeY;

};

