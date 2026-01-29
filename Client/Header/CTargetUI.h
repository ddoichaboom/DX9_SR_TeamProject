#pragma once
#include "CBaseUI.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

class CSniperPlayer;

class CTargetUI : public CBaseUI
{
protected:
	explicit	CTargetUI(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CTargetUI(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY);
	explicit	CTargetUI(const CTargetUI& rhs);
	virtual		~CTargetUI();

public:
	static		CTargetUI* Create(PDIRECT3DDEVICE9 pGraphicDev);
	static		CTargetUI* Create(PDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY);
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

	CSniperPlayer* m_pPlayer;

	_vec3   m_vScale;
	_vec3	m_vStartScale;
	_vec3	m_vEndScale;

	_float  m_fTime;
	_bool	m_bInit;

};

