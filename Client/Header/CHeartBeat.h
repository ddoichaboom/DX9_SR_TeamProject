#pragma once
#include "CBaseUI.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}


class CHeartBeat : public CBaseUI
{
protected:
	explicit	CHeartBeat(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CHeartBeat(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _uint iTextureID);
	explicit	CHeartBeat(const CHeartBeat& rhs);
	virtual		~CHeartBeat();

public:
	static		CHeartBeat* Create(PDIRECT3DDEVICE9 pGraphicDev);
	static		CHeartBeat* Create(PDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _uint iTextureID);
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
	virtual		void		Set_On() { m_fTime = 0.f;  }


protected:
	static vector<TextureSource>	m_vTextureSource;
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

protected :
	_uint	m_iTextureID;
	_float	m_fTime;
	
	_vec3	m_vStartPos;
	_vec3	m_vEndPos;
	
};

