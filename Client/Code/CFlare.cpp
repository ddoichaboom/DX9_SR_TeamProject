#include "pch.h"
#include "CFlare.h"
#include "CRenderer.h"
#include "CProtoMgr.h"

TextureSource CFlare::m_TextureSource =
{
	0, L"../Bin/Resource/Texture/Effect/muzzleFlare.dds", true,1,1,1
};

CFlare::CFlare(IDirect3DDevice9* device)
	:CParticleEmitter(device, 6), m_fFlareTime(0.04f)
{
	m_vPos = { 0,0,0 };
	m_iMaxParticle = 6;
	m_iBatchSize = 3;
	m_fLifeTime = 0.07f;
	m_bLoop = false;
}

CFlare::~CFlare()
{
}

HRESULT CFlare::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	m_pBufferCom->SetBatchSize(m_iBatchSize);
	m_pBufferCom->SetParticleCount(m_iMaxParticle);
	ChangeState(0);

	for (int i = 0; i < m_iMaxParticle; i++) AddParticle();
	return S_OK;
}

_int CFlare::Update_GameObject(const _float& fTimeDelta)
{
	if (IsDead()) return RET_DEAD;

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
	for (auto iter = m_ActiveList.begin(); iter != m_ActiveList.end(); iter++)
	{
		if ((*iter)->bIsAlive)
		{
			Particle* particle = *iter;
			particle->fAge += fTimeDelta;
			particle->fAnimTime += fTimeDelta;
			if (particle->fAnimTime > m_fFlareTime)
			{
				particle->fAnimTime = 0.f;
				particle->vSize.x *= 0.7f;
				particle->vSize.y *= 0.7f;
				_vec3 dir = particle->vDirection;
				particle->vPosition = m_vPos - _vec3(particle->vSize.x * -0.3f * dir.y, particle->vSize.x * 0.3f * dir.x, 0);
			}
			if (particle->fAge >= particle->fLifeTime)
			{
				particle->bIsAlive = false;
			}
		}
	}

	return RET_NONE;
}

void CFlare::Render_GameObject()
{
	_matrix I;
	m_pGraphicDev->SetTransform(D3DTS_WORLD, D3DXMatrixIdentity(&I));
	SetPreRenderState();
	CParticleEmitter::Render_GameObject();
	SetPostRenderState();
}

void CFlare::SetPreRenderState()
{
	m_pGraphicDev->SetTexture(1, m_pTextureDesc->pTexture);

	//투명 배경 제거 + 반투명x
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	//1번 텍스쳐를 사용할때 UV는 0번 UV사용하기 
	m_pGraphicDev->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);

	// 현재 색 + 텍스처의 알파값을 더해서 알파가 높은곳이 더 밝게 함 
	// D3DTOP_ADDSIGNED -0.5~ 0.5 범위를 갖게 함 
	m_pGraphicDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_ADDSIGNED);
	m_pGraphicDev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	m_pGraphicDev->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE | D3DTA_ALPHAREPLICATE);

	// 투명도 유지 
	m_pGraphicDev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);

	m_pGraphicDev->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pGraphicDev->SetTextureStageState(2, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
}

void CFlare::SetPostRenderState()
{
	m_pGraphicDev->SetTexture(1, NULL);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	//텍스쳐 색을 쓰기
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	m_pGraphicDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pGraphicDev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
}

HRESULT CFlare::Add_Component()
{
	//Vertex
	m_pBufferCom = CDVIBuffer::Create(m_pGraphicDev, m_iMaxParticle, m_iBatchSize);
	if (m_pBufferCom == nullptr) return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", m_pBufferCom });

	m_pTextureCom = static_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Effect_Flare_Texture"));
	if (m_pTextureCom == nullptr) return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", m_pTextureCom });

	return S_OK;
}

void CFlare::ResetParticle(Particle* particle)
{
	static _vec3 minVec = { -1,-1,0 };
	static _vec3 maxVec = { 1,1,0 };

	particle->bIsAlive = true;

	particle->vPosition = m_vPos;
	particle->color = m_StartColor;
	particle->fAnimSpeed = 0;
	particle->fAnimTime = 0.f;

	_int randX = rand() % 2;
	_int randY = rand() % 2;
	_float randSizeX = GetRandomFloat(400.f, 500.f);
	_float randSizeY = GetRandomFloat(500.f, 700.f);
	_vec3 randDir;

	GetRandomVector(&randDir, &minVec, &maxVec);
	D3DXVec3Normalize(&randDir, &randDir);

	particle->vStartUV = { randX * m_pTextureDesc->vUVoffset.x, randY * m_pTextureDesc->vUVoffset.y };
	particle->vEndUV = { particle->vStartUV.x + m_pTextureDesc->vUVoffset.x,  particle->vStartUV.y + m_pTextureDesc->vUVoffset.y };
	particle->vSize = { randSizeX, randSizeY };
	particle->vPosition -= {randSizeX * -0.3f * randDir.y, randSizeX * 0.3f * randDir.x, 0 };
	particle->bDirection = true;
	particle->vDirection = randDir;
	particle->fAge = 0.f;
	particle->fLifeTime = m_fLifeTime;
}

CFlare* CFlare::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CFlare* effect = new CFlare(pGraphicDev);

	if (FAILED(effect->Ready_GameObject()))
	{
		Safe_Release(effect);
		MSG_BOX("Flare Create Failed");
		return nullptr;
	}

	return effect;
}
void CFlare::Free()
{
	CParticleEmitter::Free();
}
