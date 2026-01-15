#include "pch.h"
#include "CShopItem.h"
#include "CProtoMgr.h"
#include "CRenderer.h"


vector<TextureSource> CShopItem::m_vTextureSource =
{
    {0, L"../Bin/Resource/Texture/UI/SHOP_KATANA.dds"},
    {1, L"../Bin/Resource/Texture/UI/SHOP_ITEM1.dds"},
    {2, L"../Bin/Resource/Texture/UI/SHOP_ITEM2.dds"},
};

CShopItem::CShopItem(LPDIRECT3DDEVICE9 pGraphicDev)
    : CBaseUI(pGraphicDev)
    , m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
    , m_iTextureID(0), m_bRender(false)
{

}

CShopItem::CShopItem(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _uint iTextureID)
    : CBaseUI(pGraphicDev)
    , m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
    , m_iTextureID(iTextureID), m_bRender(false)
{
    m_fX = fX;
    m_fY = fY;
}

CShopItem::CShopItem(const CShopItem& rhs)
    : CBaseUI(rhs)
    , m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
    , m_iTextureID(0), m_bRender(false)
{
}

CShopItem::~CShopItem()
{
}

CShopItem* CShopItem::Create(PDIRECT3DDEVICE9 pGraphicDev)
{
    CShopItem* pBG = new CShopItem(pGraphicDev);
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

CShopItem* CShopItem::Create(PDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _uint iTextureID)
{
    CShopItem* pBG = new CShopItem(pGraphicDev, fX, fY,iTextureID);
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

HRESULT CShopItem::Add_Component()
{
    Engine::CComponent* pComponent = nullptr;

    //VIBuffer
    pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_ShopItemTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

void CShopItem::Free()
{
    CGameObject::Free();
}

HRESULT CShopItem::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    //m_fX = 260.f;
    //m_fY = WINCY - 205.f;

    m_fSizeX = 180.f;
    m_fSizeY = 220.f;

    
    SetScale(m_fSizeX, m_fSizeY);
    SetPos({ m_fX, m_fY, 0.f });

    m_pTextureCom->Change_Texture(m_iTextureID);

    return S_OK;
}

_int CShopItem::Update_GameObject(const _float& fTimeDelta)
{
    //if (m_bRender == false)
    //    return 0;
    //

    _int iExit = CGameObject::Update_GameObject(fTimeDelta);
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
    

    return iExit;
}

void CShopItem::LateUpdate_GameObject(const _float& fTimeDelta)
{    
    //if (m_bRender == false)
    //    return;
    //
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CShopItem::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pTextureCom->Render_Texture();
    m_pBufferCom->Render_Buffer();
}

void CShopItem::Rotate(ROTATION eType, const _float& fAngle)
{
    m_pTransformCom->Rotation(eType, fAngle);
}

void CShopItem::SetPos(_vec3 _pos)
{
    m_pTransformCom->Set_Pos(_pos.x - WINCX * 0.5f, -_pos.y + WINCY * 0.5f, 0.f);
}

void CShopItem::SetScale(_float fCX, _float fCY)
{
    m_pTransformCom->Set_Scale(fCX * 0.5f, fCY * 0.5f, 1.f);
}
