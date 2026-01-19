#pragma once
#include "CBaseUI.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

class CPhoneBG : public CBaseUI    
{
protected:
	explicit	CPhoneBG(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CPhoneBG(const CPhoneBG& rhs);
	virtual		~CPhoneBG();

public :
	static		CPhoneBG*		Create(PDIRECT3DDEVICE9 pGraphicDev);
	static		TextureSource&	GetTextureSource() { return m_textureSource; }

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

protected :
	static TextureSource    m_textureSource;
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;	
};

