#include "CParticleEmitter.h"
#include "CProtoMgr.h"
#include "CObjectPool.h"

CParticleEmitter::CParticleEmitter(IDirect3DDevice9* device, _int _maxParticle)
	:CGameObject(device), m_vOrigin{}, m_vSize(1.f, 1.f), m_iMaxParticle(_maxParticle)
	, m_pBufferCom(nullptr), m_pTextureCom(nullptr), m_iState(-1), m_pTextureDesc(nullptr)
{
	m_eOBJ_ID = OBJ_EFFECT;
	m_Particles.resize(m_iMaxParticle);
}

CParticleEmitter::~CParticleEmitter()
{
}


HRESULT	CParticleEmitter::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	return S_OK;
}

_int CParticleEmitter::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);
	return iExit;
}

void CParticleEmitter::Render_GameObject()
{
	if (m_pTextureCom) m_pTextureCom->Render_Texture();
	if (m_pBufferCom) m_pBufferCom->Render_Buffer(m_ActiveList);
}

void CParticleEmitter::Activate()
{
	CGameObject::Activate();
}
void CParticleEmitter::Reset()
{
	for (auto iter = m_Particles.begin(); iter != m_Particles.end(); iter++)
	{
		ResetParticle(&(*iter));
	}
}

void CParticleEmitter::AddParticle()
{
	for (auto& particle : m_Particles)
	{
		if (particle.bIsAlive == false)
		{
			ResetParticle(&particle);
			m_ActiveList.push_back(&particle);
			return;
		}
	}
}

void CParticleEmitter::SetPreRenderState()
{
	//m_pGraphicDev->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	//m_pGraphicDev->SetRenderState(D3DRS_POINTSCALEENABLE, true);

	//m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_A, FtoDW(0.f));
	//m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_B, FtoDW(0.f));
	//m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_C, FtoDW(1.f));
}

void CParticleEmitter::SetPostRenderState()
{
	//m_pGraphicDev->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	//m_pGraphicDev->SetRenderState(D3DRS_POINTSCALEENABLE, false);
}

void CParticleEmitter::RemoveDeadParticles()
{
	for (auto iter = m_ActiveList.begin(); iter != m_ActiveList.end(); )
	{
		if ((*iter)->bIsAlive == false)
		{
			iter = m_ActiveList.erase(iter);
		}
		else iter++;
	}
}

bool CParticleEmitter::IsEmpty()
{
	return m_Particles.empty();
}

bool CParticleEmitter::IsDead()
{
	if (m_bDead) return m_bDead;
	for (auto iter = m_Particles.begin(); iter != m_Particles.end(); iter++)
	{
		if (iter->bIsAlive) return false;
	}

	m_bDead = true;
	return m_bDead;
}


void CParticleEmitter::Free()
{
	CGameObject::Free();
	m_Particles.clear();
	m_ActiveList.clear();
}

void CParticleEmitter::SetNextUV(Particle* pParticle)
{
	if (!pParticle || !m_pTextureDesc) return;
	_vec2 curUV = pParticle->vStartUV;
	_vec2 uvOffset = m_pTextureDesc->vUVoffset;
	curUV.x += uvOffset.x;
	if (curUV.x >= 1.f)
	{
		curUV.x = 0.f;
		curUV.y += uvOffset.y;
		if (curUV.y >= 1.f)
		{
			curUV.y = 0.f;
			if (m_bLoop == false) pParticle->bIsAlive = false;
		}
	}
	pParticle->vStartUV = curUV;
	pParticle->vEndUV = curUV + uvOffset;

}

void CParticleEmitter::ChangeState(_uint _state)
{
	if (!m_pTextureCom || m_iState == _state) return;
	m_pTextureCom->Change_Texture(_state);
	m_pTextureDesc = m_pTextureCom->GetTextureDesc(_state);
	m_iState = _state;
}