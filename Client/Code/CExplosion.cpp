#include "pch.h"
#include "CRenderer.h"
#include "CProtoMgr.h"
#include "CExplosion.h"

TextureSource CExplosion::m_TextureSource = { 0, L"../Bin/Resource/Texture/Effect/Exp1.dds", true,3,3,3 };

CExplosion::CExplosion(IDirect3DDevice9* device)
	:CParticleEmitter(device, 10)
{
	m_fAnimSpeed = 0.02f;
	m_iMaxParticle = 10;
	m_iBatchSize = 5;
	m_fLifeTime = 1.2f;
	m_bLoop = false;
}

CExplosion::~CExplosion()
{
}

HRESULT CExplosion::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	m_pBufferCom->SetBatchSize(m_iBatchSize);
	m_pBufferCom->SetParticleCount(m_iMaxParticle);
	ChangeState(0);

	for (int i = 0; i < m_iMaxParticle; i++) AddParticle();
	return S_OK;
}


_int CExplosion::Update_GameObject(const _float& fTimeDelta)
{
	if (IsDead()) return RET_DEAD;

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	for (auto iter = m_ActiveList.begin(); iter != m_ActiveList.end();iter++)
	{
		if ((*iter)->bIsAlive)
		{
			Particle* particle = (*iter);
			particle->fAge += fTimeDelta;
			particle->fAnimTime += fTimeDelta;
			if (particle->fAnimSpeed <= particle->fAnimTime)
			{
				SetNextUV(particle);
				particle->fAnimTime = 0.f;
			}

			if (particle->fAge >= m_fLifeTime)
			{
				particle->bIsAlive = false;
			}
		}
	}

	Compute_ViewZ(&m_vPos);
	return RET_NONE;
}


void CExplosion::Render_GameObject()
{
	_matrix I;
	m_pGraphicDev->SetTransform(D3DTS_WORLD, D3DXMatrixIdentity(&I));
	SetPreRenderState();
	CParticleEmitter::Render_GameObject();
	SetPostRenderState();
}

void CExplosion::SetPreRenderState()
{
	m_pGraphicDev->SetTexture(1, m_pTextureDesc->pTexture);

	//투명 배경 제거 
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	// Alpha = 텍스처 알파 * 버텍스 알파
	// 위의 배경블랜딩을 INVSRCALPHA로 했으므로 
	// Diffuse.Alpha 가 0이면 배경그대로 유지됨
	// 0이 아니면 기존 Alpha Blend 처럼 비율대로 섞임 
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	m_pGraphicDev->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);

	//투명 부분 제거 
	m_pGraphicDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	m_pGraphicDev->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE | D3DTA_ALPHAREPLICATE);

	m_pGraphicDev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);

	m_pGraphicDev->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);
}

void CExplosion::SetPostRenderState()
{
	m_pGraphicDev->SetTexture(1, NULL);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//텍스쳐 색을 쓰기
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

	m_pGraphicDev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	m_pGraphicDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pGraphicDev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
}

void CExplosion::Reset()
{
	int idx = 0;
	for (auto iter = m_Particles.begin(); iter != m_Particles.end(); iter++)
	{
		ResetParticle(&(*iter), idx);
		idx++;
	}
}

HRESULT CExplosion::Add_Component()
{
	//Vertex
	m_pBufferCom = CDVIBuffer::Create(m_pGraphicDev, m_iMaxParticle, m_iBatchSize);
	if (m_pBufferCom == nullptr) return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", m_pBufferCom });

	m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Effect_EXP_Texture"));
	if (m_pTextureCom == nullptr) return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", m_pTextureCom });

	return S_OK;
}

void CExplosion::AddParticle()
{
	for (auto& particle : m_Particles)
	{
		if (particle.bIsAlive == false)
		{
			ResetParticle(&particle, (int)m_ActiveList.size());
			m_ActiveList.push_back(&particle);
			return;
		}
	}
}

void CExplosion::ResetParticle(Particle* particle, int idx)
{
	particle->bIsAlive = true;
	if (idx >= 6)  particle->bFlag = true;
	else particle->bFlag = false;
	_float randX, randY, randSizeX, fSpeed;

	if (particle->bFlag)
	{
		randX = GetRandomFloat(-5.f, 5.f);
		randY = GetRandomFloat(-5.f, 5.f);
		randSizeX = GetRandomFloat(10.f, 20.f);
		fSpeed = GetRandomFloat(m_fAnimSpeed, m_fAnimSpeed*1.5f);
		particle->color = m_RedColor;
		particle->fAnimSpeed = fSpeed;
	}
	else
	{
		randX = GetRandomFloat(-8.f, 8.f);
		randY = GetRandomFloat(-8.f, 8.f);
		randSizeX = GetRandomFloat(20.f, 30.f);
		fSpeed = GetRandomFloat(m_fAnimSpeed * 2.f, m_fAnimSpeed * 3.f);
		particle->color = m_GrayColor;
		particle->fAnimSpeed = fSpeed;
	}

	particle->vPosition = m_vPos + _vec3(randX, randY, 0);
	particle->vSize = { randSizeX, randSizeX };

	particle->fAnimTime = 0;
	particle->vStartUV = { 0,0 };
	particle->vEndUV = m_pTextureDesc->vUVoffset;
	particle->fAge = 0.f;
	particle->fLifeTime = m_fLifeTime;
	particle->bDirection = false;
}

void CExplosion::ResetParticle(Particle* particle)
{
	ResetParticle(particle, 0);
}

CExplosion* CExplosion::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CExplosion* effect = new CExplosion(pGraphicDev);

	if (FAILED(effect->Ready_GameObject()))
	{
		Safe_Release(effect);
		MSG_BOX("Explosion Create Failed");
		return nullptr;
	}

	return effect;
}


void CExplosion::Free()
{
	CParticleEmitter::Free();
}
