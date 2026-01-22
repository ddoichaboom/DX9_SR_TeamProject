#include "pch.h"
#include "CTakeDownBlood.h"
#include "CRenderer.h"
#include "CProtoMgr.h"

//두장같은한장
TextureSource CTakeDownBlood::m_TextureSource =
{ 0, L"../Bin/Resource/Texture/TakeDown/TakeDownBlood.dds", true,5,2,2 };


CTakeDownBlood::CTakeDownBlood(IDirect3DDevice9* device)
	:CParticleEmitter(device,10)
{
	m_vPos = { 0,0,0};
	m_vSize = { 200,200 };
	m_color = { 1.f, 0.f,0.f,1.f };
	m_fAnimSpeed = 1.5f;
	m_iMaxParticle = 10;
	m_iBatchSize = 5;
	m_bLoop = false;
}

CTakeDownBlood::~CTakeDownBlood()
{
}

HRESULT CTakeDownBlood::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	m_pBufferCom->SetBatchSize(m_iBatchSize);
	m_pBufferCom->SetParticleCount(m_iMaxParticle);
	ChangeState(0);
	
	for (int i = 0; i < m_iBatchSize -1; i++) AddParticle();

	m_iBloodType = BLOOD2;
	AddParticle();
	m_iBloodType = BLOOD1;
	return S_OK;
}

_int CTakeDownBlood::Update_GameObject(const _float& fTimeDelta)
{
	//if (IsDead()) return RET_DEAD;

	if (!m_bSecondHit)
	{
		m_fTime += fTimeDelta;
		if (m_fTime >= m_fSecondTime)
		{
			m_bSecondHit = true;
			m_vDiffSum = { 0,0,0 };
			for (int i = 0; i < m_iBatchSize -1 ; i++) AddParticle();
			m_iBloodType = BLOOD2;
			AddParticle();
			return RET_NONE;
		}
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
	for (auto iter = m_ActiveList.begin(); iter != m_ActiveList.end(); iter++)
	{
		if ((*iter)->bIsAlive)
		{
			(*iter)->fAnimTime += fTimeDelta;
			if ((*iter)->fAnimSpeed <= (*iter)->fAnimTime)
			{
				(*iter)->fAnimTime = 0.f;
				SetNextUV((*iter));
			}
		}
	}

	return RET_NONE;
}

void CTakeDownBlood::Render_GameObject()
{
	_matrix I;
	D3DXMatrixIdentity(&I);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &I);
	SetPreRenderState();
	CParticleEmitter::Render_GameObject();
	SetPostRenderState();
}

void CTakeDownBlood::SetPreRenderState()
{
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
}

void CTakeDownBlood::SetPostRenderState()
{
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

}

void CTakeDownBlood::Deactivate()
{
	CParticleEmitter::Deactivate();
	m_fTime = 0.f;
	m_vDiffSum = { 0,0,0 };
	m_bSecondHit = false;
}

void CTakeDownBlood::Reset()
{
	for (auto& particle : m_ActiveList)
	{
		particle->bIsAlive = false;
	}
	m_ActiveList.clear();

	m_bSecondHit = false;
	m_fTime = 0.f;
	m_vDiffSum = { 0,0,0 };

	m_iBloodType = BLOOD1;
	for (int i = 0; i < m_iBatchSize - 1; i++) AddParticle();
	m_iBloodType = BLOOD2;
	AddParticle();
	m_iBloodType = BLOOD1;

}

void CTakeDownBlood::SetNextUV(Particle* pParticle)
{
	if (!pParticle || !m_pTextureDesc) return;
	_vec2 curUV = pParticle->vStartUV;
	_vec2 uvOffset = m_pTextureDesc->vUVoffset;
	curUV.x += uvOffset.x;
	if (curUV.x >= 1.f)
	{
		curUV.x = 0.f;
		curUV.y += uvOffset.y;
		if (pParticle->bFlag && curUV.y >=1.f)
		{
			if (m_bLoop == false)
			{
				pParticle->bIsAlive = false;
				return;
			}
		}
		else if (!pParticle->bFlag && curUV.y >=0.5f)
		{
			if (m_bLoop == false)
			{
				pParticle->bIsAlive = false;
				return;
			}
		}
	}
	pParticle->vStartUV = curUV;
	pParticle->vEndUV = curUV + uvOffset;
}

HRESULT CTakeDownBlood::Add_Component()
{
	//Vertex
	m_pBufferCom = CDVIBuffer::Create(m_pGraphicDev, m_iMaxParticle, m_iBatchSize);
	if (m_pBufferCom == nullptr) return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", m_pBufferCom });

	m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Effect_TakeDownBlood_Texture"));
	if (m_pTextureCom == nullptr) return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", m_pTextureCom });

	return S_OK;
}

void CTakeDownBlood::ResetParticle(Particle* particle)
{
	static _vec3 minVec = { -10.f,10.f,0.f };
	static _vec3 maxVec = { -2.f,2.f,0.f };

	particle->bIsAlive = true;
	particle->bFlag = (m_iBloodType == BLOOD2);
	if (m_iBloodType == BLOOD1)
	{
		if (m_bSecondHit) particle->vPosition = m_vPos + m_vSecondPosOffset + m_vDiffSum;
		else particle->vPosition = m_vPos + m_vDiffSum;
		_vec3 randVec;
		GetRandomVector(&randVec, &minVec, &maxVec);
		m_vDiffSum += randVec;
	}
	else
	{
		if (m_bSecondHit) particle->vPosition = m_vPos + m_vSecondPosOffset + m_vBlood2PosOffset;
		else particle->vPosition = m_vPos + m_vBlood2PosOffset;
	}

	particle->color = m_color;
	particle->fAnimSpeed = m_fAnimSpeed;
	particle->fAnimTime = 0.f;
	particle->vStartUV =
	{ m_StartFrame[m_iBloodType].x * m_pTextureDesc->vUVoffset.x , m_StartFrame[m_iBloodType].y * m_pTextureDesc->vUVoffset.y };
	particle->vEndUV = particle->vStartUV + m_pTextureDesc->vUVoffset;
	particle->vSize = m_vSize;
	particle->fAge = 0.f;
	particle->fLifeTime = 0.f;

	particle->bDirection = true;
	particle->vDirection = (m_bSecondHit ? _vec3(1, 1, 0) : _vec3(0, 1, 0));
	D3DXVec3Normalize(&particle->vDirection, &particle->vDirection);
}

CTakeDownBlood* CTakeDownBlood::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTakeDownBlood* effect = new CTakeDownBlood(pGraphicDev);

	if (FAILED(effect->Ready_GameObject()))
	{
		Safe_Release(effect);
		MSG_BOX("TakeDownBlood Create Failed");
		return nullptr;
	}

	return effect;

}
void CTakeDownBlood::Free()
{
	CParticleEmitter::Free();
}
