#pragma once
#include "CBaseUI.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

class CFontUI;

class CInfoUI : public CBaseUI
{
protected:
	explicit	CInfoUI(LPDIRECT3DDEVICE9 pGraphicDev);	
	explicit	CInfoUI(const CInfoUI& rhs);
	virtual		~CInfoUI();

public:
	static		CInfoUI* Create(PDIRECT3DDEVICE9 pGraphicDev);
	static TextureSource& GetTextureSource()
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
	virtual		void		Set_On();
	virtual		void		Set_Off();

protected:
	static TextureSource    m_vTextureSource;
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;


	_bool	m_bRender;
	_float	m_fTime;
	CFontUI* m_pTimeText;
	CFontUI* m_pStageText;

	_vec3	m_vTimePos;
	_vec3	m_vTimeScale;

};

