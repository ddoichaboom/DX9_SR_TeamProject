#pragma once
#include "CCharacter.h"

namespace Engine
{
	class CAnimation;
}

class CLeftHand : public CCharacter    
{

protected:
	explicit		CLeftHand(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit		CLeftHand(const CLeftHand& rhs);
	virtual			~CLeftHand();


public :
	void			Set_Animation();

public:
	HRESULT			Ready_GameObject() override;
	_int			Update_GameObject(const _float& fTimeDelta) override;
	void			LateUpdate_GameObject(const _float& fTimeDelta) override;
	void			Render_GameObject() override;


// Test

private:
	enum eState
	{
		IDLE = 0,
		PISTOL_RELOAD,
		SHOTGUN_RELOAD
	};


private :
	eState			m_eNowState;
	void			Change_State(eState eState);
	void			Update_Idle(const _float& fTimeDelta);
	void			Update_Reload_Pistol(const _float& fTimeDelta);
	void			Update_Reload_ShotGun(const _float& fTimeDelta);

public:

	HRESULT			Add_Component() override;
	static CLeftHand* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free();

private:
	Engine::CAnimation* m_pAnimationCom;	


	_vec3	m_vStartPos;
	_vec3	m_vEndPos;


	

};

