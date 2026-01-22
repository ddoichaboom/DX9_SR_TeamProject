#pragma once
#include "CBaseUI.h"


namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CAnimation;
}
class CTakeDownBlood;
class CTakeDown :
    public CBaseUI
{

protected:
	explicit	CTakeDown(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CTakeDown(const CTakeDown& rhs);
	virtual		~CTakeDown();

public:
	static		CTakeDown* Create(PDIRECT3DDEVICE9 pGraphicDev);
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
	virtual		void		Set_On();

protected:
	static vector<TextureSource>	m_vTextureSource;
	static vector<AnimationSource>	m_vAnimSource;
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CAnimation* m_pAnimationCom;

	CTakeDownBlood* m_pBloodEffect;
};

