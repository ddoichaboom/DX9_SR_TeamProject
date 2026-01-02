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

//	.dds 확장자를 불러오기!!(TextureSource 주소에) .png는 원본 파일로 취급 
//	밉맵을 사용하려면 2의거듭제곱(POT)크기가 필요해서... 
//	POT + DXT5로 압축 + 미리 밉맵 생성해둔 .dds 파일로 텍스쳐를 생성하고 
//	원본 크기인 png 파일의 정보만 읽어와서 실제 크기 저장함 

HRESULT CTexture::Ready_Texture(vector<TextureSource>& _vData)
{
	for (auto& data : _vData)
	{
		if (m_mapAllTexture.find(data.path) == m_mapAllTexture.end())
		{
			TextureDesc txDesc;
			//원본인 png파일에서 실제 크기를 얻어옴
			D3DXIMAGE_INFO info;
			_tstring pngFilePath = ConvertToPNGpath(data.path);

			//PNG파일이 없으면 불러온 파일의 크기로 넣기
			HRESULT hr = D3DXGetImageInfoFromFile(pngFilePath.c_str(), &info);
			if (FAILED(hr))
			{
				if (FAILED(D3DXGetImageInfoFromFile(data.path, &info)))
					return E_FAIL;
				return E_FAIL;
			}
			txDesc.vOriginSize = { (_float)info.Width, (_float)info.Height };

			//DDS파일로 텍스쳐 생성
			hr = D3DXCreateTextureFromFileEx
			(
				m_pGraphicDev, data.path,
				D3DX_DEFAULT,
				D3DX_DEFAULT,
				5, // 밉 맵 레벨 
				0, // 정적 usage
				D3DFMT_UNKNOWN, // 압축 포맷 그대로 
				D3DPOOL_MANAGED,
				D3DX_FILTER_NONE, D3DX_FILTER_NONE, // 필터 
				0, NULL, NULL, (IDirect3DTexture9**)&txDesc.pTexture
			);
			if (FAILED(hr)) return E_FAIL;

			m_mapAllTexture.insert({ data.path, txDesc });
		}
		//상태 값에 대응하여 삽입 
		m_mapTextures.insert({ data.state, &m_mapAllTexture[data.path] });

	}
	return S_OK;
}

HRESULT CTexture::Ready_Texture(TextureSource& _vData)
{
	if (m_mapAllTexture.find(_vData.path) == m_mapAllTexture.end())
	{
		TextureDesc txDesc;

		D3DXIMAGE_INFO info;
		_tstring pngFilePath = ConvertToPNGpath(_vData.path);

		if (FAILED(D3DXGetImageInfoFromFile(pngFilePath.c_str(), &info)))
		{
			if (FAILED(D3DXGetImageInfoFromFile(_vData.path, &info)))
				return E_FAIL;
		}

		txDesc.vOriginSize = { (_float)info.Width, (_float)info.Height };

		HRESULT hr;
		hr = D3DXCreateTextureFromFileEx
		(
			m_pGraphicDev, _vData.path,
			D3DX_DEFAULT,
			D3DX_DEFAULT,
			5, // 밉 맵 레벨 
			0, // 정적 usage
			D3DFMT_UNKNOWN, // 압축 포맷 그대로 
			D3DPOOL_MANAGED,
			D3DX_FILTER_NONE, D3DX_FILTER_NONE, // 필터 
			0, NULL, NULL, (IDirect3DTexture9**)&txDesc.pTexture
		);
		if (FAILED(hr)) return E_FAIL;

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

//앨리어싱을 방지하기 위해 밉맵을 켜야히는데 이미지 크기가 2의 거듭제곱이 아님 
//그래서 2의 거듭제곱(POT)의 surface를 만들고 그 곳에 이미지를 복사함!
//실제 cusSize는 원본 이미지 크기를 바탕으로 만들고, UV비율은 이 CutSize/만들어진 POT크기 surface크기 로 잡음 
//TODO : dds밉맵이 이상이 없으면 지우기 
LPDIRECT3DTEXTURE9 CTexture::LoadTextureAsPOT(LPDIRECT3DDEVICE9 pGraphicDev, _tchar* pFilePath, _vec2* _vOutSize)
{
	D3DXIMAGE_INFO info;
	D3DXGetImageInfoFromFile(pFilePath, &info);
	//이미지의 실제 크기. UV 비율은 실제 크기를 기준으로 잡아야함 

	if (!_vOutSize) return nullptr;
	_vOutSize->x = (float)info.Width;
	_vOutSize->y = (float)info.Height;

	//내 텍스쳐 크기보다는 크면서 가장 작은 2의 거듭제곱 찾기
	_uint potWidth = 1;
	while (potWidth < info.Width) potWidth *= 2;

	_uint potHeight = 1;
	while (potHeight < info.Height) potHeight *= 2;

	//5개의 surface를 가진 텍스쳐 객체 생성
	LPDIRECT3DTEXTURE9 pTexture = nullptr;
	pGraphicDev->CreateTexture(
		potWidth, potHeight,
		5, // 밉레벨. 5는 임의의 값. 많이 멀어지는 경우는 없을듯해서 
		0, // Usage
		D3DFMT_DXT5,//포맷형식 지정. 아래 LoadSurface에서 알아서 포맷대로 압축해줌
		D3DPOOL_MANAGED,
		&pTexture,
		nullptr
	);

	LPDIRECT3DSURFACE9 pSurface = nullptr;
	pTexture->GetSurfaceLevel(0, &pSurface);

	//0번 surface 0으로 초기화
	pGraphicDev->ColorFill(pSurface, nullptr, D3DCOLOR_ARGB(0, 0, 0, 0));

	//실제 텍스쳐를 0번 surface에 복사하기
	RECT destRect = { 0, 0, (_long)info.Width, (_long)info.Height };
	D3DXLoadSurfaceFromFile(
		pSurface,
		nullptr,
		&destRect, // surface 영역의 어디에 복사할지. nullptr로 하면 이미지가 늘려져서 surface에 꽉 채워짐 
		pFilePath,
		nullptr,	//원본에서 로드 될 영역 
		D3DX_FILTER_POINT | D3DX_FILTER_DITHER, // 필터. 뒤는 디더링옵션
		0,
		nullptr
	);

	pSurface->Release();

	//0번 텍스쳐를 기준으로 밉맵 다시 생성 
	D3DXFilterTexture(pTexture, nullptr, D3DX_DEFAULT, D3DX_FILTER_BOX);

	return pTexture;
}

_tstring CTexture::ConvertToPNGpath(const _tchar* _DDSPath)
{
	_tstring filePath = _DDSPath;
	size_t tokkenIdx = filePath.find_last_of(_T("."));
	_tstring result = filePath.substr(0, tokkenIdx) + _T(".png");
	return result;
}

void CTexture::Free()
{
	CBaseTexture::Free();
}

