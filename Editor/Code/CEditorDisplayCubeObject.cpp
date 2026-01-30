#include "pch.h"
#include "CEditorDisplayCubeObject.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

vector<TextureSource> CEditorDisplayCubeObject::m_vTextureSource =
{
    {BOX, L"../Bin/Resource/Texture/Object/DisplayObject/Box.dds"},
    {CONCRETE_BLOCK, L"../Bin/Resource/Texture/Object/DisplayObject/CONCRETE_BLOCK.dds"},
    {WALL_BLOCK, L"../Bin/Resource/Texture/Object/DisplayObject/WALL_BLOCK.dds"}
};

CEditorDisplayCubeObject::CEditorDisplayCubeObject(LPDIRECT3DDEVICE9 pGraphicDev)
    : CEditorObject(pGraphicDev)
    , m_pCubeBufferCom(nullptr)
    , m_pCubeTextureCom(nullptr)
    , m_eObjectType(BOX)
{
}

CEditorDisplayCubeObject::~CEditorDisplayCubeObject()
{
}

HRESULT CEditorDisplayCubeObject::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_wstrName = L"DisplayCubeObject";

    if (m_pCubeTextureCom)
        m_pCubeTextureCom->Change_Texture(static_cast<_int>(m_eObjectType));

    if (m_pTransformCom)
        m_pTransformCom->Set_Scale(_vec3(8.f, 8.f, 8.f));

    return S_OK;
}

_int CEditorDisplayCubeObject::Update_GameObject(const _float& fTimeDelta)
{
    CEditorObject::Update_GameObject(fTimeDelta);

    // Renderer에 추가 
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    _vec3 info;
    m_pTransformCom->Get_Info(INFO_POS, &info);
    Compute_ViewZ(&info);

    return 0;
}

void CEditorDisplayCubeObject::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CEditorObject::LateUpdate_GameObject(fTimeDelta);
}

void CEditorDisplayCubeObject::Render_GameObject()
{
    // 텍스처 스테이트 저장 
    DWORD dwOldColorOp, dwOldColorArg1, dwOldColorArg2, dwOldTextureFactor, dwOldTTF;

    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwOldColorOp);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG1, &dwOldColorArg1);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG2, &dwOldColorArg2);
    m_pGraphicDev->GetRenderState(D3DRS_TEXTUREFACTOR, &dwOldTextureFactor);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, &dwOldTTF);

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    // 텍스처가 있으면 렌더링 
    if (m_pCubeTextureCom)
        m_pCubeTextureCom->Render_Texture();

    if (m_bSelected)
    {
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
        m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 255, 100));  // 노란색 tint
    }

    m_pCubeBufferCom->Render_Buffer();

    // 텍스처 스테이트 복원
    m_pGraphicDev->SetTexture(0, nullptr);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, dwOldColorOp);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, dwOldColorArg1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, dwOldColorArg2);
    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwOldTextureFactor);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, dwOldTTF);

}

HRESULT CEditorDisplayCubeObject::Add_Component()
{
    Engine::CComponent* pComponent = nullptr;

    // CubeTex
    pComponent = m_pCubeBufferCom = static_cast<Engine::CCubeTex*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CubeTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Transform
    pComponent = m_pTransformCom = static_cast<Engine::CTransform*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Texture 
    pComponent = m_pCubeTextureCom = static_cast<Engine::CCubeTexture*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_DisplayCubeObject_Texture"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

void CEditorDisplayCubeObject::Free()
{
    CEditorObject::Free();
}

void	CEditorDisplayCubeObject::Set_CubeObjectType(DISPLAY_CUBE_OBJECT_TYPE eType)
{
    m_eObjectType = eType;

    if (m_pCubeTextureCom)
    {
        m_pCubeTextureCom->Change_Texture(static_cast<_int>(m_eObjectType));
    }

    if (!m_pTransformCom)
        return;

    switch (m_eObjectType)
    {
    case BOX:
        m_pTransformCom->Set_Scale(_vec3(8.f, 8.f, 8.f));
        break;

    case CONCRETE_BLOCK:
        m_pTransformCom->Set_Scale(_vec3(16.f, 16.f, 16.f));
        break;

    case WALL_BLOCK:
        m_pTransformCom->Set_Scale(_vec3(16.f, 16.f, 16.f));
        break;
    }
}

CEditorDisplayCubeObject* CEditorDisplayCubeObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CEditorDisplayCubeObject* pInstance = new CEditorDisplayCubeObject(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorDisplayCubeObject Create Failed");
        return nullptr;
    }

    // 위치 설정
    pInstance->Set_Position(vPos);

    return pInstance;
}

CEditorDisplayCubeObject* CEditorDisplayCubeObject::Create(LPDIRECT3DDEVICE9 pGraphicDev,
                                                            _vec3 vPos, _vec3 vRot, _vec3 vScale,
                                                                DISPLAY_CUBE_OBJECT_TYPE eType)
{
    CEditorDisplayCubeObject* pInstance = new CEditorDisplayCubeObject(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorDisplayCubeObject Create Failed");
        return nullptr;
    }

    // 위치 설정
    pInstance->Set_Position(vPos);
    pInstance->Set_Rotation(vRot);
    pInstance->Set_Scale(vScale);
    pInstance->Set_CubeObjectType(eType);

    return pInstance;
}
