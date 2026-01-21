#include "pch.h"
#include "CEditorDynamicFloor.h"
#include "CRenderer.h"
#include "CProtoMgr.h"
#include "CTransform.h"
#include "CRcTex.h"
#include "CScrollTexture.h"
#include "CAnimation.h"

vector<TextureSource> CEditorDynamicFloor::m_vTextureSource =
{
	{ DYNAMIC_FLOOR_WATER, L"../Bin/Resource/Texture/Terrain/Floor/DYNAMIC_FLOOR/WATER_Scroll.dds"},
	{ DYNAMIC_FLOOR_LAVA, L"../Bin/Resource/Texture/Terrain/Floor/DYNAMIC_FLOOR/LAVA_Scroll.dds"},
	{ DYNAMIC_FLOOR_ACID, L"../Bin/Resource/Texture/Terrain/Floor/DYNAMIC_FLOOR/ACID_Scroll.dds"},
};

CEditorDynamicFloor::CEditorDynamicFloor(LPDIRECT3DDEVICE9 pGraphicDev)
	: CEditorFloor(pGraphicDev)
	, m_pScrollTextureCom(nullptr)
{
	m_iFloorType = DYNAMIC_FLOOR_WATER;
}

CEditorDynamicFloor::~CEditorDynamicFloor()
{
}

HRESULT CEditorDynamicFloor::Ready_GameObject()
{
	FAILED_CHECK_RETURN(CEditorObject::Ready_GameObject(), E_FAIL);

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	// 기본 이름
	m_wstrName = L"DynamicFloor (WATER)";

	return S_OK;
}

_int CEditorDynamicFloor::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CEditorObject::Update_GameObject(fTimeDelta);

	if (m_pScrollTextureCom)
			m_pScrollTextureCom->Update_Texture(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA_WRAP, this);

	return iExit;
}

void CEditorDynamicFloor::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CEditorObject::LateUpdate_GameObject(fTimeDelta);
	m_pScrollTextureCom->Late_Update_Texture();
}

void CEditorDynamicFloor::Render_GameObject()
{
	// 텍스처 스테이트 저장 
	DWORD dwOldColorOp, dwOldColorArg1, dwOldColorArg2, dwOldTextureFactor;

	m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwOldColorOp);
	m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG1, &dwOldColorArg1);
	m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG2, &dwOldColorArg2);
	m_pGraphicDev->GetRenderState(D3DRS_TEXTUREFACTOR, &dwOldTextureFactor);

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pScrollTextureCom->Render_Texture();

	if (m_bSelected)
	{
		m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
		m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 255, 100));
	}

	m_pBufferCom->Render_Buffer();

	// 텍스처 스테이트 복원
	m_pGraphicDev->SetTexture(0, nullptr);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, dwOldColorOp);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, dwOldColorArg1);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, dwOldColorArg2);
	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwOldTextureFactor);

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
}

HRESULT CEditorDynamicFloor::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// Transform 
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>(
		Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	// Buffer 
	pComponent = m_pBufferCom = dynamic_cast<Engine::CVIBuffer*>(
		Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Texture 
	pComponent = m_pScrollTextureCom = dynamic_cast<Engine::CScrollTexture*>(
		Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Dynamic_FloorTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].insert({ L"Com_Texture", pComponent });

	if (m_pScrollTextureCom)
	{
		m_pScrollTextureCom->Change_Texture(m_iFloorType);
	}

	return S_OK;
}

void CEditorDynamicFloor::Set_FloorType(_uint iType)
{
	m_iFloorType = iType;

	if (m_pScrollTextureCom)
	{
		m_pScrollTextureCom->Change_Texture(iType);
	}

	// 이름 업데이트
	switch (iType)
	{
	case DYNAMIC_FLOOR_LAVA:
		m_wstrName = L"DynamicFloor (LAVA)";
		break;
	case DYNAMIC_FLOOR_WATER:
		m_wstrName = L"DynamicFloor (WATER)";
		break;
	case DYNAMIC_FLOOR_ACID:
		m_wstrName = L"DynamicFloor (ACID)";
		break;
	default:
		m_wstrName = L"DynamicFloor";
		break;
	}
}

CEditorDynamicFloor* CEditorDynamicFloor::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
	CEditorDynamicFloor* pInstance = new CEditorDynamicFloor(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CEditorDynamicFloor Create Failed");
		return nullptr;
	}

	// 위치 설정
	pInstance->Set_Position(vPos);

	// 기본 크기
	pInstance->Set_Scale(_vec3(8.f, 8.f, 1.f));

	// 기본 회전: XZ 평면 (바닥)
	pInstance->Set_Rotation(_vec3(90.f, 0.f, 0.f));

	return pInstance;
}

CEditorDynamicFloor* CEditorDynamicFloor::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot, _vec3 vScale)
{
	CEditorDynamicFloor* pInstance = new CEditorDynamicFloor(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CEditorDynamicFloor Create Failed");
		return nullptr;
	}

	pInstance->Set_Scale(vScale);
	pInstance->Set_Rotation(vRot);
	pInstance->Set_Position(vPos);

	return pInstance;
}

CEditorDynamicFloor* CEditorDynamicFloor::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot, _vec3 vScale, _uint iType)
{
	CEditorDynamicFloor* pInstance = new CEditorDynamicFloor(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CEditorDynamicFloor Create Failed");
		return nullptr;
	}

	pInstance->Set_Scale(vScale);
	pInstance->Set_Rotation(vRot);
	pInstance->Set_Position(vPos);
	pInstance->Set_FloorType(iType);

	return pInstance;
}



void CEditorDynamicFloor::Free()
{
	CEditorFloor::Free();
}