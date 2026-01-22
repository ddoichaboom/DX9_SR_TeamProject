#include "pch.h"
#include "CBossTrail.h"
#include "CRenderer.h"
#include "CProtoMgr.h"

TextureSource CBossTrail::m_TextureSource =
	{ 0, L"../Bin/Resource/Texture/Effect/Boss_Img.dds", false };


CBossTrail::CBossTrail(IDirect3DDevice9* device)
	:CParticleEmitter(device, 5)
{
	m_vPos = { 0,0,0 };
	m_vSize = { 50.f, 50.f };
	m_fAnimSpeed = 10.f;
	m_iMaxParticle = 5;
	m_iBatchSize = 5;
	m_fLifeTime = 200.f;
	m_bLoop = false;
	m_color = { 0,0,0,1 };
}

CBossTrail::~CBossTrail()
{
}

HRESULT CBossTrail::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	m_pBufferCom->SetBatchSize(m_iBatchSize);
	m_pBufferCom->SetParticleCount(m_iMaxParticle);
	ChangeState(0);

	for (int i = 0; i < m_iMaxParticle; i++) AddParticle();
	return S_OK;
}

_int CBossTrail::Update_GameObject(const _float& fTimeDelta)
{
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	for (auto iter = m_ActiveList.begin(); iter != m_ActiveList.end(); iter++)
	{
		if ((*iter)->bIsAlive)
		{
			(*iter)->fAge += fTimeDelta;
			(*iter)->fAnimTime += fTimeDelta;
			if ((*iter)->fAnimSpeed <= (*iter)->fAnimTime)
			{
				SetNextUV((*iter));
				(*iter)->fAnimTime = 0.f;
			}
			if ((*iter)->fAge >= m_fLifeTime)
			{
				(*iter)->bIsAlive = false;
			}
		}
	}

	Compute_ViewZ(&m_vPos);
	return RET_NONE;
}

void CBossTrail::Render_GameObject()
{
	_matrix I;
	D3DXMatrixIdentity(&I);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &I);
	SetPreRenderState();
	CParticleEmitter::Render_GameObject();
	SetPostRenderState();
}

void CBossTrail::SetPreRenderState()
{
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE); 
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE); 

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

}

void CBossTrail::SetPostRenderState()
{
}

HRESULT CBossTrail::Add_Component()
{
	return E_NOTIMPL;
}

void CBossTrail::ResetParticle(Particle* particle)
{
}

CBossTrail* CBossTrail::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBossTrail* effect = new CBossTrail(pGraphicDev);

	if (FAILED(effect->Ready_GameObject()))
	{
		Safe_Release(effect);
		MSG_BOX("BossTrail Create Failed");
		return nullptr;
	}

	return effect;
}

void CBossTrail::Free()
{
	CParticleEmitter::Free();
}
