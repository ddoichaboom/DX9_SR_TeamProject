#pragma once
#include "CComponent.h"
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CBaseTexture : public CComponent
{
protected:
	explicit CBaseTexture();
	explicit CBaseTexture(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBaseTexture(const CBaseTexture& rhs);
	virtual ~CBaseTexture();

protected:
	virtual HRESULT Ready_Texture(vector<TextureSource>& _vData) PURE;
	virtual HRESULT Ready_Texture(TextureSource& _vData) PURE;
public:
	void Set_Texture(const _uint& _state);
	TextureDesc* GetTextureDesc(_uint _state) { return m_mapTextures[_state]; }

public:
	static void ReleaseMap(); 
protected:
	//경로당 하나의 텍스쳐만 읽어들이기 위함 (한 이미지 파일은 한번만 생성) 
	static	map<const TCHAR*, TextureDesc> m_mapAllTexture;

protected:
	//상태 값에 대응하여 저장함	
	map<_uint, TextureDesc*> m_mapTextures;
protected:
	virtual void Free();
};

END