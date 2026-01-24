#pragma once
#include "CBaseUI.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}


class CHudUI : public CBaseUI
{
protected:
	explicit	CHudUI(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CHudUI(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY);
	explicit	CHudUI(const CHudUI& rhs);
	virtual		~CHudUI();

public:
	static		CHudUI* Create(PDIRECT3DDEVICE9 pGraphicDev);
	static		CHudUI* Create(PDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY);
	static		TextureSource& GetTextureSource() { return m_textureSource; }

protected:
	virtual		HRESULT		Add_Component();
	virtual		void		Free();

public:
	virtual		HRESULT		Ready_GameObject()	override;
	virtual		_int		Update_GameObject(const _float& fTimeDelta)	override;
	virtual		void		LateUpdate_GameObject(const _float& fTimeDelta)	override;
	virtual		void		Render_GameObject()	override;

	virtual		void		Activate() override;
	virtual		void		Deactivate() override;

protected:
	virtual		void        Rotate(ROTATION eType, const _float& fAngle) override;
	virtual		void        SetPos(_vec3 _pos) override;
	virtual		void		SetScale(_float fCX, _float fCY);
	virtual		void		SetScale(_vec3 vScale);

protected:
	static TextureSource    m_textureSource;
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

	_float	m_fTime;
	_float	m_fInterval;
	_bool	m_bSizeLerp;

	_vec3	m_vStartScale;
	_vec3	m_vEndScale;

};

