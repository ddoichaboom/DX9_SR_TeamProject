#pragma once
#include "CCharacter.h"

namespace Engine
{
	class CAnimation;
}

class CMiddlePart : public CCharacter
{
protected:
	explicit		CMiddlePart(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit		CMiddlePart(const CMiddlePart& rhs);
	virtual			~CMiddlePart();


public:
	void			Set_Animation();

public:
	HRESULT			Ready_GameObject() override;
	_int			Update_GameObject(const _float& fTimeDelta) override;
	void			LateUpdate_GameObject(const _float& fTimeDelta) override;
	void			Render_GameObject() override;

public:

	HRESULT			Add_Component() override;
	static CMiddlePart* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free();

private:
	Engine::CAnimation* m_pAnimationCom;
};

