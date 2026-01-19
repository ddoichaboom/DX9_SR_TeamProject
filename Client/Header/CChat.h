#pragma once
#include "CBaseUI.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CAnimation;
	class CStateComponent;
}

class CChat : public CBaseUI
{
private:
	enum CHAT_STATE : _byte
	{
		CHAT_IDLE	= 1,
		STATE_END
	};

protected:
	explicit	CChat(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CChat(const CChat& rhs);
	virtual		~CChat();

public:
	static		CChat* Create(PDIRECT3DDEVICE9 pGraphicDev);
	static _uint GetStateID(_byte _state, _byte _subState)
	{
		return ((_uint)_subState << 4) | (_uint)_state;
	}
	static void		CreateStateData();
	static vector<TextureSource>& GetTextureSources()
	{
		return m_vTextureSource;
	}
	static vector<AnimationSource>& GetAnimSources()
	{
		return m_vAnimSource;
	}

protected:
	virtual		HRESULT		Add_Component();	
	virtual		void		Free();

public:
	virtual		HRESULT		Ready_GameObject()	override;
	virtual		_int		Update_GameObject(const _float& fTimeDelta)	override;
	virtual		void		LateUpdate_GameObject(const _float& fTimeDelta)	override;
	virtual		void		Render_GameObject()	override;

public:
	virtual		void        Rotate(ROTATION eType, const _float& fAngle) override;
	virtual		void        SetPos(_vec3 _pos) override;
	virtual		void		SetScale(_float fCX, _float fCY);
	virtual		void		ChangeState(_uint nextStateID);	
	virtual		void		Set_On() { ChangeState(CHAT_IDLE); }

protected:
	void	Begin_Idle();
	void	Idle();


protected:
	static vector<TextureSource>	m_vTextureSource;
	static vector<AnimationSource>	m_vAnimSource;
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CAnimation* m_pAnimationCom;
	Engine::CStateComponent* m_pStateCom;
};

