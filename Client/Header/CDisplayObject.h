#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;		
}


class CDisplayObject : public CGameObject    
{

protected:
	explicit			CDisplayObject(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit			CDisplayObject(const CDisplayObject& rhs);
	virtual				~CDisplayObject();

public :
	static CDisplayObject* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static vector<TextureSource>& GetTextureSources() { return m_vTextureSource; }

public:
	HRESULT				Ready_GameObject() override;
	_int				Update_GameObject(const _float& fTimeDelta) override;
	void				LateUpdate_GameObject(const _float& fTimeDelta) override;
	void				Render_GameObject() override;

protected:
	HRESULT				Add_Component() override;
	void				Free() override;

public :
	virtual		void	SetPos(_vec3 _pos);
	virtual		void	Rotate(ROTATION _Axis, _float _degree);
	virtual		void	SetScale(_vec3 _scale);
	virtual		void	SetTexture(_uint iTextureID);
	virtual		void	SetTransformMatrix();


	virtual		void	Activate() override;
	virtual		void	Deactivate() override;

protected :
	virtual		void	SetBillBoard();

protected:
	static vector<TextureSource>	m_vTextureSource;
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

	OBJ_ITEM_TYPE	m_eItemType;
	_uint			m_iTextureID;

};

