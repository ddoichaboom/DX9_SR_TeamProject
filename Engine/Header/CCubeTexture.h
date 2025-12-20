#pragma once
#include "CBaseTexture.h"
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CCubeTexture : public CBaseTexture
{
protected:
	explicit CCubeTexture();
	explicit CCubeTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCubeTexture(const CCubeTexture& rhs);
	virtual ~CCubeTexture();

protected:
	HRESULT Ready_Texture(vector<TextureSource>& _vData) override;
	HRESULT Ready_Texture(TextureSource& _vData) override;

public:
	static CCubeTexture* Create(LPDIRECT3DDEVICE9 pGraphicDev,
		vector<TextureSource>& _vData);

	static CCubeTexture* Create(LPDIRECT3DDEVICE9 pGraphicDev,
		TextureSource _vData);

	virtual CComponent* Clone();

protected:
	void Free() override;

};

END
