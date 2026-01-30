#include "pch.h"
#include "CEditorDisplayObject.h"
#include "CRenderer.h"
#include "CProtoMgr.h"

vector<TextureSource> CEditorDisplayObject::m_vTextureSource =
{
    {JUMP_BORDER, L"../Bin/Resource/Texture/Object/DisplayObject/JUMP_BORDER.dds"},
    {CABLES, L"../Bin/Resource/Texture/Object/DisplayObject/CABLES.dds", true, 0, 3, 3, {1.f, 1.f}},
    {PALMS, L"../Bin/Resource/Texture/Object/DisplayObject/PALMS.dds"},
    {PASSARELA, L"../Bin/Resource/Texture/Object/DisplayObject/PASSARELA.dds"},
    {RUG, L"../Bin/Resource/Texture/Object/DisplayObject/RUG.dds", true, 0, 3, 3, {1.f, 1.f}},
    {SIGNS, L"../Bin/Resource/Texture/Object/DisplayObject/SIGNS.dds", true, 0, 6, 6, {1.f, 1.f}},
    {PLATE, L"../Bin/Resource/Texture/Object/DisplayObject/PLATE.dds"},
    {ROAD_CORNER, L"../Bin/Resource/Texture/Object/DisplayObject/ROAD_CORNER.dds"},
    {ROAD_PLATE, L"../Bin/Resource/Texture/Object/DisplayObject/ROAD_PLATE.dds", true, 0, 3, 3, {1.f, 1.f}},
    {TRAFFIC_LIGHTS, L"../Bin/Resource/Texture/Object/DisplayObject/TRAFFIC_LIGHTS.dds"},
    {TRAFFIC_SIGN_1, L"../Bin/Resource/Texture/Object/DisplayObject/TRAFFIC_SIGN_1.dds"},
    {TRAFFIC_SIGN_2, L"../Bin/Resource/Texture/Object/DisplayObject/TRAFFIC_SIGN_2.dds"},
    {TRAFFIC_SIGN_3, L"../Bin/Resource/Texture/Object/DisplayObject/TRAFFIC_SIGN_3.dds"},
    {WALL_WINDOW, L"../Bin/Resource/Texture/Object/DisplayObject/WALL_WINDOW.dds"}
};

CEditorDisplayObject::CEditorDisplayObject(LPDIRECT3DDEVICE9 pGraphicDev)
    : CEditorObject(pGraphicDev)
    , m_eObjectType(JUMP_BORDER)
    , m_iTextureID(0)
{
}

CEditorDisplayObject::~CEditorDisplayObject()
{
}

HRESULT CEditorDisplayObject::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_wstrName = L"DisplayObject";

    if (m_pTextureCom)
        m_pTextureCom->Change_Texture(0);

    if (m_pTransformCom)
        m_pTransformCom->Set_Scale(_vec3(16.f, 8.f, 1.f));

    return S_OK;
}

_int CEditorDisplayObject::Update_GameObject(const _float& fTimeDelta)
{
    CEditorObject::Update_GameObject(fTimeDelta);

    // Renderer에 추가 
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    _vec3 info;
    m_pTransformCom->Get_Info(INFO_POS, &info);
    Compute_ViewZ(&info);

    return 0;
}

void CEditorDisplayObject::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CEditorObject::LateUpdate_GameObject(fTimeDelta);
}

void CEditorDisplayObject::Render_GameObject()
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

HRESULT CEditorDisplayObject::Add_Component()
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
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_DisplayObject_Texture"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

void CEditorDisplayObject::Free()
{
    CEditorObject::Free();
}

void CEditorDisplayObject::Set_DisplayObjectType(DISPLAY_OBJECT_TYPE eObjectType, _uint iTextureId)
{
    m_eObjectType = eObjectType;
    m_iTextureID = iTextureId;

    if (m_pTextureCom)
    {
        m_pTextureCom->Change_Texture(static_cast<_int>(m_eObjectType));
        m_pTextureCom->Set_Frame(_vec2(iTextureId, 0));
    }

    if (!m_pTransformCom)
        return;

    switch (m_eObjectType)
    {
    case JUMP_BORDER:
        m_pTransformCom->Set_Scale(_vec3(16.f, 8.f, 1.f));
        break;

    case CABLES:
    {
        _vec3 vScale = { 0.f, 0.f, 0.f };

        if (iTextureId < 2)
            vScale = { 16.f, 8.f, 1.f };
        else
            vScale = { 16.f, 16.f, 1.f };

        m_pTransformCom->Set_Scale(vScale);

        break;
    }

    case PALMS:
        m_pTransformCom->Set_Scale(_vec3(8.f, 16.f, 1.f));
        break;

    case PASSARELA:
        m_pTransformCom->Set_Scale(_vec3(48.f, 16.f, 1.f));
        break;

    case RUG:
        m_pTransformCom->Set_Scale(_vec3(16.f, 16.f, 1.f));
        m_pTransformCom->Set_Angle(_vec3(90.f, 0.f, 0.f));
        break;

    case SIGNS:
    {
        _vec3 vScale = { 0.f, 0.f, 0.f };

        if (iTextureId < 3)
            vScale = { 8.f, 16.f, 1.f };
        else
            vScale = { 16.f, 8.f, 1.f };

        m_pTransformCom->Set_Scale(vScale);

        break;
    }
    case PLATE:
        m_pTransformCom->Set_Scale(_vec3(8.f, 64.f, 1.f));
        break;
    case ROAD_CORNER:
        m_pTransformCom->Set_Scale(_vec3(16.f, 16.f, 1.f));
        break;
    case ROAD_PLATE:
        m_pTransformCom->Set_Scale(_vec3(16.f, 16.f, 1.f));
        break;
    case TRAFFIC_LIGHTS:
        m_pTransformCom->Set_Scale(_vec3(16.f, 8.f, 1.f));
        break;
    case TRAFFIC_SIGN_1:
        m_pTransformCom->Set_Scale(_vec3(16.f, 64.f, 1.f));
        break;
    case TRAFFIC_SIGN_2:
        m_pTransformCom->Set_Scale(_vec3(16.f, 64.f, 1.f));
        break;
    case TRAFFIC_SIGN_3:
        m_pTransformCom->Set_Scale(_vec3(64.f, 8.f, 1.f));
        break;
    case WALL_WINDOW:
        m_pTransformCom->Set_Scale(_vec3(16.f, 16.f, 1.f));
        break;
    }

}

CEditorDisplayObject* CEditorDisplayObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CEditorDisplayObject* pInstance = new CEditorDisplayObject(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorDisplayObject Create Failed");
        return nullptr;
    }

    // 위치 설정
    pInstance->Set_Position(vPos);

    return pInstance;
}

CEditorDisplayObject* CEditorDisplayObject::Create(LPDIRECT3DDEVICE9 pGraphicDev,
                                                    _vec3 vPos, _vec3 vRot, _vec3 vScale,
                                                    DISPLAY_OBJECT_TYPE iType, _uint iTextureId)
{
    CEditorDisplayObject* pInstance = new CEditorDisplayObject(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorDisplayObject Create Failed");
        return nullptr;
    }

    // Transform 전체 지정 (맵 로드 시 사용)
    pInstance->Set_Position(vPos);
    pInstance->Set_DisplayObjectType(iType, iTextureId);
    pInstance->Set_Scale(vScale);
    pInstance->Set_Rotation(vRot);

    return pInstance;
}