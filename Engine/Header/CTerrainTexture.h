#pragma once

#include "CComponent.h"
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CTerrainTexture : public CComponent
{
private:
	explicit CTerrainTexture();
	explicit CTerrainTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTerrainTexture(const CTerrainTexture& rhs);
	virtual ~CTerrainTexture();

public:
	virtual HRESULT Ready_Texture(TEXTUREID eID, const _tchar* pPath, const _uint& iCnt);
	void			Set_Texture(const _uint& iIndex = 0);

private:
	vector<IDirect3DBaseTexture9*>		m_vecTerrainTexture;

public:
	static CTerrainTexture* Create(LPDIRECT3DDEVICE9 pGraphicDev, TEXTUREID eID,
		const _tchar* pPath, const _uint& iCnt = 1);
	virtual CComponent* Clone();

private:
	virtual void Free();

};


END
