#include "pch.h"
#include "CRenderer.h"
#include "CProtoMgr.h"
#include "CTrail.h"

TextureSource CTrail::m_TextureSource =
{ 0, L"../Bin/Resource/Texture/Effect/Trail58.dds", false };

CTrail::CTrail(IDirect3DDevice9* device)
	: CParticleEmitter(device, 1)
{
	m_vPos = { 0,0,0 };
	m_vSize = { 0.f, 0.f };
	m_iMaxParticle = 1;
	m_iBatchSize = 1;
	m_fLifeTime = 20.f;
	m_bLoop = false;
}

CTrail::~CTrail()
{
}

HRESULT CTrail::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	m_pBufferCom->SetBatchSize(m_iBatchSize);
	m_pBufferCom->SetParticleCount(m_iMaxParticle);
	m_pTextureCom->Change_Texture(0);

	for (int i = 0; i < m_iMaxParticle; i++) AddParticle();
	return S_OK;
}


_int CTrail::Update_GameObject(const _float& fTimeDelta)
{
	if (IsDead()) return RET_DEAD;

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
	for (auto iter = m_ActiveList.begin(); iter != m_ActiveList.end(); iter++)
	{
		if ((*iter)->bIsAlive)
		{
			(*iter)->fAge += fTimeDelta;
			if ((*iter)->fAge >= (*iter)->fLifeTime)
			{
				(*iter)->bIsAlive = false;
			}
		}
	}

	return RET_NONE;
}

void CTrail::Render_GameObject()
{
	_matrix I;
	D3DXMatrixIdentity(&I);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &I);
	CParticleEmitter::Render_GameObject();
}

void CTrail::SetPreRenderState()
{
}

void CTrail::SetPostRenderState()
{
}

void CTrail::Activate()
{
	CParticleEmitter::Activate();
	m_pTextureCom->Change_Texture(0);
}

void CTrail::SetTrailPos(_vec3 _start, _vec3 _end)
{
	m_vStartPos = _start;
	m_vEndPos = _end;
}

HRESULT CTrail::Add_Component()
{
	//Vertex
	m_pBufferCom = CDVIBuffer::Create(m_pGraphicDev, m_iMaxParticle, m_iBatchSize);
	if (m_pBufferCom == nullptr) return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", m_pBufferCom });

	m_pTextureCom = static_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Effect_Trail_Texture"));
	if (m_pTextureCom == nullptr) return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", m_pTextureCom });

	return S_OK;
}

void CTrail::ResetParticle(Particle* particle)
{
	particle->bIsAlive = true;

	particle->vPosition = m_vStartPos + (m_vEndPos - m_vStartPos) * 0.5f;
	particle->color = m_color;
	particle->fAnimSpeed = 0.f;
	particle->fAnimTime = 0.f;
	particle->vStartUV = { 0,0 };
	particle->vEndUV = { 1.f,1.f };
	particle->fAge = 0.f;
	particle->fLifeTime = m_fLifeTime;

	particle->vSize = m_vSize;
	particle->bDirection = true;
	particle->vDirection = m_vEndPos - m_vStartPos;
	D3DXVec3Normalize(&particle->vDirection, &particle->vDirection);

}

CTrail* CTrail::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTrail* effect = new CTrail(pGraphicDev);

	if (FAILED(effect->Ready_GameObject()))
	{
		Safe_Release(effect);
		MSG_BOX("Trail Create Failed");
		return nullptr;
	}

	return effect;
}

void CTrail::Free()
{
	CParticleEmitter::Free();
}
