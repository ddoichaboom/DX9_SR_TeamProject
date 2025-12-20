#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

class CCharacter :
    public CGameObject
{
protected:
	explicit			CCharacter(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit			CCharacter(const CCharacter& rhs);
	virtual				~CCharacter();

public:
	HRESULT				Ready_GameObject() override;
	_int				Update_GameObject(const _float& fTimeDelta) override;
	void				LateUpdate_GameObject(const _float& fTimeDelta) override;
	void				Render_GameObject() PURE;

protected:
	virtual HRESULT		Add_Component();
	//void				Set_OnTerrain();

protected:
	void				Free() override;

protected:
	Engine::CRcTex*		m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture*	m_pTextureCom;

};

