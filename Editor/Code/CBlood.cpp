#include "pch.h"
#include "CParticleEmitter.h"
#include "CRenderer.h"
#include "CProtoMgr.h"
#include "CBlood.h"

TextureSource CBlood::m_TextureSource
{
	0, L"../Bin/Resource/Texture/Effect/Blood1.dds", true,3,3,3, {0.f, 0.f} 
};

CBlood::CBlood(IDirect3DDevice9* device, _vec3* _origin, int numParticles)
	:CParticleEmitter(device, numParticles)
{
	m_vOrigin = *_origin;
	m_vPos = m_vOrigin;
	m_fSize = 0.3f;
	m_iMaxParticle = 2;
	m_iBatchSize = 2;
	m_vVelocity = { 0,0,1 };
	m_fLifeTime = 5.f;
	m_bLoop = false;
	m_color = { 255,0,0,1 };

	for (int i = 0; i < numParticles; i++) AddParticle();
}

CBlood::~CBlood()
{
}

HRESULT CBlood::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	m_pBufferCom->SetBatchSize(m_iBatchSize);
	m_pBufferCom->SetParticleCount(m_iMaxParticle);
	m_pTextureCom->Change_Texture(0);
	return S_OK;
}

_int CBlood::Update_GameObject(const _float& fTimeDelta)
{
	std::list<Particle*>::iterator i;
	for (i = m_ActiveList.begin(); i != m_ActiveList.end(); i++)
	{
		if ((*i)->bIsAlive)
		{
			SetNextUV((*i));
			(*i)->fAge += fTimeDelta;
			if ((*i)->fAge > (*i)->fLifeTime)
				(*i)->bIsAlive = false;
		}
	}
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}


void CBlood::Render_GameObject()
{
	SetPreRenderState();
	CParticleEmitter::Render_GameObject();
	SetPostRenderState();
}

void CBlood::SetPreRenderState()
{
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
}

void CBlood::SetPostRenderState()
{
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

HRESULT CBlood::Add_Component()
{
	//Vertex
	m_pBufferCom = CDVIBuffer::Create(m_pGraphicDev, m_iMaxParticle, m_iBatchSize);
	if (m_pBufferCom == nullptr) return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", m_pBufferCom });

	m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Effect_Blood_Texture"));
	if (m_pTextureCom == nullptr) return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", m_pTextureCom });

	return S_OK;
}

void CBlood::ResetParticle(Particle* particle)
{
	particle->bIsAlive = true;

	particle->vPosition = m_vOrigin;
	D3DXVECTOR3 min = D3DXVECTOR3(-1.f, -1.f, -1.f);
	D3DXVECTOR3 max = D3DXVECTOR3(1.f, 1.f, 1.f);

	GetRandomVector(&particle->vVelocity, &min, &max);
	D3DXVec3Normalize(&particle->vVelocity, &particle->vVelocity);
	particle->vVelocity *= 2.f;

	particle->color = { 1,0,0,1 };
	particle->vStartUV = { 0,0 };
	particle->vEndUV = { 1,1 };
	particle->fSize = m_fSize;
	particle->fAge = 0.f;
	particle->fLifeTime = 5.f;
}

CBlood* CBlood::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3* _origin, int numParticles)
{
	CBlood* effect = new CBlood(pGraphicDev, _origin, numParticles);

	if (FAILED(effect->Ready_GameObject()))
	{
		Safe_Release(effect);
		MSG_BOX("Blood Create Failed");
		return nullptr;
	}

	return effect;
}


void CBlood::Free()
{
	CParticleEmitter::Free();
}
