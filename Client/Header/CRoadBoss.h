#pragma once
#include "CBoss.h"

//Texture, Animation Proto 생성은 Boss 클래스로 하기 
class CRoadBoss : public CBoss
{
protected:
	explicit		CRoadBoss(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit		CRoadBoss(const CBoss& rhs);
	virtual			~CRoadBoss();

public:
	static CRoadBoss* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CRoadBoss* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);
protected:
	HRESULT			Ready_GameObject() override;
	_int			Update_GameObject(const _float& fTimeDelta) override;
	void			LateUpdate_GameObject(const _float& fTimeDelta) override;
	void			Render_GameObject() override;

protected:
	void			Move(const _float& fTimeDelta, _float& _dirAngle, _float ratio = 1.f) override;
	void			Attack_Idle() override;

protected:
	const _vec2		m_vWidthRange = { -200.f, 200.f };
};

