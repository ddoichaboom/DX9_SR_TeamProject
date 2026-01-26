#include "pch.h"
#include "CDynamicCeiling.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "Engine_Enum.h"

vector<TextureSource> CDynamicCeiling::m_vTextureSource =
{
		{DYNAMIC_WALL_FAN, L"../Bin/Resource/Texture/Terrain/WALL/DYNAMIC_WALL/FAN.dds"}
};

vector<AnimationSource> CDynamicCeiling::m_vAnimSource =
{
		{ DYNAMIC_WALL_FAN, 0, 2, 2, true, 0.5f}

};

CDynamicCeiling::CDynamicCeiling(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCeiling(pGraphicDev)
	, m_pAnimationCom(nullptr)
{
	m_bIsAnimated = true;
}

CDynamicCeiling::CDynamicCeiling(const CDynamicCeiling& rhs)
	: CCeiling(rhs)
	, m_pAnimationCom(nullptr)
{
	m_bIsAnimated = true;
}

CDynamicCeiling::~CDynamicCeiling()
{
}

HRESULT CDynamicCeiling::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

_int CDynamicCeiling::Update_GameObject(const _float& fTimeDelta)
{
	if (IsDead()) 
		return RET_DEAD;

	_int iExit = CTerrain::Update_GameObject(fTimeDelta);

	if (m_pAnimationCom)
		m_pAnimationCom->Update_Component(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

	return iExit;
}

void CDynamicCeiling::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CDynamicCeiling::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	if (FAILED(Ready_Material(D3DXCOLOR(0.3f, 0.5f, 0.7f, 1.f))))
		return;

	if (m_bIsAnimated && m_pAnimationCom)
		m_pAnimationCom->Render_Animation();		// 애니메이션 렌더링

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetTexture(0, nullptr);
}

void CDynamicCeiling::Set_CeilingType(_uint eCeilingType)
{
	if (m_pAnimationCom)
		m_pAnimationCom->Change_Animation(eCeilingType);
}

HRESULT CDynamicCeiling::Add_Component()
{
	if (FAILED(CTerrain::Add_Component()))
		return E_FAIL;

	Engine::CComponent* pComponent = nullptr;

	// Texture
	pComponent = m_pTextureCom = static_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Dynamic_CeilingTexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture", pComponent });

	// Animation
	pComponent = m_pAnimationCom = static_cast<Engine::CAnimation*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CeilingAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });

	return S_OK;
}

CDynamicCeiling* CDynamicCeiling::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CDynamicCeiling* pInstance = new CDynamicCeiling(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CDynamicCeiling Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CDynamicCeiling::Free()
{
	CCeiling::Free();
}