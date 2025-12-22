#pragma once
#include "CPlayerPart.h"

class CMiddlePart : public CPlayerPart
{
public :
	enum MIDDLE_STATE
	{
		MS_UNACTIVE = 999,
		MS_KICK	= 0,
		MS_SODA = 1
	};


protected:
	explicit		CMiddlePart(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit		CMiddlePart(const CMiddlePart& rhs);
	virtual			~CMiddlePart();

public:
	void Change_State(_uint iStateNum) override;
public:
	HRESULT Ready_GameObject() override;
	_int Update_GameObject(const _float& fTimeDelta) override;
	void LateUpdate_GameObject(const _float& fTimeDelta) override;
	void Render_GameObject() override;

public:
	HRESULT Add_Component() override;
	static CMiddlePart* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	void Free() override;

private :
	void	Update_Kick(const _float& fTimeDelta);
	void	Update_Drink(const _float& fTimeDelta);
	void	Update_Slide(const _float& fTimeDelta);

private :
	MIDDLE_STATE	m_eNowState;
};

