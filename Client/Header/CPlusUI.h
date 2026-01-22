#pragma once
#include "CBaseUI.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

class CPlusUI : public CBaseUI    
{
protected:
	explicit	CPlusUI(LPDIRECT3DDEVICE9 pGraphicDev);	
	explicit	CPlusUI(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);	
	explicit	CPlusUI(const CPlusUI& rhs);
	virtual		~CPlusUI();

public:
	static		CPlusUI* Create(PDIRECT3DDEVICE9 pGraphicDev);
	static		CPlusUI* Create(PDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);
	static		TextureSource& GetTextureSource() { return m_textureSource; }
	
	void		Set_Parent(CBaseUI* pParent);
	void		Set_Projection(bool bProj) { m_bProject = bProj; }

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

protected:
	static TextureSource    m_textureSource;
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

	CBaseUI* m_pParentUI;

	_vec3 m_vLocalPos;
	_bool	m_bProject;
};

