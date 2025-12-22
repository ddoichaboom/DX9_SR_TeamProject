#pragma once
#include "CCharacter.h"

namespace Engine
{
	class CAnimation;
}

class CRightHand : public CCharacter
{
protected:
	explicit		CRightHand(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit		CRightHand(const CRightHand& rhs);
	virtual			~CRightHand();

public :
	void			Set_Animation();

public:
	HRESULT			Ready_GameObject() override;
	_int			Update_GameObject(const _float& fTimeDelta) override;
	void			LateUpdate_GameObject(const _float& fTimeDelta) override;
	void			Render_GameObject() override;

public:

	HRESULT			Add_Component() override;
	static CRightHand* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free();

private:
	Engine::CAnimation* m_pAnimationCom;
};

