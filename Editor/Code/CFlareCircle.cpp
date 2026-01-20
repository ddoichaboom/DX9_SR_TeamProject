#include "pch.h"
#include "CRenderer.h"
#include "CProtoMgr.h"
#include "CFlareCircle.h"

TextureSource CFlareCircle::m_TextureSource =
{
	0, L"../Bin/Resource/Texture/Effect/BeamFlare.dds", false
};

CFlareCircle::CFlareCircle(IDirect3DDevice9* device)
	:CParticleEmitter(device, 1)
{
	m_vPos = { 0,0,0 };
	m_vSize = { 6.f, 6.f };
	m_fAnimSpeed = 0.f;
	m_iMaxParticle = 1;
	m_iBatchSize = 1;
	m_fLifeTime = FLT_MAX;

	m_bLoop = true;
	m_color = { 1,1,1,1 };
}

CFlareCircle::~CFlareCircle()
{
}

HRESULT CFlareCircle::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	m_pBufferCom->SetBatchSize(m_iBatchSize);
	m_pBufferCom->SetParticleCount(m_iMaxParticle);
	ChangeState(0);

	AddParticle();
	return S_OK;
}

void CFlareCircle::Render_GameObject()
{
}

void CFlareCircle::SetPreRenderState()
{
}

void CFlareCircle::SetPostRenderState()
{
}

HRESULT CFlareCircle::Add_Component()
{
	return E_NOTIMPL;
}

void CFlareCircle::ResetParticle(Particle* particle)
{
}

void CFlareCircle::Free()
{
}
