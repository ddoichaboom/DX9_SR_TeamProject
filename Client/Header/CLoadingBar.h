#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcColSide;
	class CTransform;
}

class CLoadingBar :
    public CGameObject
{
protected:
	explicit			CLoadingBar(LPDIRECT3DDEVICE9 pGraphicDev, D3DXCOLOR _color);
	virtual				~CLoadingBar();

public:
	HRESULT				Ready_GameObject() override;
	_int				Update_GameObject(const _float& fTimeDelta) override;
	void				Render_GameObject() override;

public:
	HRESULT				Add_Component() override;
	void				SetPercent(_float _percent);
	void				SetScale(_vec3 _scale)
	{
		m_vScale = _scale;
	}
	bool				IsBarEnd();
	static CLoadingBar* Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXCOLOR _color);
	void				Free() override;
private:
	_float				m_fSpeed = 0.1f;
	_float				m_fLength = WINCX*0.25f;
	bool				m_bLerp = false;
	_float				m_fCurPerecent = 0.f;
	_float				m_fDestPerecent = 0.f;

	_vec3				m_vScale = { 0.f,10.f,1.f };
	D3DXCOLOR			m_color{};
protected:
	CRcColSide*			m_pBufferCom;
	CTransform*			m_pTransformCom;
};

