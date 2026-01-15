#include "pch.h"
#include "CDynamicFloor.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "Engine_Enum.h"


vector<TextureSource> CDynamicFloor::m_vTextureSource =
{
	{ DYNAMIC_FLOOR_WATER, L"../Bin/Resource/Texture/Terrain/Floor/DYNAMIC_FLOOR/WATER.dds"},
	{ DYNAMIC_FLOOR_LAVA, L"../Bin/Resource/Texture/Terrain/Floor/DYNAMIC_FLOOR/LAVA.dds"},
	{ DYNAMIC_FLOOR_ACID, L"../Bin/Resource/Texture/Terrain/Floor/DYNAMIC_FLOOR/ACID.dds"},
};

vector<AnimationSource> CDynamicFloor::m_vAnimSource =
{
	{ DYNAMIC_FLOOR_WATER, 0, 4, 4, true, 0.75f},
	{ DYNAMIC_FLOOR_LAVA, 0, 4, 4, true, 0.75f},
	{ DYNAMIC_FLOOR_ACID, 0, 4, 4, true, 0.75f}
};


CDynamicFloor::CDynamicFloor(LPDIRECT3DDEVICE9 pGraphicDev)
	: CFloor(pGraphicDev)
	, m_pAnimationCom(nullptr)
{
	m_bIsAnimated = true;
}

CDynamicFloor::CDynamicFloor(const CDynamicFloor& rhs)
	: CFloor(rhs)
	, m_pAnimationCom(nullptr)
{
	m_bIsAnimated = true;
}

CDynamicFloor::~CDynamicFloor()
{
}

HRESULT CDynamicFloor::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

_int CDynamicFloor::Update_GameObject(const _float& fTimeDelta)
{
	if (IsDead()) 
		return RET_DEAD;

	_int iExit = CTerrain::Update_GameObject(fTimeDelta);

	if (m_pAnimationCom)
		m_pAnimationCom->Update_Component(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

	return iExit;
}

void CDynamicFloor::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CDynamicFloor::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	if (FAILED(Ready_Material(D3DXCOLOR(0.6f, 0.4f, 0.2f, 1.f))))
		return;

	if (m_bIsAnimated && m_pAnimationCom)
		m_pAnimationCom->Render_Animation();		// 애니메이션 렌더링

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetTexture(0, nullptr);
}

HRESULT CDynamicFloor::Add_Component()
{
	if (FAILED(CTerrain::Add_Component()))
		return E_FAIL;

	Engine::CComponent* pComponent = nullptr;

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Dynamic_FloorTexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture", pComponent });

	// Animation
	pComponent = m_pAnimationCom = dynamic_cast<Engine::CAnimation*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_FloorAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });

	return S_OK;
}

void        CDynamicFloor::Set_FloorType(_uint eFloorType)
{
	if (m_pAnimationCom)
		m_pAnimationCom->Change_Animation(eFloorType);

	switch (eFloorType)
	{
	case DYNAMIC_FLOOR_WATER:
		Set_ColliderTag(TAG_WATER);
		break;
	case DYNAMIC_FLOOR_LAVA:
		Set_ColliderTag(TAG_LAVA);
		break;
	case DYNAMIC_FLOOR_ACID:
		Set_ColliderTag(TAG_ACID);
		break;
	default:
		Set_ColliderTag(TAG_NONE);
		break;
	}
}

CDynamicFloor* CDynamicFloor::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CDynamicFloor* pInstance = new CDynamicFloor(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CDynamicFloor Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CDynamicFloor::Free()
{
	CFloor::Free();
}
