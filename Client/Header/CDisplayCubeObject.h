#pragma once
#include "CDisplayObject.h"

namespace Engine
{
	class CCubeTex;
	class CTransform;
	class CCubeTexture;
}

class CDisplayCubeObject : public CDisplayObject
{
protected:
	explicit			CDisplayCubeObject(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit			CDisplayCubeObject(const CDisplayCubeObject& rhs);
	virtual				~CDisplayCubeObject();

public:
	static CDisplayCubeObject* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static vector<TextureSource>& GetTextureSources()
	{
		return m_vTextureSource;
	}

public:
	HRESULT							Ready_GameObject() override;
	_int							Update_GameObject(const _float& fTimeDelta) override;
	void							LateUpdate_GameObject(const _float& fTimeDelta) override;
	void							Render_GameObject() override;

protected:
	HRESULT							Add_Component() override;
	void							Free() override;

protected:
	Engine::CCubeTex*				m_pCubeBufferCom;
	Engine::CCubeTexture*			m_pCubeTextureCom;

private:
	DISPLAY_CUBE_OBJECT_TYPE		m_eObjectType;

private:
	static vector<TextureSource>    m_vTextureSource;
};

