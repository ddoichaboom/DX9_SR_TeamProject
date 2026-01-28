#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CAnimation;
	class CStateComponent;
}

class CSRightHand :
	public CGameObject
{
protected:
	enum LEFT_STATE { LS_NONE, LS_INTRO, LS_IDLE, LS_ATTACK, LS_RELOAD, LS_END };
public:
	explicit		CSRightHand(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual			~CSRightHand();

public:
	HRESULT			Ready_GameObject() override;
	_int			Update_GameObject(const _float& fTimeDelta) override;
	void			LateUpdate_GameObject(const _float& fTimeDelta) override;
	void			Render_GameObject() override;

protected:
	HRESULT			Add_Component() override;

	void			Intro_Begin();
	void			Intro();
	void			Idle();

	void			ZoomIn();
	void			Attack();
	void			ZoomOut();
	void			Shoot();

	void			Reload();

protected:
	static void		CreateStateData();
	void			ChangeState(_uint nextStateID);
public:
	static vector<TextureSource>& GetTextureSources()
	{
		return m_vTextureSource;
	}
	static vector<AnimationSource>& GetAnimSources()
	{
		return m_vAnimSource;
	}
	static CSRightHand* Create(LPDIRECT3DDEVICE9 pGraphicDev);

	void			Change_State(_uint eState);

public:
	bool			IsAnimationEnd();
	bool			CanAnimationEnd();


	void			SetState_INTRO()
	{
		Change_State(LS_INTRO);
	}
	void			SetState_IDLE()
	{
		Change_State(LS_IDLE);
	}
	void			SetState_ATTACK()
	{
		Change_State(LS_ATTACK);
	}
	void			SetState_RELOAD()
	{
		Change_State(LS_RELOAD);
	}


protected:
	void			Free() override;

protected:
	static vector<TextureSource> m_vTextureSource;
	static vector<AnimationSource> m_vAnimSource;

protected:
	_vec3 m_vScale = { 700, 700, 1 };
	_vec3 m_vIntoPos = { 100, 50, 0 };
	_vec3 m_vPos = { 400, 50, 0 };

	_float m_fTime = 0.f ;
	CRcTex* m_pBufferCom = nullptr ;
	CTransform* m_pTransformCom = nullptr;
	CAnimation* m_pAnimationCom = nullptr;
	CStateComponent* m_pStateCom= nullptr;

	LEFT_STATE m_eState = LS_NONE;

	//State Value
protected:

};;

