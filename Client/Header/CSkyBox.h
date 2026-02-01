#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CCubeTex;
	class CTransform;
	class CCubeTexture;
}

class CSkyBox :   public CGameObject
{
private:
	explicit CSkyBox(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSkyBox(LPDIRECT3DDEVICE9 pGraphicDev, _float fHeight);
	explicit CSkyBox(const CGameObject& rhs);
	virtual ~CSkyBox();

public:
	static vector<TextureSource>& GetTextureSources()
	{
		return m_vTextureSource;
	}

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

protected:
	virtual HRESULT				Add_Component();	
	virtual void				Free() override;

private:
	Engine::CCubeTex*			m_pBufferCom;
	Engine::CTransform*			m_pTransformCom;
	Engine::CCubeTexture*		m_pTextureCom;

public:
	static CSkyBox* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CSkyBox* Create(LPDIRECT3DDEVICE9 pGraphicDev,_float fHeight);



private:
	static vector<TextureSource>    m_vTextureSource;
	_float	m_fHeight;
};

