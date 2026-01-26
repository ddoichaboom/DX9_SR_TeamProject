#include "pch.h"
#include "CHitUI.h"
#include "CRenderer.h"
#include "CProtoMgr.h"

TextureSource CHitUI::m_TextureSource =
{ 0, L"../Bin/Resource/Texture/Effect/HitUI_1024.dds", true,3,1,1};

CHitUI::CHitUI(IDirect3DDevice9* device)
	:CParticleEmitter(device, 1), m_vCurFrame{ 0,0 }
{
	m_vPos = { WINCX*0.5f,WINCY*0.5f,0 };
	m_vSize = {WINCX, WINCY};
	m_fAnimSpeed = 2.f;
	m_iMaxParticle = 1;
	m_iBatchSize = 1;

	m_bLoop = false;
}

CHitUI::~CHitUI()
{
}

HRESULT CHitUI::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	m_pBufferCom->SetBatchSize(m_iBatchSize);
	m_pBufferCom->SetParticleCount(m_iMaxParticle);
	ChangeState(0);

	AddParticle();

	return S_OK;
}

_int CHitUI::Update_GameObject(const _float& fTimeDelta)
{
	//if (IsDead()) return RET_DEAD;

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
	for (auto iter = m_ActiveList.begin(); iter != m_ActiveList.end(); iter++)
	{
		if ((*iter)->bIsAlive)
		{
			(*iter)->fAnimTime += fTimeDelta;
			if ((*iter)->fAnimSpeed <= (*iter)->fAnimTime)
			{
				(*iter)->fAnimTime = 0.f;
				bool IsEnd = SetNextFrame(*iter, m_vCurFrame);
				if (IsEnd) (*iter)->bIsAlive = false;
			}
		}
	}

	return RET_NONE;
}

void CHitUI::Render_GameObject()
{
	_matrix I;
	D3DXMatrixIdentity(&I);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &I);
	SetPreRenderState();
	CParticleEmitter::Render_GameObject();
	SetPostRenderState();
}

void CHitUI::SetPreRenderState()
{
}

void CHitUI::SetPostRenderState()
{
}

void CHitUI::Deactivate()
{
	CParticleEmitter::Deactivate();
	m_vCurFrame = { 0,0 };
}

HRESULT CHitUI::Add_Component()
{
	//Vertex
	m_pBufferCom = CDVIBuffer::Create(m_pGraphicDev, m_iMaxParticle, m_iBatchSize);
	if (m_pBufferCom == nullptr) return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", m_pBufferCom });

	m_pTextureCom = static_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Effect_HitUI_Texture"));
	if (m_pTextureCom == nullptr) return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", m_pTextureCom });

	return S_OK;
}

void CHitUI::ResetParticle(Particle* particle)
{
	particle->bIsAlive = true;

	particle->vPosition = {m_vPos.x - WINCX*0.5f, -m_vPos.y + WINCY * 0.5f ,0.f};
	particle->color = m_color;
	particle->fAnimSpeed = m_fAnimSpeed;
	particle->fAnimTime = 0.f;
	particle->vStartUV = { 0,0 };
	particle->vEndUV = m_pTextureDesc->vUVoffset;
	particle->vSize = m_vSize;
	particle->fAge = 0.f;
	particle->fLifeTime = 0.f;
}

CHitUI* CHitUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CHitUI* effect = new CHitUI(pGraphicDev);

	if (FAILED(effect->Ready_GameObject()))
	{
		Safe_Release(effect);
		MSG_BOX("HitUI Create Failed");
		return nullptr;
	}

	return effect;
}

void CHitUI::Free()
{
	CParticleEmitter::Free();
}
