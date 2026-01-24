#include "pch.h"
#include "CToonFlash.h"
#include "CRenderer.h"
#include "CProtoMgr.h"

TextureSource CToonFlash::m_TextureSource =
{
	0, L"../Bin/Resource/Texture/Effect/toonGlow.dds", false
};

CToonFlash::CToonFlash(IDirect3DDevice9* device)
	:CParticleEmitter(device, 1), m_curColorIdx(0)
{
	m_vPos = { 0,0,0 };
	m_vSize = { 22.f, 22.f };
	m_fAnimSpeed = 0.f;
	m_iMaxParticle = 1;
	m_iBatchSize = 1;
	m_fLifeTime = 0.08f;
	m_bLoop = true;
	m_color = { 0,0,0,1 };
}

CToonFlash::~CToonFlash()
{
}

HRESULT CToonFlash::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	m_pBufferCom->SetBatchSize(m_iBatchSize);
	m_pBufferCom->SetParticleCount(m_iMaxParticle);
	ChangeState(0);
	AddParticle();
	return S_OK;
}

_int CToonFlash::Update_GameObject(const _float& fTimeDelta)
{
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);
	for (auto iter = m_ActiveList.begin(); iter != m_ActiveList.end(); iter++)
	{
		if ((*iter)->bIsAlive)
		{
			Particle* particle = *iter;
			particle->vPosition = m_vPos;
			particle->fAge += fTimeDelta;
			if (particle->fAge >= particle->fLifeTime)
			{
				particle->fAge = 0.f;
				m_curColorIdx = m_curColorIdx == 0 ? 1 : 0;
				particle->color = m_Color[m_curColorIdx];
			}
		}
	}
	return RET_NONE;
}

void CToonFlash::Render_GameObject()
{
	_matrix I;
	m_pGraphicDev->SetTransform(D3DTS_WORLD, D3DXMatrixIdentity(&I));
	SetPreRenderState();
	CParticleEmitter::Render_GameObject();
	SetPostRenderState();
}

void CToonFlash::SetPreRenderState()
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


void CToonFlash::SetPostRenderState()
{
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
}

void CToonFlash::SetFlashPos(_vec3 _pos)
{
	m_vPos = _pos;
}

HRESULT CToonFlash::Add_Component()
{
	//Vertex
	m_pBufferCom = CDVIBuffer::Create(m_pGraphicDev, m_iMaxParticle, m_iBatchSize);
	if (m_pBufferCom == nullptr) return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", m_pBufferCom });

	m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Effect_ToonFlash_Texture"));
	if (m_pTextureCom == nullptr) return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", m_pTextureCom });

	return S_OK;
}

void CToonFlash::ResetParticle(Particle* particle)
{
	particle->bIsAlive = true;

	particle->vPosition = m_vPos;
	particle->color = m_Color[0];
	particle->fAnimSpeed = 0;
	particle->fAnimTime = 0.f;
	particle->vStartUV = { 0,0 };
	particle->vEndUV = { 1,1 };
	particle->vSize = m_vSize;
	particle->fAge = 0.f;
	particle->fLifeTime = m_fLifeTime;
	particle->bDirection = false;
}

CToonFlash* CToonFlash::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CToonFlash* effect = new CToonFlash(pGraphicDev);

	if (FAILED(effect->Ready_GameObject()))
	{
		Safe_Release(effect);
		MSG_BOX("ToonFlash Create Failed");
		return nullptr;
	}

	return effect;
}
void CToonFlash::Free()
{
	CParticleEmitter::Free();
}


