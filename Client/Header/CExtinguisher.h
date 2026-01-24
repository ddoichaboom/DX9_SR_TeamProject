#pragma once
#include "CInteractObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCollision;
	class CCollider;
}

class CExtinguisher : public CInteractObject
{
protected:
	explicit						CExtinguisher(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit						CExtinguisher(const CExtinguisher& rhs);
	virtual							~CExtinguisher();

public:
	static		CExtinguisher* Create(PDIRECT3DDEVICE9 pGraphicDev);
	static		TextureSource& GetTextureSources() { return m_vTextureSource; }


public:
	virtual		HRESULT				Ready_GameObject() override;
	virtual		_int				Update_GameObject(const _float& fTimeDelta) override;
	virtual		void				LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual		void				Render_GameObject() override;

protected:
	virtual		HRESULT				Add_Component() override;
	virtual		void				Free() override;

public:
	virtual		void				Activate() override;
	virtual		void				Deactivate() override;	

	

protected:
	void			OnCollision(CollisionInfo info);
	void			DeadAction();

	void			CheckExplosiveCollision();

protected:
	static TextureSource    m_vTextureSource;

	Engine::CCollider* m_pMainCollider;
	const _tchar* m_szMainColliderName = L"ColMain";

	CCollider* m_pExplosiveCollider;
	const _tchar* m_szExplosiveColliderName = L"ColExplosive";
};

