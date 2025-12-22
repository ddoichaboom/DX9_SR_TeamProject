#pragma once
#include "CCharacter.h"

class CLeftHand;
class CRightHand;
class CMiddlePart;

class CPlayer :
	public CCharacter
{
protected:
	explicit		CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit		CPlayer(const CPlayer& rhs);
	virtual			~CPlayer();

public:
	HRESULT			Ready_GameObject() override;
	_int			Update_GameObject(const _float& fTimeDelta) override;
	void			LateUpdate_GameObject(const _float& fTimeDelta) override;
	void			Render_GameObject() override;

protected:
	HRESULT			Add_Component() override;
	void			Key_Input(const _float& fTimeDelta);

private:
	HRESULT			Add_PlayerPart();

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free();

private:
	CLeftHand* m_pLeftHand;
	CRightHand* m_pRightHand;
	CMiddlePart* m_pMiddlePart;


	_bool		m_bCheck;
};

