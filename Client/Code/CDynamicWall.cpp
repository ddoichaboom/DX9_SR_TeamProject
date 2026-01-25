#include "pch.h"
#include "CDynamicWall.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "Engine_Enum.h"
#include "CAnimation.h"

vector<TextureSource> CDynamicWall::m_vTextureSource =
{
	{DYNAMIC_WALL_FAN, L"../Bin/Resource/Texture/Terrain/WALL/DYNAMIC_WALL/FAN.dds"}
	//{DYNAMIC_WALL_FAN_BLOOD, L"../Bin/Resource/Texture/Terrain/WALL/DYNAMIC_WALL/FAN_BLOOD.dds"}
};

vector<AnimationSource> CDynamicWall::m_vAnimSource =
{
	{ DYNAMIC_WALL_FAN, 0, 2, 2, true, 0.03f}		
};

CDynamicWall::CDynamicWall(LPDIRECT3DDEVICE9 pGraphicDev)
	: CWall(pGraphicDev)
	, m_pAnimationCom(nullptr)
{
	m_bIsAnimated = true;
}

CDynamicWall::CDynamicWall(const CDynamicWall& rhs)
	: CWall(rhs)
	, m_pAnimationCom(nullptr)
{
	m_bIsAnimated = true;
}

CDynamicWall::~CDynamicWall()
{
}

HRESULT CDynamicWall::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

_int CDynamicWall::Update_GameObject(const _float& fTimeDelta)
{
	if (IsDead()) 
		return RET_DEAD;

	_int iExit = CTerrain::Update_GameObject(fTimeDelta);

	if (m_pAnimationCom)
		m_pAnimationCom->Update_Component(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

	return iExit;
}

void CDynamicWall::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CDynamicWall::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	if (FAILED(Ready_Material(D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f))))
		return;

	if (m_bIsAnimated && m_pAnimationCom)
		m_pAnimationCom->Render_Animation();		// 애니메이션 렌더링

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetTexture(0, nullptr);
}

HRESULT CDynamicWall::Add_Component()
{
	if (FAILED(CTerrain::Add_Component()))
		return E_FAIL;

	Engine::CComponent* pComponent = nullptr;

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Dynamic_WallTexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture", pComponent });

	// Animation
	pComponent = m_pAnimationCom = dynamic_cast<Engine::CAnimation*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_WallAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });

	return S_OK;
}

void        CDynamicWall::Set_WallType(_uint eWallType)
{
	m_iWallType = eWallType;

	if (m_pAnimationCom)
		m_pAnimationCom->Change_Animation(eWallType);
}

CDynamicWall* CDynamicWall::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CDynamicWall* pInstance = new CDynamicWall(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CDynamicWall Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CDynamicWall::Free()
{
	CWall::Free();
}