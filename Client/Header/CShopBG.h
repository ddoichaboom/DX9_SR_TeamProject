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

class CShopItem;

class CShopBG : public CBaseUI
{
private :
	enum SHOPBG_STATE : _byte
	{
		LOADING  = 1,
		NOISE,
		ONPAGE,
		SHOP_END
	};
protected:
	explicit	CShopBG(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CShopBG(const CShopBG& rhs);
	virtual		~CShopBG();

public:
	static		CShopBG* Create(PDIRECT3DDEVICE9 pGraphicDev);
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
	virtual		HRESULT		Add_ShopItem();
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

protected:
	void	Begin_Idle();
	void	Idle();

	void	Begin_Noise();
	void	Noise();

	void	Begin_OnPage();
	void	OnPage();

protected:
	static vector<TextureSource>	m_vTextureSource;
	static vector<AnimationSource>	m_vAnimSource;
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CAnimation* m_pAnimationCom;
	Engine::CStateComponent* m_pStateCom;
protected :
	_vec3		m_vStartPos;
	_vec3		m_vEndPos;
	_vec3		m_vStartScale;
	_vec3		m_vEndScale;

	_float		m_fTime;
	_float		m_fDelayTime;
	_bool		m_bDelay;
	_bool		m_bStateStop;

	CShopItem* m_pItem[3];
	_bool	m_bRender;
};

