#pragma once
#include "CBaseUI.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

class CStageBG : public CBaseUI    
{
protected:
	explicit	CStageBG(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CStageBG(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _uint iStageNum);
	explicit	CStageBG(const CStageBG& rhs);
	virtual		~CStageBG();

public:
	static		CStageBG* Create(PDIRECT3DDEVICE9 pGraphicDev);	
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


protected:
	static vector<TextureSource>	m_vTextureSource;
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

	_bool	m_bRender;
	_uint	m_iStageNum;
};

