#include "pch.h"
#include "CBeamFlare.h"
#include "CRenderer.h"
#include "CProtoMgr.h"

TextureSource CBeamFlare::m_TextureSource =
{
	0, L"../Bin/Resource/Texture/Effect/BeamFlare.dds", false
};

CBeamFlare::CBeamFlare(IDirect3DDevice9* device)
	:CParticleEmitter(device, 1)
{
	m_vPos = { 0,0,0 };
	m_vSize = { 6.f, 6.f };
	m_fAnimSpeed = 0.f;
	m_iMaxParticle = 1;
	m_iBatchSize = 1;
	m_fLifeTime = FLT_MAX;

	m_bLoop = true;
	m_color = { 1,1,1,1 };
}

CBeamFlare::~CBeamFlare()
{
}

HRESULT CBeamFlare::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	m_pBufferCom->SetBatchSize(m_iBatchSize);
	m_pBufferCom->SetParticleCount(m_iMaxParticle);
	ChangeState(0);

	AddParticle();
	return S_OK;
}


_int CBeamFlare::Update_GameObject(const _float& fTimeDelta)
{
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	for (auto iter = m_ActiveList.begin(); iter != m_ActiveList.end(); iter++)
	{
		if ((*iter)->bIsAlive)
		{
			Particle* particle = *iter;
			particle->fAge += fTimeDelta;
			particle->color.a = fabsf(sinf(particle->fAge * m_fSpeed ));

			if (particle->fAge >= particle->fLifeTime)
			{
				particle->bIsAlive = false;
			}
		}
	}
	return RET_NONE;

}
void CBeamFlare::Render_GameObject()
{
	_matrix I;
	m_pGraphicDev->SetTransform(D3DTS_WORLD, D3DXMatrixIdentity(&I));
	SetPreRenderState();
	CParticleEmitter::Render_GameObject();
	SetPostRenderState();
}

void CBeamFlare::SetPreRenderState()
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

void CBeamFlare::SetPostRenderState()
{
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
}

HRESULT CBeamFlare::Add_Component()
{
	//Vertex
	m_pBufferCom = CDVIBuffer::Create(m_pGraphicDev, m_iMaxParticle, m_iBatchSize);
	if (m_pBufferCom == nullptr) return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", m_pBufferCom });

	m_pTextureCom = static_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Effect_BeamFlare_Texture"));
	if (m_pTextureCom == nullptr) return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", m_pTextureCom });

	return S_OK;
}

void CBeamFlare::ResetParticle(Particle* particle)
{
	particle->bIsAlive = true;

	particle->vPosition = m_vPos;
	particle->color = m_color;
	particle->fAnimSpeed = 0;
	particle->fAnimTime = 0.f;
	particle->vStartUV = { 0.f,0.f };
	particle->vEndUV = { 1.f, 1.f };
	particle->vSize = m_vSize;
	particle->fAge = 0.f;
	particle->fLifeTime = m_fLifeTime;
	particle->bDirection = false;
}


CBeamFlare* CBeamFlare::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBeamFlare* effect = new CBeamFlare(pGraphicDev);

	if (FAILED(effect->Ready_GameObject()))
	{
		Safe_Release(effect);
		MSG_BOX("BeamFlare Create Failed");
		return nullptr;
	}

	return effect;
}


void CBeamFlare::Free()
{
	CParticleEmitter::Free();
}
