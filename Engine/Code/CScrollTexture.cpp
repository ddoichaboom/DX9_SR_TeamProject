#include "CScrollTexture.h"

CScrollTexture::CScrollTexture()
    :CTexture(), m_vDir{}, m_fSpeed(0.f)
{
}

CScrollTexture::CScrollTexture(LPDIRECT3DDEVICE9 pGraphicDev, _float _speed, _vec2 _dir)
    :CTexture(pGraphicDev), m_vDir(_dir), m_fSpeed(_speed), m_vUVPos{ 0,0 }
{
}

CScrollTexture::CScrollTexture(const CScrollTexture& rhs)
    :CTexture(rhs),m_vDir(rhs.m_vDir), m_fSpeed(rhs.m_fSpeed), m_vUVPos{0,0}
{ 
}

CScrollTexture::~CScrollTexture()
{
}

void CScrollTexture::Update_Texture(const _float& fTimeDelta)
{
    m_vUVPos.x += fTimeDelta * m_fSpeed * m_vDir.x ;
    m_vUVPos.y += fTimeDelta * m_fSpeed * m_vDir.y ;
}

void CScrollTexture::Late_Update_Texture()
{
    D3DXMatrixIdentity(&m_UVMatrix);
    m_UVMatrix._31 = m_vUVPos.x;
    m_UVMatrix._32 = m_vUVPos.y;
}



CScrollTexture* CScrollTexture::Create(LPDIRECT3DDEVICE9 pGraphicDev, vector<TextureSource>& _vData, _float _speed, _vec2 _dir)
{
    CScrollTexture* pTexture = new CScrollTexture(pGraphicDev, _speed, _dir);

    if (FAILED(pTexture->Ready_Texture(_vData)))
    {
        Safe_Release(pTexture);
        MSG_BOX("Scroll Texture Create Failed");
        return nullptr;
    }

    return pTexture;
}

CScrollTexture* CScrollTexture::Create(LPDIRECT3DDEVICE9 pGraphicDev, TextureSource _vData, _float _speed, _vec2 _dir)
{
    CScrollTexture* pTexture = new CScrollTexture(pGraphicDev,_speed, _dir);

    if (FAILED(pTexture->Ready_Texture(_vData)))
    {
        Safe_Release(pTexture);
        MSG_BOX("Scroll Texture Create Failed");
        return nullptr;
    }

    return pTexture;
}

CComponent* CScrollTexture::Clone()
{
    return new CScrollTexture(*this);

}
void CScrollTexture::Free()
{
    CTexture::Free();
}

