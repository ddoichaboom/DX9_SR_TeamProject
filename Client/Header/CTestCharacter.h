#pragma once
#include "CCharacter.h"

namespace Engine
{
	class CAnimation;
}

class CTestCharacter :
    public CCharacter
{
protected:
	explicit		CTestCharacter(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit		CTestCharacter(const CTestCharacter& rhs);
	virtual			~CTestCharacter();

public:
	HRESULT			Ready_GameObject() override;
	_int			Update_GameObject(const _float& fTimeDelta) override;
	void			LateUpdate_GameObject(const _float& fTimeDelta) override;
	void			Render_GameObject() override;

public:

	HRESULT			Add_Component() override;
	static CTestCharacter* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	void			Key_Input(const _float& fTimeDelta);
	virtual void Free();

protected:
	Engine::CAnimation* m_pAnimationCom;

};

