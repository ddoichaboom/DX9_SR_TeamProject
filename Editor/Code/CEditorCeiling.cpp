#include "pch.h"
#include "CEditorCeiling.h"
#include "CRenderer.h"

#include "CProtoMgr.h"
#include "CTransform.h"
#include "CRcTex.h"

vector<TextureSource> CEditorCeiling::m_vTextureSource =
{
    {
        STATIC_CEILING,
        L"../Bin/Resource/Texture/Terrain/Floor/STATIC_FLOOR/FLOORS.dds",  // Floor와 같은 텍스처
        true,
        0,
        7,
        7,
        {2.f, 2.f}
    }
};

CEditorCeiling::CEditorCeiling(LPDIRECT3DDEVICE9 pGraphicDev)
    : CEditorObject(pGraphicDev)
    , m_pTextureCom(nullptr)
    , m_iCeilingType(STATIC_CEILING)
    , m_iTextureIdx(0)
{
}

CEditorCeiling::~CEditorCeiling()
{
}

HRESULT CEditorCeiling::Ready_GameObject()
{
    FAILED_CHECK_RETURN(CEditorObject::Ready_GameObject(), E_FAIL);
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    // 기본 이름
    m_wstrName = L"Ceiling";

    return S_OK;
}

_int CEditorCeiling::Update_GameObject(const _float& fTimeDelta)
{
    CEditorObject::Update_GameObject(fTimeDelta);

    // Renderer에 추가
    Engine::CRenderer::GetInstance()->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

    return 0;
}

void CEditorCeiling::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CEditorObject::LateUpdate_GameObject(fTimeDelta);
}

void CEditorCeiling::Render_GameObject()
{
    // 텍스처 스테이트 저장 
    DWORD dwOldColorOp, dwOldColorArg1, dwOldColorArg2, dwOldTextureFactor;

    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwOldColorOp);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG1, &dwOldColorArg1);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG2, &dwOldColorArg2);
    m_pGraphicDev->GetRenderState(D3DRS_TEXTUREFACTOR, &dwOldTextureFactor);

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    if (m_pTextureCom)
    {
        m_pTextureCom->Render_Texture();
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

HRESULT CEditorCeiling::Add_Component()
{
    Engine::CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[Engine::ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Buffer ( RcTex - Floor와 동일 )
    pComponent = m_pBufferCom = dynamic_cast<Engine::CVIBuffer*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[Engine::ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Static_CeilingTexture"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[Engine::ID_DYNAMIC].insert({ L"Com_Texture", pComponent });

    if (m_pTextureCom)
    {
        m_pTextureCom->Change_Texture(m_iCeilingType);
        m_pTextureCom->Set_Frame(_vec2(m_iTextureIdx, 0));
    }

    return S_OK;
}

void CEditorCeiling::Set_CeilingType(_uint eCeilingType)
{
    m_iCeilingType = eCeilingType;

    if (m_pTextureCom)
    {
        m_pTextureCom->Change_Texture(m_iCeilingType);
        m_pTextureCom->Set_Frame(_vec2(m_iTextureIdx, 0));
    }
}

void CEditorCeiling::Set_TextureIdx(_int iIdx)
{
    m_iTextureIdx = iIdx;

    if (m_pTextureCom)
    {
        m_pTextureCom->Set_Frame(_vec2(m_iTextureIdx, 0));
    }
}

CEditorCeiling* CEditorCeiling::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CEditorCeiling* pInstance = new CEditorCeiling(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorCeiling Create Failed");
        return nullptr;
    }

    // 위치 설정
    pInstance->Set_Position(vPos);

    // 기본 크기
    pInstance->Set_Scale(_vec3(8.f, 8.f, 1.f));

    // 기본 회전: XZ 평면 (천장, Floor와 반대)
    pInstance->Set_Rotation(_vec3(-90.f, 0.f, 0.f));

    return pInstance;
}

CEditorCeiling* CEditorCeiling::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot, _vec3 vScale)
{
    CEditorCeiling* pInstance = new CEditorCeiling(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorCeiling Create Failed");
        return nullptr;
    }

    // Transform 전체 지정 (맵 로드 시 사용)
    pInstance->Set_Scale(vScale);
    pInstance->Set_Rotation(vRot);
    pInstance->Set_Position(vPos);

    return pInstance;
}

CEditorCeiling* CEditorCeiling::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot, _vec3 vScale, _uint iType, _int iIdx, _int iRoomIndex)
{
    CEditorCeiling* pInstance = new CEditorCeiling(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorCeiling Create Failed");
        return nullptr;
    }

    // Transform 전체 지정 (맵 로드 시 사용)
    pInstance->Set_Scale(vScale);
    pInstance->Set_Rotation(vRot);
    pInstance->Set_Position(vPos);
    pInstance->Set_CeilingType(iType);
    pInstance->Set_TextureIdx(iIdx);

    return pInstance;
}

void CEditorCeiling::Free()
{
    CEditorObject::Free();
}