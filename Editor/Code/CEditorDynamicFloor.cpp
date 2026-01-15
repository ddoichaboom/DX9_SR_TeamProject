#include "pch.h"
#include "CEditorDynamicFloor.h"
#include "CRenderer.h"
#include "CProtoMgr.h"
#include "CTransform.h"
#include "CRcTex.h"
#include "CTexture.h"
#include "CAnimation.h"

vector<TextureSource> CEditorDynamicFloor::m_vTextureSource =
{
	{ DYNAMIC_FLOOR_WATER, L"../Bin/Resource/Texture/Terrain/Floor/DYNAMIC_FLOOR/WATER.dds"},
	{ DYNAMIC_FLOOR_LAVA, L"../Bin/Resource/Texture/Terrain/Floor/DYNAMIC_FLOOR/LAVA.dds"},
	{ DYNAMIC_FLOOR_ACID, L"../Bin/Resource/Texture/Terrain/Floor/DYNAMIC_FLOOR/ACID.dds"},
};

vector<AnimationSource> CEditorDynamicFloor::m_vAnimSource =
{
	{ DYNAMIC_FLOOR_WATER, 0, 4, 4, true, 10.0f},
	{ DYNAMIC_FLOOR_LAVA, 0, 4, 4, true, 0.75f},
	{ DYNAMIC_FLOOR_ACID, 0, 4, 4, true, 0.75f}
};

CEditorDynamicFloor::CEditorDynamicFloor(LPDIRECT3DDEVICE9 pGraphicDev)
	: CEditorFloor(pGraphicDev)
	, m_pAnimationCom(nullptr)
	, m_fAnimSpeed(1.0f)
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

	if (m_pAnimationCom)
	{
		// 속도 배율 적용
		_float fScaledDelta = fTimeDelta * m_fAnimSpeed;
		m_pAnimationCom->Update_Component(fScaledDelta);
	}

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

	return iExit;
}

void CEditorDynamicFloor::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CEditorObject::LateUpdate_GameObject(fTimeDelta);
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

	if (m_pAnimationCom)
		m_pAnimationCom->Render_Animation();

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
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(
		Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Dynamic_FloorTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].insert({ L"Com_Texture", pComponent });

	// Animation
	pComponent = m_pAnimationCom = dynamic_cast<Engine::CAnimation*>(
		Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_FloorAnimation"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].insert({ L"Com_Animation", pComponent });

	if (m_pAnimationCom)
	{
		m_pAnimationCom->Change_Animation(m_iFloorType);
		//m_pAnimationCom->Play(); // 원래도 자동 재생
	}

	return S_OK;
}

void CEditorDynamicFloor::Set_FloorType(_uint iType)
{
	m_iFloorType = iType;

	if (m_pAnimationCom)
	{
		m_pAnimationCom->Change_Animation(m_iFloorType);
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




void CEditorDynamicFloor::Play_Animation()
{
	if (m_pAnimationCom)
		m_pAnimationCom->Play();
}

void CEditorDynamicFloor::Pause_Animation()
{
	if (m_pAnimationCom)
		m_pAnimationCom->Pause();
}

void CEditorDynamicFloor::Set_AnimationSpeed(_float fSpeed)
{
	m_fAnimSpeed = fSpeed;
}

bool CEditorDynamicFloor::Is_Playing() const
{
	if (m_pAnimationCom)
		return m_pAnimationCom->IsPlaying();
	return false;
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

CEditorDynamicFloor* CEditorDynamicFloor::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot, _vec3 vScale, _uint iType, _int iRoomIndex)
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
	pInstance->Set_RoomIndex(iRoomIndex);

	return pInstance;
}



void CEditorDynamicFloor::Free()
{
	CEditorFloor::Free();
}