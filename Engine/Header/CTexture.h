#pragma once

#include "CBaseTexture.h"
#include "Engine_Define.h"
#include <tchar.h>

BEGIN(Engine)

class ENGINE_DLL CTexture :   public CBaseTexture
{
protected:
	explicit			CTexture();
	explicit			CTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit			CTexture(const CTexture& rhs);
	virtual				~CTexture();

protected:
	HRESULT				Ready_Texture(vector<TextureSource>& _vData) override;
	HRESULT				Ready_Texture(TextureSource& _data) override;
public:
	static CTexture*	Create(LPDIRECT3DDEVICE9 pGraphicDev,vector<TextureSource>& _vData);
	static CTexture*	Create(LPDIRECT3DDEVICE9 pGraphicDev, TextureSource _vData);
	virtual CComponent* Clone();

public:
	void				Set_Frame(_vec2 _idx);
private:
	bool				CheckValidIndex(const _vec2& _idx);
	LPDIRECT3DTEXTURE9	LoadTextureAsPOT(LPDIRECT3DDEVICE9 pGraphicDev, _tchar*  pFilePath, _vec2* _vOutSize);
	_tstring			ConvertToPNGpath(const _tchar* _DDSPath);

protected:
	virtual void		Free();
};

END
