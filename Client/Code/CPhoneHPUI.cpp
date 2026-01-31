#include "pch.h"
#include "CPhoneHPUI.h"
#include "CRenderer.h"
#include "CProtoMgr.h"


CPhoneHPUI::CPhoneHPUI(IDirect3DDevice9* device, CGameObject* _pOwner)
	:CParticleEmitter(device, 1), m_pOwner(_pOwner)
{
	m_color = m_HPColors[0];
	m_vPos = { 0,0,0 };
	m_vSize = { 100,100 };
	m_fAnimSpeed = 1.f;
	m_iMaxParticle = 1;
	m_iBatchSize = 1;
	m_fLifeTime = 1000;
	m_bLoop = false;
	if (m_pOwner)
	{
		m_fMaxHP = m_pOwner->GetMaxHP();
	}
}

CPhoneHPUI::~CPhoneHPUI()
{
}

HRESULT CPhoneHPUI::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	m_pBufferCom->SetBatchSize(m_iBatchSize);
	m_pBufferCom->SetParticleCount(m_iMaxParticle);
	AddParticle();

	return S_OK;
}

_int CPhoneHPUI::Update_GameObject(const _float& fTimeDelta)
{
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);

	for (auto iter = m_ActiveList.begin(); iter != m_ActiveList.end(); iter++)
	{
		Particle* particle = (*iter);
		if (particle->bIsAlive)
		{
			if (m_pOwner && !m_bHPEmpty)
			{

				m_fOwnerHP = m_pOwner->GetHP();
				m_fRatio = (m_fOwnerHP / m_fMaxHP);
				m_type = GetHPType(m_fRatio);

				if (m_type == HP_DANAGER)
				{
					m_fTime += fTimeDelta;
					if (m_fTime >= m_bClickTime)
					{
						if (m_bClicker) particle->color = m_HPColors[HP_DANAGER];
						else particle->color = { 1,1,1,1 };
						m_bClicker = !m_bClicker;
						m_fTime = 0.f;
					}
					particle->vSize.y = m_vSize.y;
				}
				else
				{
					if (m_type == HP_DEAD)
					{
						particle->vSize.y = m_vSize.y;
						//체력 0 찍은 후(DEAD) 회복 불가능하게 할거면 주석 제거 
						//m_bHPEmpty = true;
					}
					else particle->vSize.y = m_vSize.y * m_fRatio;
					particle->color = m_HPColors[m_type];
				}

				particle->vPosition.y = m_vPos.y + particle->vSize.y*0.5f;
			}

		}
		else m_bDead = true;
	}

	return RET_NONE;
}

void CPhoneHPUI::Render_GameObject()
{
	_matrix I;
	m_pGraphicDev->SetTransform(D3DTS_WORLD, D3DXMatrixIdentity(&I));
	SetPreRenderState();
	CParticleEmitter::Render_GameObject();
	SetPostRenderState();

}

void CPhoneHPUI::SetPreRenderState()
{
	m_pGraphicDev->SetTexture(0, nullptr);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
}

void CPhoneHPUI::SetPostRenderState()
{
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
}

void CPhoneHPUI::Reset()
{
	m_bDead = false;
	m_bHPEmpty = false;
	CParticleEmitter::Reset();
}

HRESULT CPhoneHPUI::Add_Component()
{
	//Vertex
	m_pBufferCom = CDVIBuffer::Create(m_pGraphicDev, m_iMaxParticle, m_iBatchSize);
	if (m_pBufferCom == nullptr) return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", m_pBufferCom });

	return S_OK;
}

void CPhoneHPUI::ResetParticle(Particle* particle)
{
	particle->vPosition = m_vPos;
	particle->bIsAlive = true;
	particle->color = m_color;
	particle->fAnimSpeed = m_fAnimSpeed;
	particle->fAnimTime = 0.f;

	particle->vSize = m_vSize;
	particle->fAge = 0.f;
	particle->fLifeTime = m_fLifeTime;
	particle->vStartUV = { 0,0 };
	particle->vEndUV = { 1,1 };
	particle->vPosition.y += m_vSize.y*0.5f;


}

CPhoneHPUI* CPhoneHPUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* _owner)
{
	CPhoneHPUI* effect = new CPhoneHPUI(pGraphicDev, _owner);

	if (FAILED(effect->Ready_GameObject()))
	{
		Safe_Release(effect);
		MSG_BOX("Phone HP UI Create Failed");
		return nullptr;
	}

	return effect;
}

void CPhoneHPUI::SetOwner(CGameObject* _owner)
{
	if (!_owner) return;
	m_pOwner = _owner;
	m_fMaxHP = m_pOwner->GetMaxHP();
}

void CPhoneHPUI::Free()
{
	CParticleEmitter::Free();
}

HP_TYPE CPhoneHPUI::GetHPType(_float _hp)
{
	for (int i = 0; i < HP_END; i++)
	{
		if (_hp > m_HPMinRatio[i]) return (HP_TYPE)i;
	}
	return HP_END;
}
