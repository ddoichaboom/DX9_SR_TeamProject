#pragma once
#include "CPlayerPart.h"

class CLeftPart : public CPlayerPart    
{
public :
	enum LEFT_STATE
	{
		LS_UNACTIVE = 999,
		LS_IDLE		= 0,
		LS_RELOAD_PISTOL	= 1,
		LS_RELOAD_SHOTGUN	= 2,
	};

protected:
	explicit		CLeftPart(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit		CLeftPart(const CLeftPart& rhs);
	virtual			~CLeftPart();

public :
	void Change_State(_uint iStateNum) override;
public:
	HRESULT Ready_GameObject() override;
	_int Update_GameObject(const _float& fTimeDelta) override;
	void LateUpdate_GameObject(const _float& fTimeDelta) override;
	void Render_GameObject() override;

public:
	HRESULT Add_Component() override;
	static CLeftPart* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	void Free() override;

private :
	void		Update_Idle(const float& fTimeDelta);
	void		Update_Reload(const float& fTimeDelta);	

private :
	LEFT_STATE	m_eNowState;
	_vec3		m_vEndPos;

	_float		m_fTime;
};

