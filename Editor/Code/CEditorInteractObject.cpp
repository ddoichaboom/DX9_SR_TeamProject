#include "pch.h"
#include "CEditorInteractObject.h"
#include "CRenderer.h"
#include "CProtoMgr.h"
#include "CTexture.h"

vector<TextureSource> CEditorInteractObject::m_vTextureSource =
{
    {0, L"../Bin/Resource/Texture/Object/Extinguisher.dds"},
    {1, L"../Bin/Resource/Texture/Object/Axe.dds"}
};


CEditorInteractObject::CEditorInteractObject(LPDIRECT3DDEVICE9 pGraphicDev)
    : CEditorObject(pGraphicDev)
    , m_eObjectItemType(ITEM_EXTINGUISHER)
{
}

CEditorInteractObject::~CEditorInteractObject()
{
}


HRESULT CEditorInteractObject::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    // 기본 이름
    m_wstrName = L"InteractObject";

    if (m_pTextureCom)
        m_pTextureCom->Change_Texture(0);

    if (m_pTransformCom)
        m_pTransformCom->Set_Scale(_vec3(2.f, 4.f, 1.f));       // 기본 설정이 소화기이므로 소화기의 크기로 우선 설정

    return S_OK;
}

_int    CEditorInteractObject::Update_GameObject(const _float& fTimeDelta)
{
    CEditorObject::Update_GameObject(fTimeDelta);

    // Renderer에 추가 
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    _vec3 info;
    m_pTransformCom->Get_Info(INFO_POS, &info);
    Compute_ViewZ(&info);

    return 0;
}

void    CEditorInteractObject::LateUpdate_GameObject(const _float& fTimeDelta)
{
    SetBillBoard();
    CEditorObject::LateUpdate_GameObject(fTimeDelta);
}

void    CEditorInteractObject::Render_GameObject()
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

HRESULT CEditorInteractObject::Add_Component()
{
    Engine::CComponent* pComponent = nullptr;

    //VIBuffer
    pComponent = m_pBufferCom = static_cast<Engine::CRcTex*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Transform
    pComponent = m_pTransformCom = static_cast<Engine::CTransform*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[Engine::ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    //Texutre
    pComponent = m_pTextureCom = static_cast<Engine::CTexture*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_InteractObject_Texture"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

void    CEditorInteractObject::SetBillBoard()
{
    _matrix matView, matBill;

    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
    D3DXMatrixInverse(&matView, NULL, &matView);
    _vec3 camPos;
    memcpy(&camPos, &matView.m[3], sizeof(_vec3));
    _vec3 myPos = *m_pTransformCom->Get_Info(INFO_POS);
    _vec3 myScale = m_pTransformCom->m_vScale;

    //방향 주의! 카메라의 방향을 처다봐야 뒷면이 랜더링 됨 
    _vec3 look = myPos - camPos;
    look.y = 0.0f;
    D3DXVec3Normalize(&look, &look);

    _vec3 right;
    _vec3 up = { 0.0f, 1.0f, 0.0f };
    D3DXVec3Cross(&right, &up, &look);
    D3DXVec3Normalize(&right, &right);

    D3DXVec3Cross(&up, &look, &right);
    D3DXVec3Normalize(&up, &up);

    D3DXMatrixIdentity(&matBill);
    right *= myScale.x;
    up *= myScale.y;
    look *= myScale.z;

    memcpy(&matBill.m[0], &right, sizeof(_vec3));
    memcpy(&matBill.m[1], &up, sizeof(_vec3));
    memcpy(&matBill.m[2], &look, sizeof(_vec3));
    memcpy(&matBill.m[3], &myPos, sizeof(_vec3));
    m_pTransformCom->Set_World(&matBill);
}

void    CEditorInteractObject::Set_ItemType(OBJ_ITEM_TYPE eObjItemType)
{
    m_eObjectItemType = eObjItemType;

    if (m_pTextureCom)
    {
        switch (m_eObjectItemType)
        {
        case ITEM_EXTINGUISHER:
            m_pTextureCom->Change_Texture(0);
            if (m_pTransformCom)
                m_pTransformCom->Set_Scale(_vec3(2.f, 4.f, 1.f));
            break;
        case ITEM_AXE:
            m_pTextureCom->Change_Texture(1);
            if (m_pTransformCom)
                m_pTransformCom->Set_Scale(_vec3(4.f, 2.f, 1.f));
            break;
        default:
            m_pTextureCom->Change_Texture(0);
            if (m_pTransformCom)
                m_pTransformCom->Set_Scale(_vec3(2.f, 4.f, 1.f));
            break;
        }
    }
}

CEditorInteractObject* CEditorInteractObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CEditorInteractObject* pInstance = new CEditorInteractObject(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorInteractObject Create Failed");
        return nullptr;
    }

    // 위치 설정
    pInstance->Set_Position(vPos);

    return pInstance;
}

CEditorInteractObject* CEditorInteractObject::Create(LPDIRECT3DDEVICE9 pGraphicDev,
                                                        _vec3 vPos, OBJ_ITEM_TYPE iType)
{
    CEditorInteractObject* pInstance = new CEditorInteractObject(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorInteractObject Create Failed");
        return nullptr;
    }

    // Transform 전체 지정 (맵 로드 시 사용)
    pInstance->Set_Position(vPos);
    pInstance->Set_ItemType(iType);

    return pInstance;
}

void    CEditorInteractObject::Free()
{
    CEditorObject::Free();
}

 