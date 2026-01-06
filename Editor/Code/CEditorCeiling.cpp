#include "pch.h"
#include "CEditorCeiling.h"
#include "CRenderer.h"

#include "CProtoMgr.h"
#include "CTransform.h"
#include "CRcTex.h"

CEditorCeiling::CEditorCeiling(LPDIRECT3DDEVICE9 pGraphicDev)
    : CEditorObject(pGraphicDev)
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
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    // 텍스처 설정 (기본 흰색)
    if (nullptr == m_pTextureCom)
    {
        m_pGraphicDev->SetTexture(0, nullptr);
    }
    else
    {
        //m_pTextureCom->Set_Texture(0);
    }

    // 선택 상태 표시
    if (m_bSelected)
    {
        // 선택됨: 노란색 (모든 타입 공통)
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
        m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 255, 0));  // 노란색
    }
    else
    {
        // 미선택: 연한 파란색 (하늘/천장 느낌)
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
        m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 150, 180, 220));  // 연한 파란색
    }

    m_pBufferCom->Render_Buffer();

    // 렌더 상태 복원
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
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

    return S_OK;
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
    pInstance->Set_Scale(_vec3(4.f, 4.f, 1.f));

    // 기본 회전: XZ 평면 (천장, Floor와 반대)
    pInstance->Set_Rotation(_vec3(90.f, 0.f, 0.f));

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

void CEditorCeiling::Free()
{
    CEditorObject::Free();
}