#include "pch.h"
#include "CDisplayCubeObject.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

vector<TextureSource> CDisplayCubeObject::m_vTextureSource =
{
    {BOX, L"../Bin/Resource/Texture/Object/DisplayObject/Box.dds"},
    {CONCRETE_BLOCK, L"../Bin/Resource/Texture/Object/DisplayObject/CONCRETE_BLOCK.dds"},
};

CDisplayCubeObject::CDisplayCubeObject(LPDIRECT3DDEVICE9 pGraphicDev)
    : CDisplayObject(pGraphicDev)
    , m_pCubeBufferCom(nullptr), m_pCubeTextureCom(nullptr)
{
    m_eOBJ_ID = OBJ_DISPLAY;
}

CDisplayCubeObject::CDisplayCubeObject(const CDisplayCubeObject& rhs)
    : CDisplayObject(rhs)
    , m_pCubeBufferCom(rhs.m_pCubeBufferCom)
    , m_pCubeTextureCom(rhs.m_pCubeTextureCom)
{
    m_eOBJ_ID = rhs.m_eOBJ_ID;
}

CDisplayCubeObject::~CDisplayCubeObject()
{
}

CDisplayCubeObject* CDisplayCubeObject::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CDisplayCubeObject* pDisplay = new CDisplayCubeObject(pGraphicDev);

    if (FAILED(pDisplay->Ready_GameObject()))
    {
        Safe_Release(pDisplay);
        MSG_BOX("Display Cube Object Create Failed");
        return nullptr;
    }

    return pDisplay;
}

HRESULT CDisplayCubeObject::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

_int CDisplayCubeObject::Update_GameObject(const _float& fTimeDelta)
{
    if (IsDead())
        return RET_DEAD;

    int iExit = CDisplayObject::Update_GameObject(fTimeDelta);

    return iExit;
}

void CDisplayCubeObject::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CDisplayObject::LateUpdate_GameObject(fTimeDelta);
}

void CDisplayCubeObject::Render_GameObject()
{
    DWORD dOldCullMode, dwOldTTF;
    m_pGraphicDev->GetRenderState(D3DRS_CULLMODE, &dOldCullMode);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, &dwOldTTF);

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3);

    // 텍스처가 있으면 렌더링 
    if (m_pTextureCom)
        m_pTextureCom->Render_Texture();

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, dwOldTTF);
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, dOldCullMode);
    m_pGraphicDev->SetTexture(0, nullptr);
}

HRESULT CDisplayCubeObject::Add_Component()
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

    m_mapComponent[ID_STATIC].insert({ L"Com_Transform", pComponent });

    // Texture 
    pComponent = m_pCubeTextureCom = static_cast<Engine::CCubeTexture*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_DisplayCubeObject_Texture"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

void CDisplayCubeObject::Free()
{
    CDisplayObject::Free();
}
