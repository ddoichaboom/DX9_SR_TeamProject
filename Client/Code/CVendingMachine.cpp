#include "pch.h"
#include "CVendingMachine.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "Engine_Enum.h"
#include "CSoda.h"
#include "CPoolMgr.h"
#include "CManagement.h"
#include "CSoundMgr.h"


vector<TextureSource> CVendingMachine::m_vTextureSource =
{
    {0, L"../Bin/Resource/Texture/Terrain/Object/SODAMACHINE.dds", false, 0, 0, 0, {1.f, 1.f}},
    {1, L"../Bin/Resource/Texture/Terrain/Object/SODAMACHINE_BROKEN.dds", false, 0, 0, 0, {1.f, 1.f}}
};

wstring CVendingMachine::szSodaMakeSFX = L"Soda_Make_SFX.wav";

CVendingMachine::CVendingMachine(LPDIRECT3DDEVICE9 pGraphicDev)
    :CGameObject(pGraphicDev)
    , m_pBufferCom(nullptr)
    , m_pTransformCom(nullptr)
    , m_pTextureCom(nullptr)
    , m_pCollisionCom(nullptr)
    , m_pCollider(nullptr)
    , m_pSoda(nullptr)
    , m_vColliderScale(1.f, 1.f, 1.f)
    , m_bDispens(false)
    , m_fTime(0.f)
    , m_iCount(0)
{
    m_eOBJ_ID = OBJ_VENDINGMACHINE;
    m_iID = Make_ID();
}

CVendingMachine::CVendingMachine(const CVendingMachine& rhs)
    :CGameObject(rhs)
    , m_pBufferCom(rhs.m_pBufferCom)
    , m_pTransformCom(rhs.m_pTransformCom)
    , m_pTextureCom(rhs.m_pTextureCom)
    , m_pCollisionCom(rhs.m_pCollisionCom)
    , m_pCollider(rhs.m_pCollider)
    , m_pSoda(rhs.m_pSoda)
    , m_bDispens(false)
    , m_fTime(0.f)
    , m_iCount(0)
{
    m_eOBJ_ID = rhs.m_eOBJ_ID;
    m_iID = Make_ID();
}

CVendingMachine::~CVendingMachine()
{

}

HRESULT CVendingMachine::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (m_pTextureCom)
    {
        m_pTextureCom->Change_Texture(0);       // 0번 -> 1번 ( 배출 시 변경 )
    }

    if (!m_pTransformCom)
        return E_FAIL;

    m_vColliderScale = m_pTransformCom->Get_Scale();

    if (!m_pCollisionCom)
        return E_FAIL;

    m_pCollider = m_pCollisionCom->CreateCollider(this, m_szColliderName);

    if (!m_pCollider)
        return E_FAIL;

    m_pCollider->Set_Scale(m_vColliderScale);



    // 충돌 콜백 함수 바인딩
    m_pCollider->BindFuncToCollision([this](CollisionInfo info)
        {
            this->OnCollision(info);
        });

    return S_OK;
}

_int CVendingMachine::Update_GameObject(const _float& fTimeDelta)
{
    if (IsDead())
        return RET_DEAD;

    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    if (m_bDispens && m_iCount)
    {
        m_fTime += fTimeDelta;

        if (m_fTime > 0.02f)
        {
            CreateSoda();
            m_fTime = 0.f;
            m_iCount--;
            if (m_iCount <= 0)
                m_pTextureCom->Change_Texture(1);
        }
    }
    

    return iExit;
}

void CVendingMachine::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);

    m_pCollisionCom->LateUpdate_Component();
}

void CVendingMachine::Render_GameObject()
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

void CVendingMachine::Dispense()
{
    // TODO 소다 배출로직     
    m_vDispensPos = *m_pTransformCom->Get_Info(INFO_POS);
    m_fTime = 0.f;
    m_iCount = 5;
    m_bDispens = true;      
}

void CVendingMachine::CreateSoda()
{
    CSoda* pSoda = CPoolMgr::GetInstance()->Get_Object<CSoda>();
    if (!pSoda)
        return;
    m_vDispensPos.y += 2.5f;
    pSoda->SetPos(m_vDispensPos);
    pSoda->Set_JumpDir();

    CLayer* layer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");
    if (!layer) pSoda->ReturnToPool();
    else layer->Add_GameObject(pSoda);
    CSoundMgr::GetInstance()->PlaySFXSound(szSodaMakeSFX.c_str(), 0.8f);
}

HRESULT CVendingMachine::Add_Component()
{
    Engine::CComponent* pComponent = nullptr;

    // CubeTex
    pComponent = m_pBufferCom = static_cast<Engine::CCubeTex*>
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
    pComponent = m_pTextureCom = static_cast<Engine::CCubeTexture*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_VendingMachine_Texture"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    // Collision 추가 
    pComponent = m_pCollisionCom = static_cast<Engine::CCollision*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collision"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collision", pComponent });
    return S_OK;
}

void	CVendingMachine::OnCollision(CollisionInfo info)
{
    // TODO: 조건 작성 
    
    if(!m_bDispens)
        Dispense();
}

void    CVendingMachine::Set_ColliderScale(_vec3 _scale)
{
    m_vColliderScale = _scale;

    if (m_pCollider)
    {
        m_pCollider->Set_Scale(m_vColliderScale);
    }
}

CVendingMachine* CVendingMachine::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CVendingMachine* pInstance = new CVendingMachine(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CVendingMachine Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CVendingMachine::Free()
{
    CGameObject::Free();
}

void CVendingMachine::Activate()
{
    CGameObject::Activate();
    m_bDispens = false;
    m_fTime = 0.f;
    m_iCount = 0;

    m_pTextureCom->Change_Texture(0);
}

void CVendingMachine::Deactivate()
{
    CGameObject::Deactivate();
}

void	CVendingMachine::SetPos(_vec3 _pos)
{
    if (m_pTransformCom)
    {
        m_pTransformCom->Set_Pos(_pos);
    }
}

void	CVendingMachine::SetAngle(_vec3 _rot)
{
    if (m_pTransformCom)
    {
        m_pTransformCom->Set_Angle(_rot);
    }
}

void	CVendingMachine::SetScale(_vec3 _scale)
{
    if (m_pTransformCom)
    {
        m_pTransformCom->Set_Scale(_scale);
    }
}
