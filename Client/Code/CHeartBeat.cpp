#include "pch.h"
#include "CHeartBeat.h"
#include "CProtoMgr.h"
#include "CRenderer.h"


vector<TextureSource> CHeartBeat::m_vTextureSource =
{
    {0, L"../Bin/Resource/Texture/UI/UI_Heart_Line.dds"},
    {1, L"../Bin/Resource/Texture/UI/UI_Heart_Beat.dds"},    
};

CHeartBeat::CHeartBeat(LPDIRECT3DDEVICE9 pGraphicDev)
    : CBaseUI(pGraphicDev)
    , m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
    , m_fTime(0.f), m_iTextureID(0)
{
    m_iOrder = 6;
}

CHeartBeat::CHeartBeat(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _uint iTextureID)
    : CBaseUI(pGraphicDev)
    , m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
    , m_fTime(0.f), m_iTextureID(iTextureID)
{
    m_fX = fX;
    m_fY = fY;
    m_iOrder = 6;
}

CHeartBeat::CHeartBeat(const CHeartBeat& rhs)
    : CBaseUI(rhs)
    , m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
    , m_fTime(0.f),m_iTextureID(0)
{
    m_iOrder = 6;
}

CHeartBeat::~CHeartBeat()
{
}

CHeartBeat* CHeartBeat::Create(PDIRECT3DDEVICE9 pGraphicDev)
{
    CHeartBeat* pHeartBeat = new CHeartBeat(pGraphicDev);
    if (!pHeartBeat)
        return nullptr;

    if (FAILED(pHeartBeat->Ready_GameObject()))
    {
        Safe_Release(pHeartBeat);
        MSG_BOX("HeartBeat Create Failed");
        return nullptr;
    }
    return pHeartBeat;
}

CHeartBeat* CHeartBeat::Create(PDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _uint iTextureID)
{
    CHeartBeat* pHeartBeat = new CHeartBeat(pGraphicDev, fX, fY, iTextureID);
    if (!pHeartBeat)
        return nullptr;

    if (FAILED(pHeartBeat->Ready_GameObject()))
    {
        Safe_Release(pHeartBeat);
        MSG_BOX("HeartBeat Create Failed");
        return nullptr;
    }
    return pHeartBeat;
}

HRESULT CHeartBeat::Add_Component()
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
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_HeartBeatTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

void CHeartBeat::Free()
{    
    CGameObject::Free();
}

HRESULT CHeartBeat::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;


    if (m_iTextureID > 0)
    {
        m_fSizeX = 80.f;
        m_fSizeY = 80.f;

        m_vStartPos = { 400.f , WINCY - 90.f , 0.f };
        m_vEndPos = { 720.f , WINCY - 90.f , 0.f };

        SetScale(m_fSizeX, m_fSizeY);
        SetPos({ m_fX, m_fY, 0.f });

    }
    else
    {
        m_fSizeX = 380.f;
        m_fSizeY = 80.f;

        m_vStartPos = { 600.f , WINCY - 90.f , 0.f };
        m_vEndPos = { 720.f , WINCY - 90.f , 0.f };
        SetScale(m_fSizeX, m_fSizeY);
        SetPos({ m_fX, m_fY, 0.f });

    }
 
    
    m_pTextureCom->Change_Texture(m_iTextureID);

    return S_OK;
}

_int CHeartBeat::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);

    if (m_iTextureID > 0)
    {
        m_fTime += fTimeDelta * 1.25f;
        _vec3 vPos;
        if (m_fTime < 1.f)
        {
            //fTime = m_fDelayTime * 3.f;
            D3DXVec3Lerp(&vPos, &m_vStartPos, &m_vEndPos, m_fTime);
            SetPos(vPos);                        
        }
        else
        {
            SetPos(m_vStartPos);
            m_fTime = 0.f;
        }
    }
    return iExit;
}

void CHeartBeat::LateUpdate_GameObject(const _float& fTimeDelta)
{    
    CGameObject::LateUpdate_GameObject(fTimeDelta);

}

void CHeartBeat::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pTextureCom->Render_Texture();
    m_pBufferCom->Render_Buffer();
}

void CHeartBeat::Rotate(ROTATION eType, const _float& fAngle)
{
    m_pTransformCom->Rotation(eType, fAngle);
}

void CHeartBeat::SetPos(_vec3 _pos)
{
    m_pTransformCom->Set_Pos(_pos.x - WINCX * 0.5f, -_pos.y + WINCY * 0.5f, 0.f);
}

void CHeartBeat::SetScale(_float fCX, _float fCY)
{
    m_pTransformCom->Set_Scale(fCX * 0.5f, fCY * 0.5f, 1.f);
}
