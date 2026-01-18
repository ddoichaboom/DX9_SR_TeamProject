#include "pch.h"
#include "CTestEffect.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CTestEffect::CTestEffect(IDirect3DDevice9* device, _vec3* _origin, int numParticles)
	:CParticleEmitter(device,numParticles)
{
	m_vOrigin = *_origin;
	m_fSize = 0.3f;
	m_iMaxParticle = 2048;
	m_iBatchSize = 512;

	for (int i = 0; i < numParticles; i++) AddParticle();
}

HRESULT CTestEffect::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	m_pBufferCom->SetBatchSize(m_iBatchSize);
	m_pBufferCom->SetParticleCount(m_iMaxParticle);
	m_pTextureCom->Change_Texture(0);

	return S_OK;
}


_int CTestEffect::Update_GameObject(const _float& fTimeDelta)
{
	std::list<Particle*>::iterator i;
	for (i = m_ActiveList.begin(); i != m_ActiveList.end(); i++)
	{
		if ((*i)->bIsAlive)
		{
			(*i)->vPosition += (*i)->vVelocity * fTimeDelta;
			(*i)->fAge += fTimeDelta;
			if ((*i)->fAge > (*i)->fLifeTime)
				(*i)->bIsAlive = false;
		}
	}
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

	return 0;
}
void CTestEffect::Render_GameObject()
{
	SetPreRenderState();
	CParticleEmitter::Render_GameObject();
}

void CTestEffect::SetPreRenderState()
{
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
}


HRESULT CTestEffect::Add_Component()
{
	//Vertex
	m_pBufferCom = CDVIBuffer::Create(m_pGraphicDev, m_iMaxParticle, m_iBatchSize);
	if (m_pBufferCom == nullptr) return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", m_pBufferCom });

	m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_FireworkTexture"));
	if (m_pTextureCom == nullptr) return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", m_pTextureCom });

	return S_OK;
}

CTestEffect* CTestEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3* _origin, int numParticles)
{
	CTestEffect* effect = new CTestEffect(pGraphicDev, _origin, numParticles);

	if (FAILED(effect->Ready_GameObject()))
	{
		Safe_Release(effect);
		MSG_BOX("effect Create Failed");
		return nullptr;
	}

	return effect;
}

void CTestEffect::ResetParticle(Particle* particle)
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
	particle->vEndUV = { 1,1};
	particle->fSize = m_fSize;
	particle->fAge = 0.f;
	particle->fLifeTime = 5.f;
}

void CTestEffect::Free()
{
	CParticleEmitter::Free();
}

