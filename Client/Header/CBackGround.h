#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

class CBackGround :
    public CGameObject
{
protected:
	explicit		CBackGround(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit		CBackGround(const CBackGround& rhs);
	virtual			~CBackGround();
public:
	static TextureSource GetTextureSource()
	{
		return m_TextureSource;
	}
	static TextureSource m_TextureSource;
	static CBackGround* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public :
	HRESULT			Ready_GameObject() override;
	_int			Update_GameObject(const _float& fTimeDelta) override;
	void			LateUpdate_GameObject(const _float& fTimeDelta) override;
	void			Render_GameObject() override;

protected:
	HRESULT			Add_Component();

private:
	virtual void Free();

protected:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

};

