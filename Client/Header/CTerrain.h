#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CAnimation;
};

class CTerrain : public CGameObject
{
protected:
	explicit CTerrain(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTerrain(const CTerrain& rhs);
	virtual ~CTerrain();

public:
	void						Set_TextureIdx(_int iIdx) { m_iTextureIdx = iIdx; }

	COLLIDER_TAG				Get_ColliderTag() const { return m_eColliderTag; }
	void						Set_ColliderTag(COLLIDER_TAG eColliderTag) { m_eColliderTag = eColliderTag; }

	virtual void				SetPos(_vec3 _pos) override;
	void						SetAngle(_vec3 _rot);
	void						SetScale(_vec3 _scale);

	// Transform 컴포넌트 접근
	Engine::CTransform*			Get_Transform() const { return m_pTransformCom; }

	// ========== GameObject 인터페이스 (자식에서 구현) ==========
	virtual HRESULT				Ready_GameObject() PURE;
	virtual _int				Update_GameObject(const _float& fTimeDelta) PURE;
	virtual void				LateUpdate_GameObject(const _float& fTimeDelta) PURE;
	virtual void				Render_GameObject() PURE;
	virtual HRESULT				Add_Component();

protected:
	Engine::CRcTex*				m_pBufferCom;
	Engine::CTransform*			m_pTransformCom;
	Engine::CTexture*			m_pTextureCom;

	_bool						m_bIsAnimated;
	_bool						m_bIsBlocked;
	_int						m_iTextureIdx;
	COLLIDER_TAG                m_eColliderTag;


protected:
	// 공통 Material 설정 (자식에서 색상 커스터마이징 가능)
	virtual HRESULT Ready_Material(const D3DXCOLOR& diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	virtual void Free() override;


};