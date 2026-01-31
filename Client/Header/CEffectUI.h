#pragma once
#include "CBaseUI.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

class CTakeDownUI;
class CPlusUI;
class CFontUI;

class CEffectUI : public CBaseUI
{
protected:
	explicit	CEffectUI(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CEffectUI(const CEffectUI& rhs);
	virtual		~CEffectUI();

public:
	static		CEffectUI* Create(PDIRECT3DDEVICE9 pGraphicDev);
	static		TextureSource& GetTextureSource() { return m_textureSource; }

	void		Set_Text(const wstring& wDeadTime);

	void		Init(_bool bRandomColor);
	void		Init(D3DXCOLOR eColor);

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

	CTakeDownUI* m_pBackUI;
	CFontUI* m_pEffectText;

	_float	m_fTime = 0.f;

	_bool	m_bRandomColor;

	_vec3   m_vEffectPos;
	_vec3	m_vClearPos;

	_float  m_fDuration;

	D3DXCOLOR m_FontColor = { 1,1,1,1 };

};

