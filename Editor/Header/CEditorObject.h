#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CTransform;
	class CVIBuffer;
	class CTexture;
}

class CEditorObject : public CGameObject
{
protected:
	explicit        CEditorObject(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual         ~CEditorObject();

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fTimeDelta) override;
	virtual void		LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual void		Render_GameObject() PURE;

public:

	// Transform 관련
	void				Set_Position(_vec3 vPos);
	void				Set_Rotation(_vec3 vRot);
	void				Set_Scale(_vec3 vScale);

	_vec3				Get_Position();
	_vec3				Get_Rotation();
	_vec3				Get_Scale();

	// 선택 관련
	void				Set_Selected(bool bSelected) { m_bSelected = bSelected; }
	bool				Get_Selected() const { return m_bSelected; }

	// 이름
	void				Set_Name(const wstring& wstrName) { m_wstrName = wstrName; }
	const wstring& Get_Name() const { return m_wstrName; }

	// 텍스처 (Phase 5+)
	void				Set_Texture(Engine::CTexture* pTexture);
	Engine::CTexture*	Get_Texture() { return m_pTextureCom; }

	const _matrix*		Get_WorldMatrix() const;

protected:
	// Component
	Engine::CTransform* m_pTransformCom;
	Engine::CVIBuffer* m_pBufferCom;
	Engine::CTexture* m_pTextureCom;

	// State
	bool                    m_bSelected;
	wstring                 m_wstrName;

protected:
	LPDIRECT3DDEVICE9       m_pGraphicDev;

protected:
	virtual void Free() override;

};

