#include "pch.h"
#include "CSodaUI.h"
#include "CRenderer.h"
#include "CProtoMgr.h"

TextureSource CSodaUI::m_TextureSource =
{ 0, L"../Bin/Resource/Texture/Effect/LIKES.dds",false };

CSodaUI::CSodaUI(IDirect3DDevice9* device)
	:CParticleEmitter(device, 30)
{
	m_vPos = {0,0,0};
	m_vSize = { 50, 50 };
	m_fAnimSpeed = 2.f;
	m_iMaxParticle = 30;
	m_iBatchSize = 5;
	m_bLoop = false;

	m_fSpeed = 7.f;
	m_fLifeTime = 15.f;
	m_fCreateTime = 1.5f;
	m_fLerpStartTime = m_fLifeTime * 0.5f;
}

CSodaUI::~CSodaUI()
{
}

HRESULT CSodaUI::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	m_pBufferCom->SetBatchSize(m_iBatchSize);
	m_pBufferCom->SetParticleCount(m_iMaxParticle);
	ChangeState(0);

	return S_OK;
}

_int CSodaUI::Update_GameObject(const _float& fTimeDelta)
{
	//if (IsDead()) return RET_DEAD;

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
	m_fTime += fTimeDelta;
	if (m_iIdx < m_iMaxParticle && m_fTime >= m_fCreateTime)
	{
		m_iIdx++;
		m_fTime = 0.f;
		AddParticle();
	}

	for (auto iter = m_ActiveList.begin(); iter != m_ActiveList.end();)
	{
		Particle* particle = (*iter);
		if (particle->bIsAlive)
		{
			particle->fAge += fTimeDelta;
			particle->vPosition.y += m_fSpeed * fTimeDelta;

			if (particle->fAge >= particle->fLifeTime)
			{
				particle->bIsAlive = false;
			}
			else if (particle->fAge >= m_fLerpStartTime)
			{
				_float ratio = particle->fAge / particle->fLifeTime;
				D3DXVec4Lerp((_vec4*)&particle->color, (_vec4*)& m_vStartColor, (_vec4*)&m_vEndColor, ratio);
			}
		}

		if (!particle->bIsAlive) iter = m_ActiveList.erase(iter);
		else iter++;
	}

	return RET_NONE;
}

void CSodaUI::Render_GameObject()
{
	_matrix I;
	D3DXMatrixIdentity(&I);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &I);
	SetPreRenderState();
	CParticleEmitter::Render_GameObject();
	SetPostRenderState();
}

void CSodaUI::SetPreRenderState()
{
	m_pGraphicDev->SetTexture(1, m_pTextureDesc->pTexture);

	//투명 배경 제거 
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

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

void CSodaUI::SetPostRenderState()
{
	m_pGraphicDev->SetTexture(1, NULL);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//텍스쳐 색을 쓰기
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTA_TEXTURE);

	m_pGraphicDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pGraphicDev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
}

void CSodaUI::Reset()
{
	for (auto iter = m_ActiveList.begin(); iter != m_ActiveList.end(); iter++)
	{
		(*iter)->bIsAlive = false;
	}
	m_ActiveList.clear();
	m_fTime = 0.f;
	m_iIdx = 0;
}

HRESULT CSodaUI::Add_Component()
{
	//Vertex
	m_pBufferCom = CDVIBuffer::Create(m_pGraphicDev, m_iMaxParticle, m_iBatchSize);
	if (m_pBufferCom == nullptr) return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", m_pBufferCom });

	m_pTextureCom = static_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Effect_SodaUI_Texture"));
	if (m_pTextureCom == nullptr) return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", m_pTextureCom });

	return S_OK;
}

void CSodaUI::ResetParticle(Particle* particle)
{
	particle->bIsAlive = true;

	_float posX = GetRandomFloat(WINCX*-0.5f, WINCX*0.5f);
	particle->vPosition = { posX, -WINCY*0.5f + 100, 0 };
	particle->color = m_vStartColor;
	particle->fAnimSpeed = 0.f;
	particle->fAnimTime = 0.f;
	particle->vStartUV = { 0,0 };
	particle->vEndUV = { 1,1 };
	particle->vSize = m_vSize;
	particle->fAge = 0.f;
	particle->fLifeTime = m_fLifeTime;
}

CSodaUI* CSodaUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSodaUI* effect = new CSodaUI(pGraphicDev);

	if (FAILED(effect->Ready_GameObject()))
	{
		Safe_Release(effect);
		MSG_BOX("SodaUI Create Failed");
		return nullptr;
	}

	return effect;
}

void CSodaUI::Free()
{
	CParticleEmitter::Free();
}
