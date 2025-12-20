#include "CBaseTexture.h"

map<const TCHAR*, TextureDesc> CBaseTexture::m_mapAllTexture;

CBaseTexture::CBaseTexture()
{
}

CBaseTexture::CBaseTexture(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
{
}

CBaseTexture::CBaseTexture(const CBaseTexture& rhs)
	: CComponent(rhs)
{
	for (auto& mp : rhs.m_mapTextures)
	{
		m_mapTextures.insert(mp);
		mp.second->pTexture->AddRef();
	}

}

CBaseTexture::~CBaseTexture()
{
}

void CBaseTexture::Set_Texture(const _uint& _state)
{
	IDirect3DBaseTexture9* texture = m_mapTextures[_state]->pTexture;
	if (texture == nullptr) return;
	m_pGraphicDev->SetTexture(0, texture);
}

void CBaseTexture::ReleaseMap()
{
	for (auto& mp : m_mapAllTexture)
	{
		Safe_Release(mp.second.pTexture);
	}
	m_mapAllTexture.clear();
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