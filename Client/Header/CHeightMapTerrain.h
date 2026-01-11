#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CTerrainTex;
	class CTransform;
	class CTerrainTexture;
}

class CHeightMapTerrain : public CGameObject
{
private:
	explicit CHeightMapTerrain(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CHeightMapTerrain(const CGameObject& rhs);
	virtual ~CHeightMapTerrain();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();
	HRESULT			Ready_Material();

private:
	Engine::CTerrainTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTerrainTexture* m_pTextureCom;

public:
	static CHeightMapTerrain* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void Free();

};

