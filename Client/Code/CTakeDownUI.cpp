#include "pch.h"
#include "CTakeDownUI.h"
#include "CRenderer.h"
#include "CProtoMgr.h"

TextureSource CTakeDownUI::m_TextureSource =
{ 0, L"../Bin/Resource/Texture/Effect/TakeDown_UI.dds", false };

CTakeDownUI::CTakeDownUI(IDirect3DDevice9* device)
	:CParticleEmitter(device, 1)
{
	m_vPos = { 0.f, WINCY * 0.35f,0.f };
	m_vSize = { 360.f,180.f };
	m_fAnimSpeed = 1.f;
	m_iMaxParticle = 1;
	m_iBatchSize = 1;
	m_fLifeTime = 1000;
	m_bLoop = false;

	m_fTime = m_fEffectTime;
}

CTakeDownUI::~CTakeDownUI()
{
}

HRESULT CTakeDownUI::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	m_pBufferCom->SetBatchSize(m_iBatchSize);
	m_pBufferCom->SetParticleCount(m_iMaxParticle);
	ChangeState(0);
	AddParticle();

	return S_OK;
}

_int CTakeDownUI::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bDead) return RET_NONE;
	if (m_ActiveList.empty()) return RET_NONE;

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	Particle* particle = *m_ActiveList.begin();
	if (particle->bIsAlive)
	{
		m_fTime += fTimeDelta;
		particle->vPosition = m_vPos;
		if (m_fTime >= m_fEffectTime)
		{
			m_fTime = 0.f;
			m_startColor = m_RGBs[m_iColorIdx];
			if (++m_iColorIdx >= COLOR_CNT) m_iColorIdx = 0;
			m_endColor = m_RGBs[m_iColorIdx];
		}
		D3DXCOLOR lerpColor;
		D3DXVec4Lerp((_vec4*)(&lerpColor), (_vec4*)(&m_startColor), (_vec4*)(&m_endColor), m_fTime / m_fEffectTime);
		particle->color = lerpColor;
	}
	else m_bDead = true;

	return RET_NONE;
}

void CTakeDownUI::Render_GameObject()
{
	_matrix I;
	m_pGraphicDev->SetTransform(D3DTS_WORLD, D3DXMatrixIdentity(&I));

	SetPreRenderState();
	CParticleEmitter::Render_GameObject();
	SetPostRenderState();

}

void CTakeDownUI::SetPreRenderState()
{
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

}

void CTakeDownUI::SetPostRenderState()
{
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
}

void CTakeDownUI::SetPos(_vec3 _pos)
{
	m_vPos = { _pos.x - WINCX * 0.5f, -_pos.y + WINCY * 0.5f, 0.f };
}

void CTakeDownUI::Reset()
{
	m_fTime = m_fEffectTime;
	m_iColorIdx = 0;
	CParticleEmitter::Reset();
}

HRESULT CTakeDownUI::Add_Component()
{
	//Vertex
	m_pBufferCom = CDVIBuffer::Create(m_pGraphicDev, m_iMaxParticle, m_iBatchSize);
	if (m_pBufferCom == nullptr) return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", m_pBufferCom });

	m_pTextureCom = static_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Effect_TakeDownUI_Texture"));
	if (m_pTextureCom == nullptr) return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", m_pTextureCom });

	return S_OK;
}

void CTakeDownUI::ResetParticle(Particle* particle)
{
	particle->bIsAlive = true;

	particle->vPosition = m_vPos;
	particle->color = m_color;
	particle->fAnimSpeed = m_fAnimSpeed;
	particle->fAnimTime = 0.f;

	particle->vSize = m_vSize;
	particle->fAge = 0.f;
	particle->fLifeTime = m_fLifeTime;
	particle->vStartUV = { 0,0 };
	particle->vEndUV = { 1,1 };


}

CTakeDownUI* CTakeDownUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTakeDownUI* effect = new CTakeDownUI(pGraphicDev);

	if (FAILED(effect->Ready_GameObject()))
	{
		Safe_Release(effect);
		MSG_BOX("Take Down UI Create Failed");
		return nullptr;
	}

	return effect;
}

void CTakeDownUI::Free()
{
	CParticleEmitter::Free();
}
