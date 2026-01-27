#include "pch.h"
#include "CEditorDynamicWall.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "Engine_Enum.h"

vector<TextureSource> CEditorDynamicWall::m_vTextureSource =
{
	{DYNAMIC_WALL_FAN, L"../Bin/Resource/Texture/Terrain/WALL/DYNAMIC_WALL/FAN.dds"}
	//{DYNAMIC_WALL_FAN_BLOOD, L"../Bin/Resource/Texture/Terrain/WALL/DYNAMIC_WALL/FAN_BLOOD.dds"}
};

vector<AnimationSource> CEditorDynamicWall::m_vAnimSource =
{
	{ DYNAMIC_WALL_FAN, 0, 2, 2, true, 0.5f}
	//{DYNAMIC_WALL_FAN_BLOOD, 0, 2, 2, true, 0.5f}
};

CEditorDynamicWall::CEditorDynamicWall(LPDIRECT3DDEVICE9 pGraphicDev)
	: CEditorWall(pGraphicDev)
	, m_pAnimationCom(nullptr)
{
	m_iWallType = DYNAMIC_WALL_FAN;
}

CEditorDynamicWall::~CEditorDynamicWall()
{
}

HRESULT CEditorDynamicWall::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_wstrName = L"DynamicWall";

    if (m_pAnimationCom)
    {
        m_pAnimationCom->Change_Animation(m_iWallType);
    }

    return S_OK;
}

_int CEditorDynamicWall::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = CEditorWall::Update_GameObject(fTimeDelta);

    if (m_pAnimationCom)
        m_pAnimationCom->Update_Component(fTimeDelta);

	return iExit;
}

void CEditorDynamicWall::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CEditorWall::LateUpdate_GameObject(fTimeDelta);
}

void CEditorDynamicWall::Render_GameObject()
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
    {
        m_pAnimationCom->Render_Animation();
    }

    if (m_bSelected)
    {
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
        m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 255, 100));  // 노란색 tint
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

HRESULT CEditorDynamicWall::Add_Component()
{
    Engine::CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = static_cast<Engine::CTransform*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[Engine::ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Buffer ( RcTex - Floor/Ceiling과 동일 )
    pComponent = m_pBufferCom = static_cast<Engine::CVIBuffer*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[Engine::ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTextureCom = static_cast<Engine::CTexture*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Dynamic_WallTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture", pComponent });

    // Animation
    pComponent = m_pAnimationCom = static_cast<Engine::CAnimation*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_WallAnimation"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });

	return S_OK;
}

CEditorDynamicWall* CEditorDynamicWall::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CEditorDynamicWall* pInstance = new CEditorDynamicWall(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorDynamicWall Create Failed");
        return nullptr;
    }

    // 위치 설정
    pInstance->Set_Position(vPos);

    // 기본 크기 (벽은 세로로 큼)
    pInstance->Set_Scale(_vec3(16.f, 16.f, 1.f));

    // 기본 방향: XY 평면
    pInstance->Set_WallDirection(WALL_XY_FRONT);

    return pInstance;
}

CEditorDynamicWall* CEditorDynamicWall::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, WALL_DIR eDir)
{
    CEditorDynamicWall* pInstance = new CEditorDynamicWall(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorDynamicWall Create Failed");
        return nullptr;
    }

    pInstance->Set_Position(vPos);
    pInstance->Set_Scale(_vec3(16.f, 16.f, 1.f));
    pInstance->Set_WallDirection(eDir);  // 방향 지정

    return pInstance;
}

CEditorDynamicWall* CEditorDynamicWall::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot, _vec3 vScale, WALL_DIR eDir)
{
    CEditorDynamicWall* pInstance = new CEditorDynamicWall(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorDynamicWall Create Failed");
        return nullptr;
    }

    // Transform 전체 지정 (맵 로드 시 사용)
    pInstance->Set_Scale(vScale);
    pInstance->Set_Rotation(vRot);
    pInstance->Set_Position(vPos);
    pInstance->m_eWallDir = eDir;  // 방향만 저장 (Set_WallDirection 호출 X, 회전은 이미 설정됨)

    return pInstance;
}

CEditorDynamicWall* CEditorDynamicWall::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot, _vec3 vScale, WALL_DIR eDir, _uint iType, _int iIdx)
{
    CEditorDynamicWall* pInstance = new CEditorDynamicWall(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorDynamicWall Create Failed");
        return nullptr;
    }

    // Transform 전체 지정 (맵 로드 시 사용)
    pInstance->Set_Scale(vScale);
    pInstance->Set_Rotation(vRot);
    pInstance->Set_Position(vPos);
    pInstance->m_eWallDir = eDir;  // 방향만 저장 (Set_WallDirection 호출 X, 회전은 이미 설정됨)
    pInstance->Set_WallType(iType);
    pInstance->Set_TextureIdx(iIdx);

    return pInstance;
}

void CEditorDynamicWall::Set_WallType(_uint eWallType)
{
    m_iWallType = eWallType;

    if (m_pAnimationCom)
    {
        m_pAnimationCom->Change_Animation(m_iWallType);
    }
}

void CEditorDynamicWall::Set_TextureIdx(_int iIdx)
{
    m_iTextureIdx = iIdx;
}

void CEditorDynamicWall::Free()
{
    CEditorWall::Free();
}
