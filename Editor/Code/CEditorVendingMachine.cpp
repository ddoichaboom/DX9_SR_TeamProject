#include "pch.h"
#include "CEditorVendingMachine.h"

#include "CProtoMgr.h"
#include "CTransform.h"
#include "CCubeTex.h"
#include "CRenderer.h"

CEditorVendingMachine::CEditorVendingMachine(LPDIRECT3DDEVICE9 pGraphicDev)
    : CEditorObject(pGraphicDev)
{
}

CEditorVendingMachine::~CEditorVendingMachine()
{
}

HRESULT CEditorVendingMachine::Ready_GameObject()
{
    FAILED_CHECK_RETURN(CEditorObject::Ready_GameObject(), E_FAIL);
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    if (m_pTextureCom) m_pTextureCom->Change_Texture(0);
    m_wstrName = L"VendingMachine";

    return S_OK;
}

_int CEditorVendingMachine::Update_GameObject(const _float& fTimeDelta)
{
    CEditorObject::Update_GameObject(fTimeDelta);

    Engine::CRenderer::GetInstance()->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

    return 0;
}

void CEditorVendingMachine::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CEditorObject::LateUpdate_GameObject(fTimeDelta);
}

void CEditorVendingMachine::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    DWORD dOldAlphaTest;
    m_pGraphicDev->GetRenderState(D3DRS_ALPHATESTENABLE, &dOldAlphaTest);

    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    if (nullptr == m_pTextureCom)
    {
        m_pGraphicDev->SetTexture(0, nullptr);
    }
    else
    {
        m_pTextureCom->Render_Texture();
    }

    // 선택 상태
    if (m_bSelected)
    {
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
        m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 255, 0));  // 노란색
    }
    else
    {
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
        m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 200, 200, 200));  // 회색
    }

    m_pBufferCom->Render_Buffer();

    // 상태값 복원 (다음 객체를 위해 기본 모드로 복원)
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, dOldAlphaTest);
}

HRESULT CEditorVendingMachine::Add_Component()
{
    Engine::CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = static_cast<Engine::CTransform*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[Engine::ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Buffer ( CubeTex )
    pComponent = m_pBufferCom = static_cast<Engine::CVIBuffer*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CubeTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[Engine::ID_STATIC].insert({ L"Com_Buffer", pComponent });

    return S_OK;
}

CEditorVendingMachine* CEditorVendingMachine::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CEditorVendingMachine* pInstance = new CEditorVendingMachine(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorVendingMachine Create Failed");
        return nullptr;
    }

    pInstance->Set_Position(vPos);
    pInstance->Set_Scale(_vec3(8.f, 12.f, 4.f));
    pInstance->Set_Rotation(_vec3(0.f, 180.f, 0.f));        // 앞면이 -Z 축으로 가게 기본 회전 
    return pInstance;
}

CEditorVendingMachine* CEditorVendingMachine::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot, _vec3 vScale)
{
    CEditorVendingMachine* pInstance = new CEditorVendingMachine(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorVendingMachine Create Failed");
        return nullptr;
    }

    pInstance->Set_Scale(vScale);
    pInstance->Set_Rotation(vRot);
    pInstance->Set_Position(vPos);

    return pInstance;
}

void CEditorVendingMachine::Free()
{
    CEditorObject::Free();
}