#include "pch.h"
#include "CBossHPUI.h"
#include "CRenderer.h"
#include "CProtoMgr.h"

TextureSource CBossHPUI::m_TextureSource =
{ 0, L"../Bin/Resource/Texture/Effect/BossHPBar.dds", true,3,3,3 };

CBossHPUI::CBossHPUI(IDirect3DDevice9* device, CGameObject* _pOwner)
	:CParticleEmitter(device, 1), m_pOwner(_pOwner)
{
	m_vPos = { 0,WINCY * 0.27f,0 };
	m_vSize = { 310,75 };
	m_fAnimSpeed = 1.f;
	m_iMaxParticle = 1;
	m_iBatchSize = 1;
	m_fLifeTime = 1000;
	m_bLoop = false;
	m_color = { 0.7,0,0,1 };
	if (m_pOwner)
	{
		m_fMaxHP = m_pOwner->GetMaxHP();
		//전체 체력중 한 칸 당 ...값? HP바가 15개 니까 mapHp/15  = 한칸당 HP
		m_DistHp = m_fMaxHP / 15;

	}
	m_bDecrease = false;
}

CBossHPUI::~CBossHPUI()
{
}

HRESULT CBossHPUI::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	m_pBufferCom->SetBatchSize(m_iBatchSize);
	m_pBufferCom->SetParticleCount(m_iMaxParticle);
	ChangeState(0);
	AddParticle();

	return S_OK;
}

_int CBossHPUI::Update_GameObject(const _float& fTimeDelta)
{
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	if (m_bDecrease)
	{
		randMove = { GetRandomFloat(-2.f,2.f),GetRandomFloat(-5.f,5.f),0.f };
		m_fTime += fTimeDelta;
		if (m_fTime >= m_fEffectTime)
		{
			m_bDecrease = false;
			m_fTime = 0.f;
			randMove = {};
		}
	}

	for (auto iter = m_ActiveList.begin(); iter != m_ActiveList.end(); iter++)
	{
		Particle* particle = (*iter);
		if (particle->bIsAlive)
		{
			if (m_pOwner)
			{
				m_fOwnerHP = max(0, m_pOwner->GetHP());
				_int cnt = (int)(m_fOwnerHP / m_DistHp);
				if (cnt != m_CurCnt)
				{
					m_bDecrease = true;
					m_CurCnt = cnt;
				}
				m_vFrame = { (_float)(cnt % 4), (_float)(cnt / 4) };
				particle->vStartUV = { m_vFrame.x * m_pTextureDesc->vUVoffset.x, m_vFrame.y * m_pTextureDesc->vUVoffset.y };
				particle->vEndUV = particle->vStartUV + m_pTextureDesc->vUVoffset;
			}

		}
	}

	return RET_NONE;
}

void CBossHPUI::Render_GameObject()
{
	_matrix I;
	m_pGraphicDev->SetTransform(D3DTS_WORLD, D3DXMatrixIdentity(&I));

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	CParticleEmitter::Render_GameObject();

	if (m_bDecrease)
	{
		m_ActiveList.front()->vPosition += randMove;
		SetPreRenderState();
		CParticleEmitter::Render_GameObject();
		SetPostRenderState();
		m_ActiveList.front()->vPosition -= randMove;
	}

}

void CBossHPUI::SetPreRenderState()
{
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	//Complement (1-Args)
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE | D3DTA_COMPLEMENT);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
}

void CBossHPUI::SetPostRenderState()
{
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTA_TEXTURE);
}

void CBossHPUI::Reset()
{
	m_bDead = false;
	m_bDecrease = false;
	m_vFrame = { 0,0 };
	CParticleEmitter::Reset();
}

HRESULT CBossHPUI::Add_Component()
{
	//Vertex
	m_pBufferCom = CDVIBuffer::Create(m_pGraphicDev, m_iMaxParticle, m_iBatchSize);
	if (m_pBufferCom == nullptr) return E_FAIL;
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", m_pBufferCom });

	m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Effect_BOSSHPUI_Texture"));
	if (m_pTextureCom == nullptr) return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", m_pTextureCom });

	return S_OK;
}

void CBossHPUI::ResetParticle(Particle* particle)
{
	particle->bIsAlive = true;

	particle->vPosition = m_vPos;
	particle->color = m_color;
	particle->fAnimSpeed = m_fAnimSpeed;
	particle->fAnimTime = 0.f;

	particle->vSize = m_vSize;
	particle->fAge = 0.f;
	particle->fLifeTime = m_fLifeTime;
	particle->vStartUV = { 0,0 };
	particle->vEndUV = m_pTextureDesc->vUVoffset;


}

CBossHPUI* CBossHPUI::Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* _owner)
{
	CBossHPUI* effect = new CBossHPUI(pGraphicDev, _owner);

	if (FAILED(effect->Ready_GameObject()))
	{
		Safe_Release(effect);
		MSG_BOX("HitUI Create Failed");
		return nullptr;
	}

	return effect;
}

void CBossHPUI::Free()
{
	CParticleEmitter::Free();
}
