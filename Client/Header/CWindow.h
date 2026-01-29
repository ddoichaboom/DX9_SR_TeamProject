#pragma once
#include "CInteractObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CAnimation;
	class CStateComponent;
	class CCollision;
	class CCollider;
}

class CWindow : public CInteractObject
{
private:
	enum GLASS_STATE : _byte
	{
		GLASS_IDLE = 0,
		GLASS_DEAD,
		GLASS_END
	};
protected:
	explicit						CWindow(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit						CWindow(const CWindow& rhs);
	virtual							~CWindow();

public:
	static		CWindow* Create(PDIRECT3DDEVICE9 pGraphicDev);
	static void		CreateStateData();
	static vector<TextureSource>& GetTextureSources()
	{
		return m_vTextureSource;
	}
	static vector<AnimationSource>& GetAnimSources()
	{
		return m_vAnimSource;
	}


public:
	virtual		HRESULT				Ready_GameObject() override;
	virtual		_int				Update_GameObject(const _float& fTimeDelta) override;
	virtual		void				LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual		void				Render_GameObject() override;

protected:
	virtual		HRESULT				Add_Component() override;
	virtual		void				Free() override;

public:
	virtual		void				Activate() override;
	virtual		void				Deactivate() override;

	virtual		void				ChangeState(_uint nextStateID);

protected:
	void			OnCollision(CollisionInfo info);
	void			DeadAction();

protected:
	void	Begin_Idle();
	void	Idle();

	void	Begin_Dead();
	void	Dead();

protected:
	static vector<TextureSource>	m_vTextureSource;
	static vector<AnimationSource>	m_vAnimSource;
	Engine::CAnimation* m_pAnimationCom;
	Engine::CStateComponent* m_pStateCom;
	Engine::CCollider* m_pMainCollider;
	const _tchar* m_szMainColliderName = L"ColMain";

	_float	m_fTime = 0.f;
};

