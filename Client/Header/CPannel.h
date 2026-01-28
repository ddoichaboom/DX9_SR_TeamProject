#pragma once
#include "CGameObject.h"
#include "Engine_Define.h"


namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CStateComponent;
	class CAnimation;
}

class CPannel : public CGameObject
{
protected:
	enum MINIGUN_STATE : _byte
	{
		MS_IDLE = 0,
	};

protected:
	explicit			CPannel(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit			CPannel(const CPannel& rhs);
	virtual				~CPannel();

public:
	static CPannel* Create(LPDIRECT3DDEVICE9 pGraphicDev);
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
	HRESULT				Ready_GameObject() override;
	_int				Update_GameObject(const _float& fTimeDelta) override;
	void				LateUpdate_GameObject(const _float& fTimeDelta) override;
	void				Render_GameObject() override;

	virtual	void		Activate() override;
	virtual	void		Deactivate() override;

protected:
	virtual	void		ChangeState(_uint nextStateID);
	virtual	HRESULT		Add_Component() override;
	virtual	void		Free();


protected:
	void				Idle();

protected:
	virtual		void        Rotate(ROTATION eType, const _float& fAngle);
	virtual		void        SetPos(_vec3 _pos);
	virtual		void		SetScale(_vec3 _scale);

protected:
	static vector<TextureSource>	m_vTextureSource;
	static vector<AnimationSource>	m_vAnimSource;
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CStateComponent* m_pStateCom;
	Engine::CAnimation* m_pAnimationCom;

	_float	m_fTime = 0.f;
	_vec3	m_vPosition;
	_vec3	m_vScale;
};

