#pragma once
#include "CPlayerPart.h"

class CRightPart : public CPlayerPart
{
public:
	enum RIGHT_STATE
	{
		RS_UNACTIVE = 999,

		RS_IDLE_PISTOL = 0,
		RS_ATTACK_PISTOL = 1,
		RS_RELOAD_PISTOL = 2,

		RS_IDLE_SHOTGUN = 3,
		RS_ATTACK_SHOTGUN = 4,
		RS_RELOAD_SHOTGUN = 5,		
	};
protected:
	explicit		CRightPart(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit		CRightPart(const CRightPart& rhs);
	virtual			~CRightPart();

public:
	void Change_State(_uint iStateNum) override;
public:
	HRESULT Ready_GameObject() override;
	_int Update_GameObject(const _float& fTimeDelta) override;
	void LateUpdate_GameObject(const _float& fTimeDelta) override;
	void Render_GameObject() override;

public:
	HRESULT Add_Component() override;
	static CRightPart* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	void Free() override;

	// 만약 만든다면 상태별로 전부 만들건지 아니면 공통적인건 묶어서 처리해버릴건지
private:
	void		Update_Idle		(const float& fTimeDelta);
	void		Update_Attack	(const float& fTimeDelta);
	void		Update_Reload	(const float& fTimeDelta);

private:
	RIGHT_STATE	m_eNowState;

};

