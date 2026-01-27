#include "pch.h"
#include "CStageBG.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

#include "CManagement.h"

vector<TextureSource> CStageBG::m_vTextureSource =
{
    {1, L"../Bin/Resource/Texture/UI/STAGE_UI_FLOOR1.dds"},
    {2, L"../Bin/Resource/Texture/UI/STAGE_UI_FLOOR2.dds"},
};

CStageBG::CStageBG(LPDIRECT3DDEVICE9 pGraphicDev)
    : CBaseUI(pGraphicDev)
    , m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
    , m_iStageNum(0), m_bRender(false)
{
    m_iOrder = 5;
}


CStageBG::CStageBG(const CStageBG& rhs)
    : CBaseUI(rhs)
    , m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
    , m_iStageNum(rhs.m_iStageNum), m_bRender(false)
{
    m_iOrder = 5;
}

CStageBG::~CStageBG()
{
}

CStageBG* CStageBG::Create(PDIRECT3DDEVICE9 pGraphicDev)
{
    CStageBG* pBG = new CStageBG(pGraphicDev);
    if (!pBG)
        return nullptr;

    if (FAILED(pBG->Ready_GameObject()))
    {
        Safe_Release(pBG);
        MSG_BOX("Phone BackGround Create Failed");
        return nullptr;
    }
    return pBG;
}

HRESULT CStageBG::Add_Component()
{
    Engine::CComponent* pComponent = nullptr;

    //VIBuffer
    pComponent = m_pBufferCom = static_cast<Engine::CRcTex*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Transform
    pComponent = m_pTransformCom = static_cast<Engine::CTransform*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    pComponent = m_pTextureCom = static_cast<Engine::CTexture*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_StageBGTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

void CStageBG::Free()
{
    CGameObject::Free();
}

HRESULT CStageBG::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_fX = WINCX * 0.5f;
    m_fY = WINCY * 0.5f;
    m_fSizeX = WINCX + 190.f;
    m_fSizeY = WINCY + 10.f;


    SetScale(m_fSizeX, m_fSizeY);
    SetPos({ m_fX, m_fY, 0.f });

    
    //m_pTextureCom->Change_Texture(m_iStageNum);
    m_pTextureCom->Change_Texture(1);

    return S_OK;
}

_int CStageBG::Update_GameObject(const _float& fTimeDelta)
{
    if (IsDead()) return RET_DEAD;
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);

    return iExit;
}

void CStageBG::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CStageBG::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pTextureCom->Render_Texture();
    m_pBufferCom->Render_Buffer();
}

void CStageBG::Set_On()
{
    _uint iFloorNumber = CManagement::GetInstance()->Get_FloorNumber();
    m_pTextureCom->Change_Texture(iFloorNumber);
}

void CStageBG::Rotate(ROTATION eType, const _float& fAngle)
{
    m_pTransformCom->Rotation(eType, fAngle);
}

void CStageBG::SetPos(_vec3 _pos)
{
    m_pTransformCom->Set_Pos(_pos.x - WINCX * 0.5f, -_pos.y + WINCY * 0.5f, 0.f);
}

void CStageBG::SetScale(_float fCX, _float fCY)
{
    m_pTransformCom->Set_Scale(fCX * 0.5f, fCY * 0.5f, 1.f);
}
