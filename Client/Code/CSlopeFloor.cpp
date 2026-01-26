#include "pch.h"
#include "CSlopeFloor.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "Engine_Enum.h"

vector<TextureSource> CSlopeFloor::m_vTextureSource =
{
	// 위 두개는 인덱스 채우기 용도 
	{ STATIC_FLOOR, L"../Bin/Resource/Texture/Terrain/Floor/STATIC_FLOOR/FLOORS.dds", true, 0, 7, 7, {2.f, 2.f}},
	{ STATIC_FLOOR_FLUID, L"../Bin/Resource/Texture/Terrain/Floor/STATIC_FLOOR/FLOOR_FLUID.dds", true, 0, 2, 2, {0.f, 0.f}},
	{ STATIC_FLOOR_SLOPE, L"../Bin/Resource/Texture/Terrain/Floor/STATIC_FLOOR/FLOOR_SLOPE.dds", false, 0, 0, 0, { 2.f, 2.f}}
};

CSlopeFloor::CSlopeFloor(LPDIRECT3DDEVICE9 pGraphicDev)
	: CFloor(pGraphicDev)
	, m_eSlopeDir(SLOPE_POSITIVE_X)
{
	m_eColliderTag = TAG_SLOPE;
	m_iFloorType = STATIC_FLOOR_SLOPE;
}

CSlopeFloor::CSlopeFloor(const CSlopeFloor& rhs)
	: CFloor(rhs)
	, m_eSlopeDir(rhs.m_eSlopeDir)
{
	m_eColliderTag = rhs.m_eColliderTag;
	m_iFloorType = rhs.m_iFloorType;
}

CSlopeFloor::~CSlopeFloor()
{
}

HRESULT CSlopeFloor::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

_int CSlopeFloor::Update_GameObject(const _float& fTimeDelta)
{
	//if (IsDead())
	//	return RET_DEAD;

	//_int iExit = CTerrain::Update_GameObject(fTimeDelta);

	//CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

	//return iExit;

	_int iExit = CFloor::Update_GameObject(fTimeDelta);

	return iExit;
}


void CSlopeFloor::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CSlopeFloor::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	if (FAILED(Ready_Material(D3DXCOLOR(0.6f, 0.4f, 0.2f, 1.f))))
		return;

	if (!m_bIsAnimated && m_pTextureCom)
		m_pTextureCom->Render_Texture();

	m_pBufferUpCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetTexture(0, nullptr);
}

void CSlopeFloor::Set_FloorType(_uint eFloorType)
{
	m_iFloorType = eFloorType;

	if (m_pTextureCom)
	{
		m_pTextureCom->Change_Texture(eFloorType);
		m_pTextureCom->Set_Frame(_vec2(m_iTextureIdx, 0));
	}

	switch (eFloorType)
	{
	case STATIC_FLOOR_SLOPE:
		Set_ColliderTag(TAG_SLOPE);
		break;
	default:
		Set_ColliderTag(TAG_NONE);
		break;
	}

}

void CSlopeFloor::Set_SlopeDirection(SLOPE_DIR eDir)
{
	m_eSlopeDir = eDir;
}

HRESULT CSlopeFloor::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// Transform
	pComponent = m_pTransformCom = static_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Transform", pComponent });

	// Buffer - RcTexUp
	pComponent = m_pBufferUpCom = static_cast<Engine::CRcTexUp*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTexUp"));
	
	if (nullptr == pComponent)
		return E_FAIL;	

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Texture - 경사면 용 텍스처 
	pComponent = m_pTextureCom = static_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Slope_FloorTexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

CSlopeFloor* CSlopeFloor::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSlopeFloor* pInstance = new CSlopeFloor(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CSlopeFloor Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CSlopeFloor::Free()
{
	CFloor::Free();
}
