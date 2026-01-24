#include "pch.h"
#include "CInfoUI.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

#include "CFontUI.h"
#include "CManagement.h"
#include "CUIManager.h"

TextureSource CInfoUI::m_vTextureSource =
{
    0, L"../Bin/Resource/Texture/UI/UI_Guide.dds"
};

CInfoUI::CInfoUI(LPDIRECT3DDEVICE9 pGraphicDev)
    : CBaseUI(pGraphicDev)
    , m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
    , m_fTime(0.f) , m_pTimeText(nullptr), m_bRender(false)
    , m_pStageText(nullptr), m_bDelay(false)
    
{
    m_iOrder = 1;    
}


CInfoUI::CInfoUI(const CInfoUI& rhs)
    : CBaseUI(rhs)
    , m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
    , m_fTime(0.f), m_pTimeText(nullptr), m_bRender(false)
    , m_pStageText(nullptr), m_bDelay(false)
    
{
    m_iOrder = 1;
}

CInfoUI::~CInfoUI()
{
}

CInfoUI* CInfoUI::Create(PDIRECT3DDEVICE9 pGraphicDev)
{
    CInfoUI* pInfo = new CInfoUI(pGraphicDev);
    if (!pInfo)
        return nullptr;

    if (FAILED(pInfo->Ready_GameObject()))
    {
        Safe_Release(pInfo);
        MSG_BOX("Phone BackGround Create Failed");
        return nullptr;
    }
    return pInfo;
}

HRESULT CInfoUI::Add_Component()
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
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_InfoUITexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

void CInfoUI::Free()
{
    Safe_Release(m_pTimeText);
    Safe_Release(m_pStageText);
    CGameObject::Free();
}

HRESULT CInfoUI::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_vPos = { 500.f, 325.f, 0.f };        
    m_fSizeX = 1100.f;
    m_fSizeY = 100.f;

    m_vTimePos = { -300.f,-25.f,0.f };
    m_vTimeScale = { 1100.f, 100.f, 0.f };

    SetScale(m_fSizeX, m_fSizeY);
    SetPos(m_vPos);

    m_pTextureCom->Change_Texture(0);

    m_pTimeText = CFontUI::Create(FONT_LARGENUMBER, m_vTimePos, m_vTimeScale);
    if (nullptr == m_pTimeText)
        return E_FAIL;

    m_pTimeText->Set_Parent(this);

    m_pStageText = CFontUI::Create(FONT_NUMBER, {0.f, -65.f, 0.f}, m_vTimeScale);
    if (nullptr == m_pStageText)
        return E_FAIL;

    m_pStageText->Set_Parent(this);

    return S_OK;
}

_int CInfoUI::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);    

    if (m_bRender)
    {
        m_fTime += fTimeDelta;

        if (m_fTime > 2.5f)
        {
            m_bRender = false;
            return iExit;
        }
        else if (m_fTime > 1.f)
        {
            if (m_bDelay)
            {
                CUIManager::GetInstance()->Set_OnEffectUI(CUIManager::ES_CLEAR);
                m_bDelay = false;
            }
            
            CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
        }
        
    }   
  

    return iExit;
}

void CInfoUI::LateUpdate_GameObject(const _float& fTimeDelta)
{
    if (m_bRender == false)
        return;
    CGameObject::LateUpdate_GameObject(fTimeDelta);   
    if (m_fTime > 1.f)
    {
        m_pTimeText->LateUpdate_GameObject(fTimeDelta);
        m_pStageText->LateUpdate_GameObject(fTimeDelta);
    }    
}

void CInfoUI::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pTextureCom->Render_Texture();
    m_pBufferCom->Render_Buffer();
}

void CInfoUI::Rotate(ROTATION eType, const _float& fAngle)
{
    m_pTransformCom->Rotation(eType, fAngle);
}

void CInfoUI::SetPos(_vec3 _pos)
{
    m_pTransformCom->Set_Pos(_pos.x - WINCX * 0.5f, -_pos.y + WINCY * 0.5f, 0.f);
}

void CInfoUI::SetScale(_float fCX, _float fCY)
{
    m_pTransformCom->Set_Scale(fCX * 0.5f, fCY * 0.5f, 1.f);
}

void CInfoUI::Set_On()
{
    m_fTime = 0.f;
    m_bRender = true;
    m_bDelay = true;
    m_pTextureCom->Change_Texture(0);
    wstring wText = CManagement::GetInstance()->Convert_PlayTime();
    m_pTimeText->Set_Text(wText);

    wText = CManagement::GetInstance()->Convert_StageInfo();
    m_pStageText->Set_Text(wText);
    m_pStageText->Set_Color(D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
}

void CInfoUI::Set_Off()
{
}
