#include "CCubeTexture.h"

CCubeTexture::CCubeTexture()
{
}

CCubeTexture::CCubeTexture(LPDIRECT3DDEVICE9 pGraphicDev)
	: CBaseTexture(pGraphicDev)
{
}

CCubeTexture::CCubeTexture(const CCubeTexture& rhs)
	: CBaseTexture(rhs)
{
}

CCubeTexture::~CCubeTexture()
{
}

HRESULT CCubeTexture::Ready_Texture(vector<TextureSource>& _vData)
{
	for (auto& data : _vData)
	{
		if (m_mapAllTexture.find(data.path) == m_mapAllTexture.end())
		{
			TextureDesc txDesc;
			D3DXIMAGE_INFO info;
			HRESULT hr;
			hr = D3DXCreateCubeTextureFromFileEx
			(
				m_pGraphicDev, data.path,
				D3DX_DEFAULT,
				5,// π”∏ ∑π∫ß 
				0, // ¡§¿˚ usage
				D3DFMT_UNKNOWN,
				D3DPOOL_MANAGED,
				D3DX_DEFAULT, D3DX_DEFAULT, // « ≈Õ 
				0, &info, NULL, (IDirect3DCubeTexture9**)&txDesc.pTexture
			);
			if (FAILED(hr)) return E_FAIL;
			txDesc.vOriginSize = { (float)info.Width, (float)info.Height };
			m_mapAllTexture.insert({ data.path, txDesc });
		}

		m_mapTextures.insert({ data.state, &m_mapAllTexture[data.path] });

	}
	return S_OK;
}

HRESULT CCubeTexture::Ready_Texture(TextureSource& _vData)
{
	if (m_mapAllTexture.find(_vData.path) == m_mapAllTexture.end())
	{
		TextureDesc txDesc;
		D3DXIMAGE_INFO info;
		HRESULT hr;
		hr = D3DXCreateCubeTextureFromFileEx
		(
			m_pGraphicDev, _vData.path,
			D3DX_DEFAULT,
			5,// π”∏ ∑π∫ß 
			0, // ¡§¿˚ usage
			D3DFMT_UNKNOWN,
			D3DPOOL_MANAGED,
			D3DX_DEFAULT, D3DX_DEFAULT, // « ≈Õ 
			0, &info, NULL, (IDirect3DCubeTexture9**)&txDesc.pTexture
		);
		if (FAILED(hr)) return E_FAIL;
		txDesc.vOriginSize = { (float)info.Width, (float)info.Height };
		m_mapAllTexture.insert({ _vData.path, txDesc });
	}

	m_mapTextures.insert({ _vData.state, &m_mapAllTexture[_vData.path] });
	return S_OK;
}

CCubeTexture* CCubeTexture::Create(LPDIRECT3DDEVICE9 pGraphicDev,  vector<TextureSource>& _vData)
{
	CCubeTexture* pCubeTexture = new CCubeTexture(pGraphicDev);

	if (FAILED(pCubeTexture->Ready_Texture(_vData)))
	{
		Safe_Release(pCubeTexture);
		MSG_BOX("Cube Texture Create Failed");
		return nullptr;
	}
	return pCubeTexture;
}

CCubeTexture* CCubeTexture::Create(LPDIRECT3DDEVICE9 pGraphicDev, TextureSource _vData)
{
	CCubeTexture* pCubeTexture = new CCubeTexture(pGraphicDev);

	if (FAILED(pCubeTexture->Ready_Texture(_vData)))
	{
		Safe_Release(pCubeTexture);
		MSG_BOX("Cube Texture Create Failed");
		return nullptr;
	}
	return pCubeTexture;
}


CComponent* CCubeTexture::Clone()
{
	return new CCubeTexture(*this);
}

void CCubeTexture::Free()
{
	CBaseTexture::Free();
}
