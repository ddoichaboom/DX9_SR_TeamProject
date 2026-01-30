#include "pch.h"
#include "CRenderer.h"
#include "CProtoMgr.h"
#include "CBodyEmit.h"

vector<TextureSource> CBodyEmit::m_TextureSources =
{
	{MON_PART, L"../Bin/Resource/Texture/Effect/SteelParts_64.dds", true,1,3,3},
	{GLASS_PART, L"../Bin/Resource/Texture/Effect/GlassParts_126.dds", true,  1,1,1},
};

CBodyEmit::CBodyEmit(IDirect3DDevice9* device)
	:CParticleEmitter(device, 10)
{
	m_vPos = { 0,0,0 };
	m_vSize = { 3.f, 3.f };
	m_iMaxParticle = 10;
	m_iBatchSize = 5;
	m_fLifeTime = 7.f;

	m_bLoop = true;
}

CBodyEmit::~CBodyEmit()
{
}

HRESULT CBodyEmit::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	m_pBufferCom->SetBatchSize(m_iBatchSize);
	m_pBufferCom->SetParticleCount(m_iMaxParticle);
	ChangeState(MON_PART);

	for (int i = 0; i < m_iMaxParticle; i++) AddParticle();
	return S_OK;
}

_int CBodyEmit::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bDead) return RET_DEAD;

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);
	bool bActive = false;
	for (auto iter = m_ActiveList.begin(); iter != m_ActiveList.end(); iter++)
	{
		Particle* particle = (*iter);
		if (particle->bIsAlive)
		{
			if (!bActive) bActive = true;
			_float val = m_fGravity * fTimeDelta * fTimeDelta * m_fSpeed;
			particle->vDirection.y -= val;

			particle->vPosition += particle->vDirection * fTimeDelta* m_fSpeed;
			particle->fAge += fTimeDelta;

			if (particle->fAge >= particle->fLifeTime)
			{
				particle->bIsAlive = false;
			}
		}
	}
	if (!bActive)
	{
		m_bDead = true;
		return RET_DEAD;
	}

	return RET_NONE;
}


void CBodyEmit::Render_GameObject()
{
	_matrix I;
	D3DXMatrixIdentity(&I);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &I);
	CParticleEmitter::Render_GameObject();
}

void CBodyEmit::SetPreRenderState()
{
}

void CBodyEmit::SetPostRenderState()
{
}

void CBodyEmit::Reset()
{
	CParticleEmitter::Reset();
}

HRESULT CBodyEmit::Add_Component()
{
	//Vertex
	m_pBufferCom = CDVIBuffer::Create(m_pGraphicDev, m_iMaxParticle, m_iBatchSize);
	if (m_pBufferCom == nullptr) return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", m_pBufferCom });

	m_pTextureCom = static_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Effect_BodyEmit_Texture"));
	if (m_pTextureCom == nullptr) return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", m_pTextureCom });

	return S_OK;
}

void CBodyEmit::ResetParticle(Particle* particle)
{
	static _vec3 minDir = { 0,0,0 };
	static _vec3 maxDir = { 0,0,0 };

	particle->bIsAlive = true;
	particle->vPosition = m_vPos;

	if (m_iState == GLASS_PART)
	{
		_float randX = GetRandomFloat(m_vRangeX.x, m_vRangeX.y);
		particle->vPosition.x += randX;
		particle->vDirection = { 0,-1,0 };
		minDir = { -0.5f, 0, 0 };
		maxDir = { 0.5f, 1, 0 };
	}
	else
	{
		minDir = { -1,0,-1 };
		maxDir = { 1,1,1 };
	}
	GetRandomVector(&particle->vDirection, &minDir, &maxDir);
	D3DXVec3Normalize(&particle->vDirection, &particle->vDirection);

	particle->color = { 0,0,0,1 };
	particle->fAnimTime = 0.f;
	particle->fAnimSpeed = 0.f;
	particle->vSize = m_vSize;
	particle->fAge = 0.f;
	particle->fLifeTime = m_fLifeTime;

	//Direction으로 정점 회전 변환하지 않을거라 false 로 두기 
	particle->bDirection = false;
	if (!m_pTextureDesc) return;

	_vec2 maxFrame = m_pTextureDesc->vMaxIdx;
	_vec2 randFrame;
	randFrame.x = rand() % (int)maxFrame.x;
	randFrame.y = rand() % (int)maxFrame.y;
	_vec2 uvOffset = m_pTextureDesc->vUVoffset;
	particle->vStartUV = { randFrame.x * uvOffset.x, randFrame.y * uvOffset.y };
	particle->vEndUV = { particle->vStartUV.x + uvOffset.x, particle->vStartUV.y + uvOffset.y };
}

CBodyEmit* CBodyEmit::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBodyEmit* effect = new CBodyEmit(pGraphicDev);

	if (FAILED(effect->Ready_GameObject()))
	{
		Safe_Release(effect);
		MSG_BOX("BodyEmit Create Failed");
		return nullptr;
	}

	return effect;
}

void CBodyEmit::Free()
{
	CParticleEmitter::Free();
}
