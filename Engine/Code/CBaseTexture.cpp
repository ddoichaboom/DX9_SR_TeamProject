#include "CBaseTexture.h"

map<const TCHAR*, TextureDesc> CBaseTexture::m_mapAllTexture;

CBaseTexture::CBaseTexture()
	:m_pCurTextDesc(nullptr)
{
	D3DXMatrixIdentity(&m_UVMatrix);
}

CBaseTexture::CBaseTexture(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev), m_pCurTextDesc(nullptr)
{
	D3DXMatrixIdentity(&m_UVMatrix);
}

CBaseTexture::CBaseTexture(const CBaseTexture& rhs)
	: CComponent(rhs), m_pCurTextDesc(nullptr)
{
	D3DXMatrixIdentity(&m_UVMatrix);
	for (auto& mp : rhs.m_mapTextures)
	{
		m_mapTextures.insert(mp);
		mp.second->pTexture->AddRef();
	}

}

CBaseTexture::~CBaseTexture()
{
}



void CBaseTexture::Change_Texture(const _uint& _state)
{
	m_pCurTextDesc = m_mapTextures[_state];
	D3DXMatrixIdentity(&m_UVMatrix);
}


void CBaseTexture::Render_Texture()
{
	if (!m_pCurTextDesc || !m_pCurTextDesc->pTexture) return;
	m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &m_UVMatrix);
	m_pGraphicDev->SetTexture(0, m_pCurTextDesc->pTexture);
}

void CBaseTexture::ReleaseMap()
{
	for (auto& mp : m_mapAllTexture)
	{
		Safe_Release(mp.second.pTexture);
	}
	m_mapAllTexture.clear();
}

void CBaseTexture::Reset()
{
	m_pCurTextDesc = nullptr;
}

void CBaseTexture::Free()
{
	CComponent::Free();

	for (auto& mp : m_mapTextures)
	{
		Safe_Release(mp.second->pTexture);
	}
	m_mapTextures.clear();
}