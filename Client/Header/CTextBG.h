#pragma once
#include "CBaseUI.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

class CPlusUI;
class CFontUI;

class CTextBG : public CBaseUI    
{
protected:
	explicit	CTextBG(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CTextBG(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);
	explicit	CTextBG(const CTextBG& rhs);
	virtual		~CTextBG();

public:
	static		CTextBG* Create(PDIRECT3DDEVICE9 pGraphicDev);
	static		CTextBG* Create(PDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);
	static		TextureSource& GetTextureSource() { return m_textureSource; }
	
	void		Set_Text(const wstring& wDeadText, const wstring& wDeadTime);

	void		Init();

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
	CPlusUI* m_pPlusUI;
	CFontUI* m_pDeadFontUI;
	CFontUI* m_pTimeFontUI;

	_float	m_fTime = 0.f;
	_vec3	m_vStartPos;
	_vec3	m_vEndPos;

};

