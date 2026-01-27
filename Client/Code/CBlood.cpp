#include "pch.h"
#include "CParticleEmitter.h"
#include "CRenderer.h"
#include "CProtoMgr.h"
#include "CBlood.h"

//3번은 폐기
vector<TextureSource> CBlood::m_TextureSources
{
	{ BLOOD1, L"../Bin/Resource/Texture/Effect/Blood1.dds", true,3,3,3, {0.f, 0.f} }
	,{ BLOOD2, L"../Bin/Resource/Texture/Effect/Blood2.dds", true,7,7,7, {0.f, 0.f} }
	,{ BLOOD3, L"../Bin/Resource/Texture/Effect/Blood3.dds", true,7,3,3, {0.f, 0.f} }
};

Particle CBlood::m_ParticleInfo[BLOOD_END] =
{
	{_vec3{},_vec3{},{	20.f,20.f }, 10.f, 0.035f, 0.f, 0.f, { 0.7f,0.043f,0.043f,1.f },{0.f, 0.f},{0.f, 0.f},false}, // 1.f
	{_vec3{},_vec3{},{	25.f,25.f }, 10.f, 0.01f,0.f, 0.f, { 0.7f,0.043f,0.043f,1.f },{0.f, 0.f},{0.f, 0.f},false},
	{_vec3{},_vec3{},{	30.f,20.f },  10.f, 0.01f,0.f, 0.f,{ 0.7f,0.043f,0.043f,1.f },{0.f, 0.f},{0.f, 0.f},false}, // 0.5f
};

CBlood::CBlood(IDirect3DDevice9* devices)
	:CParticleEmitter(devices, 1)
{
	m_vOrigin = { 0,0,0 };
	m_vPos = m_vOrigin;
	m_iBatchSize = 1;
	m_bLoop = false;
	m_vSize = { 1.f,1.f };
}

CBlood::~CBlood()
{
}

HRESULT CBlood::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	m_pBufferCom->SetBatchSize(m_iBatchSize);
	m_pBufferCom->SetParticleCount(m_iMaxParticle);
	ChangeState(BLOOD1);

	for (int i = 0; i < m_iMaxParticle; i++) AddParticle();
	return S_OK;
}

_int CBlood::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bDead) return RET_DEAD;

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	bool bActive = false;
	for (auto iter = m_ActiveList.begin(); iter != m_ActiveList.end(); iter++)
	{
		if ((*iter)->bIsAlive)
		{
			if(!bActive) bActive = true;

			(*iter)->fAge += fTimeDelta;
			(*iter)->fAnimTime += fTimeDelta;
			if ((*iter)->fAnimSpeed <= (*iter)->fAnimTime)
			{
				SetNextUV((*iter));
				(*iter)->fAnimTime = 0.f;
			}
			if ((*iter)->fAge >= (*iter)->fLifeTime)
			{
				(*iter)->bIsAlive = false;
			}
		}
	}

	if (!bActive)
	{
		m_bDead = true;
		return RET_DEAD;
	}

	Compute_ViewZ(&m_vPos);
	return RET_NONE;
}


void CBlood::Render_GameObject()
{
	_matrix I;
	D3DXMatrixIdentity(&I);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &I);
	SetPreRenderState();
	CParticleEmitter::Render_GameObject();
	SetPostRenderState();
}

void CBlood::SetPreRenderState()
{
	//Blood 이미지는 알파가 1이고 투명한 부분이 검정색, 나머지가 흰색임 
	//알파블랜딩이 안됨. 때문에 강제적으로RGB색상값을 알파 채널로 복사하는 방법 

	//0,0,0 검정이랑 내적하면 0, 흰색은 3이지만 알아서 클램핑해줌,내 색RGB와 내적하면 RGB 
	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFFFFFFFF);

	//D3DTOP_DOTPRODUCT3 는 내적. 결과값을 모든 채널에 복사함. 즉, 결과값이 알파 채널에더 적용됨 
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DOTPRODUCT3);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE); // 텍스처
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR); // 흰색 0xFFFFFFFF

	//검정색 픽셀은 0,0,0 이므로 색이 들어가지않음
	m_pGraphicDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_DIFFUSE); // 파티클 색
	m_pGraphicDev->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT); // 스테이지 0의 결과


	m_pGraphicDev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT); // 스테이지 0의 Alpha

}

void CBlood::SetPostRenderState()
{
	//텍스쳐 색으로 나타넴
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	m_pGraphicDev->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pGraphicDev->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

}


void CBlood::Deactivate()
{
	CParticleEmitter::Deactivate();
	m_vSize = { 1.f, 1.f };
}

HRESULT CBlood::Add_Component()
{
	//Vertex
	m_pBufferCom = CDVIBuffer::Create(m_pGraphicDev, m_iMaxParticle, m_iBatchSize);
	if (m_pBufferCom == nullptr) return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", m_pBufferCom });

	m_pTextureCom = static_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Effect_Blood_Texture"));
	if (m_pTextureCom == nullptr) return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", m_pTextureCom });

	return S_OK;
}

void CBlood::ResetParticle(Particle* particle)
{
	static _vec3 vMin = { -2,-1,0 };
	static _vec3 vMax = { 2,1,0 };
	_float m_fRand = 1.f;
	_vec3 posOffset = { 0,0,0 };
	if (m_iState < 0) return;

	particle->bIsAlive = true;
	if (m_iState != BLOOD2)
	{
		m_fRand = GetRandomFloat(0.7f, 1.3f);
		GetRandomVector(&posOffset, &vMin, &vMax);
	}
	particle->vPosition = m_vPos + posOffset;
	particle->color = m_ParticleInfo[m_iState].color;
	particle->fAnimTime = 0.f;
	particle->fAnimSpeed = m_ParticleInfo[m_iState].fAnimSpeed;
	particle->vStartUV = { 0,0 };
	if (m_pTextureDesc) particle->vEndUV = m_pTextureDesc->vUVoffset;
	else particle->vEndUV = { 1.f,1.f };
	particle->vSize = { m_ParticleInfo[m_iState].vSize.x * m_vSize.x ,m_ParticleInfo[m_iState].vSize.y * m_vSize.y };
	particle->vSize.x *= m_fRand;
	particle->fAge = 0.f;
	particle->fLifeTime = m_ParticleInfo[m_iState].fLifeTime;

}

CBlood* CBlood::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBlood* effect = new CBlood(pGraphicDev);

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
