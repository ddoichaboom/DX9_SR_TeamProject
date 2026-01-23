#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCollision;
	class CCollider;
}


class CInteractObject : public CGameObject
{

protected:
	explicit			CInteractObject(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit			CInteractObject(const CInteractObject& rhs);
	virtual				~CInteractObject();

public:
	virtual		HRESULT				Ready_GameObject() override;
	virtual		_int				Update_GameObject(const _float& fTimeDelta) override;
	virtual		void				LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual		void				Render_GameObject() override;

protected:
	virtual		HRESULT				Add_Component() override;
	virtual		void				Free() override;

public:
	virtual		void	SetPos(_vec3 _pos);
	virtual		void	Rotate(ROTATION _Axis, _float _degree);
	virtual		void	SetScale(_vec3 _scale);
	virtual		void	SetTexture(_uint iTextureID);


	virtual		void	Activate() override;
	virtual		void	Deactivate() override;

protected:
	virtual		void	SetBillBoard();

protected:	
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CCollision* m_pCollisionCom;

	OBJ_ITEM_TYPE	m_eItemType;
	_uint			m_iTextureID;

};

