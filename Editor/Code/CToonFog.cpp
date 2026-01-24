#include "pch.h"
#include "CToonFog.h"
#include "CRenderer.h"
#include "CProtoMgr.h"

TextureSource CToonFog::m_TextureSource =
{
	0, L"../Bin/Resource/Texture/Effect/ToonFog.dds", true,7,1,1
};

CToonFog::CToonFog(IDirect3DDevice9* device)
	:CParticleEmitter(device, 20),m_pOwnerTransform(nullptr)
{
	m_vPos = { 0,0,0 };
	m_vSize = { 6.f, 3.f };
	m_fAnimSpeed = 5.f;
	m_iMaxParticle = 20;
	m_iBatchSize = 20;
	m_fLifeTime = 150.f;
	m_bLoop = false;
	m_color = { 0,0,0,1 };
	m_fFogCreateTime = 20.f;
}

CToonFog::~CToonFog()
{
}

HRESULT CToonFog::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	m_pBufferCom->SetBatchSize(m_iBatchSize);
	m_pBufferCom->SetParticleCount(m_iMaxParticle);
	ChangeState(0);
	return S_OK;
}

_int CToonFog::Update_GameObject(const _float& fTimeDelta)
{
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

	m_fTime += fTimeDelta;
	if (m_fTime >= m_fFogCreateTime)
	{
		m_fTime = 0.f;
		AddParticle();
	}

	for (auto iter = m_ActiveList.begin(); iter != m_ActiveList.end(); iter++)
	{
		if ((*iter)->bIsAlive)
		{
			Particle* particle = *iter;
			particle->fAge += fTimeDelta;
			particle->fAnimTime += fTimeDelta;
			if (particle->fAnimTime >= particle->fAnimSpeed)
			{
				particle->fAnimTime = 0.f;
				SetNextUV(particle);
			}
			//if (particle->fAge >= particle->fLifeTime)
			//{
			//	particle->bIsAlive = false;
			//}
		}
	}
	return RET_NONE;
}

void CToonFog::Render_GameObject()
{
	_matrix I;
	m_pGraphicDev->SetTransform(D3DTS_WORLD, D3DXMatrixIdentity(&I));
	CParticleEmitter::Render_GameObject();
}

void CToonFog::SetPreRenderState()
{
}

void CToonFog::SetPostRenderState()
{
}

HRESULT CToonFog::Add_Component()
{
	//Vertex
	m_pBufferCom = CDVIBuffer::Create(m_pGraphicDev, m_iMaxParticle, m_iBatchSize);
	if (m_pBufferCom == nullptr) return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", m_pBufferCom });

	m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Effect_ToonFog_Texture"));
	if (m_pTextureCom == nullptr) return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", m_pTextureCom });

	return S_OK;
}


void CToonFog::ResetParticle(Particle* particle)
{
	particle->bIsAlive = true;
	particle->color = { 1,1,1,1 };
	particle->fAnimSpeed = m_fAnimSpeed;
	particle->fAnimTime = 0.f;
	particle->vStartUV = { 0,0 };
	particle->vEndUV = { m_pTextureDesc->vUVoffset.x ,0};
	particle->vSize = m_vSize;
	particle->fAge = 0.f;
	particle->fLifeTime = m_fLifeTime;
	particle->bDirection = true;

	//Editor 용 
	if (!m_pOwnerTransform)
	{
		particle->vDirection = { 0,1,0 };
		particle->vPosition = m_vDebugPos;
		m_vDebugPos += {0.3f, 0, 0};
	}
	else
	{
		_vec3 pos{}, dir{};
		//빌보드를 사용하는 오브젝트는 info 말고 Matrix에서 직접 꺼내오기 
		_matrix* mat = m_pOwnerTransform->Get_World();
		memcpy(&pos, &mat->m[INFO_POS], sizeof(_vec3));
		memcpy(&dir, &mat->m[INFO_UP], sizeof(_vec3));

		particle->vDirection = dir;
		particle->vPosition = pos;
	}

}

CToonFog* CToonFog::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CToonFog* effect = new CToonFog(pGraphicDev);

	if (FAILED(effect->Ready_GameObject()))
	{
		Safe_Release(effect);
		MSG_BOX("ToonFlash Create Failed");
		return nullptr;
	}

	return effect;
}
void CToonFog::Free()
{
	CParticleEmitter::Free();
}


