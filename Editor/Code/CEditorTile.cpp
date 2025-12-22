#include "pch.h"
#include "CEditorTile.h"
#include "CRenderer.h"

#include "CProtoMgr.h"
#include "CTransform.h"
#include "CRcTex.h"

CEditorTile::CEditorTile(LPDIRECT3DDEVICE9 pGraphicDev)
    : CEditorObject(pGraphicDev)
{
}

CEditorTile::~CEditorTile()
{
}

HRESULT CEditorTile::Ready_GameObject()
{
    FAILED_CHECK_RETURN(CEditorObject::Ready_GameObject(), E_FAIL);
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    // 기본 이름
    m_wstrName = L"Tile";

    return S_OK;
}

_int CEditorTile::Update_GameObject(const _float& fTimeDelta)
{
    CEditorObject::Update_GameObject(fTimeDelta);

    // Renderer에 추가
    Engine::CRenderer::GetInstance()->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

    return 0;
}

void CEditorTile::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CEditorObject::LateUpdate_GameObject(fTimeDelta);
}

void CEditorTile::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    // 텍스처가 없으면 기본 흰색
    if (nullptr == m_pTextureCom)
    {
        m_pGraphicDev->SetTexture(0, nullptr);
    }
    else
    {
        m_pTextureCom->Set_Texture(0);
    }

    // 선택 상태 표시 ( 색상 변경 )
    if (m_bSelected)
    {

        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
        m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 255, 0));    //  노란색
    }
    else
    {
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
        m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 255, 255));  // 흰색
    }

    m_pBufferCom->Render_Buffer();

    // 상태값 복원
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
}

HRESULT CEditorTile::Add_Component()
{
    Engine::CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[Engine::ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Buffer ( RcTex )
    pComponent = m_pBufferCom = dynamic_cast<Engine::CVIBuffer*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[Engine::ID_STATIC].insert({ L"Com_Buffer", pComponent });

    return S_OK;
}

CEditorTile* CEditorTile::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CEditorTile* pInstance = new CEditorTile(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorTile Create Failed");
        return nullptr;
    }

    // 위치 설정
    pInstance->Set_Position(vPos);

    // 기본 크기
    pInstance->Set_Scale(_vec3(1.f, 1.f, 1.f));

    // XZ 평면으로 회전 ( X축 - 90도 )
    pInstance->Set_Rotation(_vec3(-90.f, 0.f, 0.f));

    return pInstance;
}

void CEditorTile::Free()
{
    CEditorObject::Free();
}