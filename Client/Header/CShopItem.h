#pragma once
#include "CBaseUI.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

class CSelectBG;

class CShopItem : public CBaseUI
{
protected:
	explicit	CShopItem(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CShopItem(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _uint iTextureID);
	explicit	CShopItem(const CShopItem& rhs);
	virtual		~CShopItem();

public:
	static		CShopItem* Create(PDIRECT3DDEVICE9 pGraphicDev);
	static		CShopItem* Create(PDIRECT3DDEVICE9 pGraphicDev,_float fX, _float fY, _uint iTextureID);
	static vector<TextureSource>& GetTextureSources()
	{
		return m_vTextureSource;
	}

protected:
	virtual		HRESULT		Add_Component();
	virtual		void		Free();

public:
	virtual		HRESULT		Ready_GameObject()	override;
	virtual		_int		Update_GameObject(const _float& fTimeDelta)	override;
	virtual		void		LateUpdate_GameObject(const _float& fTimeDelta)	override;
	virtual		void		Render_GameObject()	override;

protected:
	virtual		void        Rotate(ROTATION eType, const _float& fAngle) override;
	virtual		void        SetPos(_vec3 _pos) override;
	virtual		void		SetScale(_float fCX, _float fCY);

public :
	void		Set_Render(_bool bRender) { m_bRender = bRender; }
	void		Set_TextureID(_uint id) { m_iTextureID = id; }
	virtual		void		Set_On();

protected:
	static vector<TextureSource>	m_vTextureSource;
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

	_uint	m_iTextureID;

	_bool	m_bRender;
	CSelectBG* m_pSelectBG;

	EventData			m_EventData;

	_float		m_fTime;
	_float		m_fInterval;
	_bool		m_bSelect;
	_bool		m_bPick;

public :
	static	wstring	 szItemHoverSFX;
	static	wstring	 szItemSelectSFX;	
	static	wstring	 szMascottCloseSFX;
	

};

