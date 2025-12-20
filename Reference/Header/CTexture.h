#pragma once

#include "CBaseTexture.h"
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CTexture :   public CBaseTexture
{
protected:
	explicit CTexture();
	explicit CTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTexture(const CTexture& rhs);
	virtual ~CTexture();

protected:
	HRESULT Ready_Texture(vector<TextureSource>& _vData) override;
	HRESULT Ready_Texture(TextureSource& _vData) override;
public:
	static CTexture* Create(LPDIRECT3DDEVICE9 pGraphicDev,
		vector<TextureSource>& _vData);

	static CTexture* Create(LPDIRECT3DDEVICE9 pGraphicDev,
		TextureSource _vData);
	virtual CComponent* Clone();

protected:
	virtual void Free();
};

END
