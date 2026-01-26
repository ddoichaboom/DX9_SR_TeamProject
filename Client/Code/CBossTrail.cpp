#include "pch.h"
#include "CBossTrail.h"
#include "CRenderer.h"
#include "CProtoMgr.h"

TextureSource CBossTrail::m_TextureSource =
{ 0, L"../Bin/Resource/Texture/Effect/Boss_Img.dds", false };


CBossTrail::CBossTrail(IDirect3DDevice9* device)
	:CParticleEmitter(device, 10), m_fCreateTime(0.06f), m_pOwnerTransform(nullptr)
{
	m_vPos = { 0,0,0 };
	m_vSize = { 100.f,100.f };
	m_fAnimSpeed = 0.f;
	m_iMaxParticle = 10;
	m_iBatchSize = 10;
	m_fLifeTime = 50.f;
	m_bLoop = false;
	m_fFadeSpeed = 2.f;
	m_color = { 0,0,0,1 };
}

CBossTrail::~CBossTrail()
{
}

HRESULT CBossTrail::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	m_pBufferCom->SetBatchSize(m_iBatchSize);
	m_pBufferCom->SetParticleCount(m_iMaxParticle);
	ChangeState(0);

	return S_OK;
}

_int CBossTrail::Update_GameObject(const _float& fTimeDelta)
{
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

	m_fTime += fTimeDelta;
	if (m_fTime >= m_fCreateTime)
	{
		m_fTime = 0.f;
		AddParticle();
	}

	for (auto iter = m_ActiveList.begin(); iter != m_ActiveList.end();)
	{
		if ((*iter)->bIsAlive)
		{
			Particle* particle = *iter;
			particle->fAge += fTimeDelta;
			particle->color.a -= fTimeDelta * m_fFadeSpeed;
			if (particle->color.a <= 0.f)
			{
				particle->bIsAlive = false;
			}
		}
		if ((*iter)->bIsAlive == false)
		{
			iter = m_ActiveList.erase(iter);
		}
		else iter++;
	}
	return RET_NONE;
}

void CBossTrail::Render_GameObject()
{
	_matrix I;
	m_pGraphicDev->SetTransform(D3DTS_WORLD, D3DXMatrixIdentity(&I));
	SetPreRenderState();
	CParticleEmitter::Render_GameObject();
	SetPostRenderState();
}

void CBossTrail::SetPreRenderState()
{
	m_pGraphicDev->SetTexture(1, m_pTextureDesc->pTexture);

	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	// Alpha = 텍스처 알파 * 버텍스 알파
	// 위의 배경블랜딩을 INVSRCALPHA로 했으므로 
	//	Diffuse.Alpha 가 0이면 배경그대로 유지됨
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

void CBossTrail::SetPostRenderState()
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

void CBossTrail::Reset()
{
	for (auto iter = m_ActiveList.begin(); iter != m_ActiveList.end(); iter++)
	{
		(*iter)->bIsAlive = false;
	}
	m_ActiveList.clear();
	m_fTime = 0.f;
}

HRESULT CBossTrail::Add_Component()
{
	//Vertex
	m_pBufferCom = CDVIBuffer::Create(m_pGraphicDev, m_iMaxParticle, m_iBatchSize);
	if (m_pBufferCom == nullptr) return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", m_pBufferCom });

	m_pTextureCom = static_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Effect_BossTrail_Texture"));
	if (m_pTextureCom == nullptr) return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", m_pTextureCom });

	return S_OK;
}

void CBossTrail::ResetParticle(Particle* particle)
{
	particle->bIsAlive = true;
	_int idx = (m_iIdx++) % 3;
	particle->color = m_Colors[idx];
	particle->fAnimSpeed = 0.f;
	particle->fAnimTime = 0.f;
	particle->vStartUV = { 0,0 };
	particle->vEndUV = { 1.f,1.f };
	particle->vSize = m_vSize;
	particle->fAge = 0.f;
	particle->fLifeTime = m_fLifeTime;
	particle->bDirection = false;
	particle->vDirection = { 0,1,0 };
	//Editor 용 
	if (!m_pOwnerTransform)
	{
		particle->vPosition = m_vDebugPos;
		m_vDebugPos += {0.5f, 0, 0};
	}
	else
	{
		_vec3 vLook;
		memcpy(&vLook, m_pOwnerTransform->Get_World()->m[INFO_LOOK], sizeof(_vec3));
		D3DXVec3Normalize(&vLook, &vLook);
		particle->vPosition = *m_pOwnerTransform->Get_Info(INFO_POS) + vLook*2;
	}

}

CBossTrail* CBossTrail::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBossTrail* effect = new CBossTrail(pGraphicDev);

	if (FAILED(effect->Ready_GameObject()))
	{
		Safe_Release(effect);
		MSG_BOX("BossTrail Create Failed");
		return nullptr;
	}

	return effect;
}

void CBossTrail::Free()
{
	CParticleEmitter::Free();
}
