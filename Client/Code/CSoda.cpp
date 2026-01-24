#include "pch.h"
#include "CSoda.h"
#include "CProtoMgr.h"
#include "CPoolMgr.h"
#include "CRenderer.h"
#include "CEventMgr.h"
#include "CPlayer.h"

TextureSource CSoda::m_vTextureSource =
{
    0, L"../Bin/Resource/Texture/Object/Soda.dds"
};


CSoda::CSoda(LPDIRECT3DDEVICE9 pGraphicDev)
    : CInteractObject(pGraphicDev), m_pMainCollider(nullptr)
{
    m_eItemType = ITEM_SODA;
}

CSoda::CSoda(const CSoda& rhs)
    : CInteractObject(rhs), m_pMainCollider(nullptr)
{
    m_eItemType = ITEM_SODA;
}

CSoda::~CSoda()
{
}

CSoda* CSoda::Create(PDIRECT3DDEVICE9 pGraphicDev)
{
    CSoda* pSoda = new CSoda(pGraphicDev);

    if (FAILED(pSoda->Ready_GameObject()))
    {
        Safe_Release(pSoda);
        MSG_BOX("Soda Create Failed");
        return nullptr;
    }

    return pSoda;
}

HRESULT CSoda::Ready_GameObject()
{
    if (FAILED(CInteractObject::Add_Component())) 
        return E_FAIL;

    if (FAILED(Add_Component())) 
        return E_FAIL;

    m_pTransformCom->m_vScale = { 2.f, 1.f, 1.f };

    //Collider 생성
    m_pMainCollider = m_pCollisionCom->CreateCollider(this, m_szMainColliderName);
    if (!m_pMainCollider) return E_FAIL;
    m_pMainCollider->Set_RelativePos(_vec3(0.f, 0.f, 0.f));
    m_pMainCollider->Set_Scale(_vec3(5.f, 5.f, 5.f));

    m_pMainCollider->BindFuncToCollision([&](CollisionInfo info)
        {
            OnCollision(info);
        });


    m_pTextureCom->Change_Texture(m_iTextureID);
    return S_OK;
}

_int CSoda::Update_GameObject(const _float& fTimeDelta)
{
    if (IsDead()) 
        return RET_DEAD;

    int iExit = CGameObject::Update_GameObject(fTimeDelta);
    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    _vec3 info;
    m_pTransformCom->Get_Info(INFO_POS, &info);
    Compute_ViewZ(&info);

    return iExit;
}

void CSoda::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CInteractObject::LateUpdate_GameObject(fTimeDelta);
}

void CSoda::Render_GameObject()
{
    CInteractObject::Render_GameObject();
}

HRESULT CSoda::Add_Component()
{
    Engine::CComponent* pComponent = nullptr;
    //Texutre - 자식 클래스에서 생성
    pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SodaTexture"));
    
    if (nullptr == pComponent)
        return E_FAIL;
    
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

void CSoda::Free()
{
    CInteractObject::Free();
}

void CSoda::Activate()
{
    CInteractObject::Activate();
    m_pTextureCom->Change_Texture(m_iTextureID);
}

void CSoda::Deactivate()
{
    CInteractObject::Deactivate();
}

void CSoda::OnCollision(CollisionInfo info)
{
    if (info.pTarget->GetOBJID() == OBJ_PLAYER)
    {
        static_cast<CPlayer*>(info.pTarget)->Add_Item(TAG_DRINK);
        m_bDead = true;
    }
}
