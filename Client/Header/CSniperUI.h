#pragma once
#include "CBaseUI.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

class CSniperUI :
    public CBaseUI
{
protected:
	explicit				CSniperUI(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit				CSniperUI(const CSniperUI& rhs);
	virtual					~CSniperUI();
public:
	static		CSniperUI*	Create(PDIRECT3DDEVICE9 pGraphicDev);
	static		TextureSource& GetTextureSource() { return m_textureSource; }

protected:
	virtual		HRESULT		Add_Component();
	virtual		void		Free();

public:
	virtual		HRESULT		Ready_GameObject()	override;
	virtual		_int		Update_GameObject(const _float& fTimeDelta)	override;
	virtual		void		Render_GameObject()	override;

protected:
	static TextureSource    m_textureSource;
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

};

