#include "CTexture.h"

CTexture::CTexture()
{
}

CTexture::CTexture(LPDIRECT3DDEVICE9 pGraphicDev)
	: CBaseTexture(pGraphicDev)
{
}

CTexture::CTexture(const CTexture& rhs)
	: CBaseTexture(rhs)
{

}

CTexture::~CTexture()
{
}

HRESULT CTexture::Ready_Texture(vector<TextureSource>& _vData)
{
	for (auto& data : _vData)
	{
		if (m_mapAllTexture.find(data.path) == m_mapAllTexture.end())
		{
			TextureDesc txDesc;
			D3DXIMAGE_INFO info;
			HRESULT hr;
			hr = D3DXCreateTextureFromFileEx
			(
				m_pGraphicDev, data.path,
				D3DX_DEFAULT_NONPOW2, // mipmap용으로 2의거듭제곱으로 반올림하지 말기 (하드웨어가 지원하면 적용됨) 
				D3DX_DEFAULT_NONPOW2,
				5,// 밉맵레벨 
				0, // 정적 usage
				D3DFMT_UNKNOWN,
				D3DPOOL_MANAGED,
				D3DX_FILTER_NONE, D3DX_FILTER_NONE, // 필터 
				0, &info, NULL, (IDirect3DTexture9**)&txDesc.pTexture
			);
			if (FAILED(hr)) return E_FAIL;
			txDesc.vOriginSize = { (float)info.Width, (float)info.Height };
			m_mapAllTexture.insert({ data.path, txDesc });
		}

		m_mapTextures.insert({ data.state, &m_mapAllTexture[data.path]});

	}
	return S_OK;
}

HRESULT CTexture::Ready_Texture(TextureSource& _vData)
{
	if (m_mapAllTexture.find(_vData.path) == m_mapAllTexture.end())
	{
		TextureDesc txDesc;
		D3DXIMAGE_INFO info;
		HRESULT hr;
		hr = D3DXCreateTextureFromFileEx
		(
			m_pGraphicDev, _vData.path,
			D3DX_DEFAULT_NONPOW2, // mipmap용으로 2의거듭제곱으로 반올림하지 말기 (하드웨어가 지원하면 적용됨) 
			D3DX_DEFAULT_NONPOW2,
			5,// 밉맵레벨 
			0, // 정적 usage
			D3DFMT_UNKNOWN,
			D3DPOOL_MANAGED,
			D3DX_FILTER_NONE, D3DX_FILTER_NONE, // 필터 
			0, &info, NULL, (IDirect3DTexture9**)&txDesc.pTexture
		);
		if (FAILED(hr)) return E_FAIL;
		txDesc.vOriginSize = { (float)info.Width, (float)info.Height };
		m_mapAllTexture.insert({ _vData.path, txDesc });
	}

	m_mapTextures.insert({ _vData.state, &m_mapAllTexture[_vData.path] });
	return S_OK;
}

CTexture* CTexture::Create(LPDIRECT3DDEVICE9 pGraphicDev, vector<TextureSource>& _vData)
{
	CTexture* pTexture = new CTexture(pGraphicDev);

	if (FAILED(pTexture->Ready_Texture(_vData)))
	{
		Safe_Release(pTexture);
		MSG_BOX("Texture Create Failed");
		return nullptr;
	}

	return pTexture;
}

CTexture* CTexture::Create(LPDIRECT3DDEVICE9 pGraphicDev, TextureSource _vData)
{
	CTexture* pTexture = new CTexture(pGraphicDev);

	if (FAILED(pTexture->Ready_Texture(_vData)))
	{
		Safe_Release(pTexture);
		MSG_BOX("Texture Create Failed");
		return nullptr;
	}

	return pTexture;
}

CComponent* CTexture::Clone()
{
	return new CTexture(*this);
}

void CTexture::Free()
{
	CBaseTexture::Free();
}
