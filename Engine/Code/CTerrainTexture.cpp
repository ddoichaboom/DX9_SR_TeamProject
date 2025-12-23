#include "CTerrainTexture.h"

CTerrainTexture::CTerrainTexture()
{
}

CTerrainTexture::CTerrainTexture(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
{
}

CTerrainTexture::CTerrainTexture(const CTerrainTexture& rhs)
	: CComponent(rhs)
{
	size_t iSize = rhs.m_vecTerrainTexture.size();

	m_vecTerrainTexture.reserve(iSize);

	m_vecTerrainTexture = rhs.m_vecTerrainTexture;

	for (size_t i = 0; i < iSize; ++i)
	{
		m_vecTerrainTexture[i]->AddRef();
	}

}

CTerrainTexture::~CTerrainTexture()
{
}

HRESULT CTerrainTexture::Ready_Texture(TEXTUREID eID, const _tchar* pPath, const _uint& iCnt)
{
	m_vecTerrainTexture.reserve(iCnt);

	IDirect3DBaseTexture9* pTexture = nullptr;

	for (_uint i = 0; i < iCnt; ++i)
	{
		TCHAR szFileName[128] = L"";

		wsprintf(szFileName, pPath, i);

		switch (eID)
		{
		case TEX_NORMAL:

			if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, szFileName, (LPDIRECT3DTEXTURE9*)&pTexture)))
				return E_FAIL;

			break;

		case TEX_CUBE:

			if (FAILED(D3DXCreateCubeTextureFromFile(m_pGraphicDev, szFileName, (LPDIRECT3DCUBETEXTURE9*)&pTexture)))
				return E_FAIL;

			break;
		}

		m_vecTerrainTexture.push_back(pTexture);
	}

	return S_OK;
}

void CTerrainTexture::Set_Texture(const _uint& iIndex)
{
	if (m_vecTerrainTexture.size() <= iIndex)
		return;

	m_pGraphicDev->SetTexture(0, m_vecTerrainTexture[iIndex]);
}

CTerrainTexture* CTerrainTexture::Create(LPDIRECT3DDEVICE9 pGraphicDev, TEXTUREID eID, const _tchar* pPath, const _uint& iCnt)
{
	CTerrainTexture* pTexture = new CTerrainTexture(pGraphicDev);

	if (FAILED(pTexture->Ready_Texture(eID, pPath, iCnt)))
	{
		Safe_Release(pTexture);
		MSG_BOX("Texture Create Failed");
		return nullptr;
	}

	return pTexture;
}

CComponent* CTerrainTexture::Clone()
{
	return new CTerrainTexture(*this);
}

void CTerrainTexture::Free()
{
	CComponent::Free();

	for_each(m_vecTerrainTexture.begin(), m_vecTerrainTexture.end(), Safe_Release<IDirect3DBaseTexture9*>);
	m_vecTerrainTexture.clear();

}
