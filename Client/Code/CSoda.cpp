#include "pch.h"
#include "CSoda.h"
#include "CProtoMgr.h"
#include "CPoolMgr.h"
#include "CRenderer.h"
#include "CEventMgr.h"


#include "CManagement.h"
#include "CPlayer.h"
#include "CFloor.h"

TextureSource CSoda::m_vTextureSource =
{
    0, L"../Bin/Resource/Texture/Object/Soda.dds"
};


CSoda::CSoda(LPDIRECT3DDEVICE9 pGraphicDev)
    : CInteractObject(pGraphicDev), m_pMainCollider(nullptr)
    , m_bJump(false), m_bFall(false), m_fVelocity(0.f), m_bGround(false)
    , m_fJumpTime(0.f), m_fJumpDuration(0.4f), m_fJumpHeight(5.f)
{
    m_eItemType = ITEM_SODA;
    
}

CSoda::CSoda(const CSoda& rhs)
    : CInteractObject(rhs), m_pMainCollider(nullptr)
    , m_bJump(false), m_bFall(false), m_fVelocity(0.f), m_bGround(false)
    , m_fJumpTime(0.f), m_fJumpDuration(0.4f), m_fJumpHeight(5.f)
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

    if (m_bJump)
        Update_Jump(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    _vec3 info;
    m_pTransformCom->Get_Info(INFO_POS, &info);
    Compute_ViewZ(&info);

    return iExit;
}

void CSoda::LateUpdate_GameObject(const _float& fTimeDelta)
{   
    CInteractObject::LateUpdate_GameObject(fTimeDelta);

    if (m_bFall)    
        Gravity(fTimeDelta);    
    else    
        m_fVelocity = 0.f;
    
    if(m_bGround == false)
        Set_OnFloor(fTimeDelta);
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
    m_bJump = true;
    m_bFall = false;
    m_bGround = false;
    m_fVelocity = 0.f;
    m_fJumpTime = 0.f;    
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

void CSoda::Gravity(const _float& fTimeDelta)
{
    _float fVelocity = m_fVelocity;
    fVelocity -= 9.81f * (fTimeDelta + 0.25f);
    m_fVelocity = fVelocity;
}

_bool CSoda::CheckOnFloor(const _float& fTimeDelta, _float* pHeight)
{
    CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"Environment_Layer");
    if (!pLayer) return false;

    _vec3 vPosition = m_pTransformCom->m_vInfo[INFO_POS];
    auto pairIter = pLayer->Get_Objects(OBJ_FLOOR);

    _float	fMaxY = -FLT_MAX;
    _bool	bFound = false;

    for (auto iter = pairIter.first; iter != pairIter.second; iter++)
    {
        CTransform* pTransform = static_cast<CTransform*>(iter->second->Get_Component(ID_STATIC, L"Com_Transform"));
        CGameObject* pGameObject = iter->second;        
        _float fCurrentFloorY = 0.f;


        if (pTransform->Check_OnRange(&vPosition, &fCurrentFloorY))
        {
            if (fCurrentFloorY <= vPosition.y && fCurrentFloorY > fMaxY)
            {
                fMaxY = fCurrentFloorY;
                bFound = true;
            }
        }
    }

    if (bFound)
    {        
        *pHeight = fMaxY;
        return true;
    }

    return false;
}

void CSoda::Set_OnFloor(const _float& fTimeDelta)
{    
    _vec3   vPosition = *m_pTransformCom->Get_Info(INFO_POS);
    _float  fHeight = 0.f;
    _float  fBottom = vPosition.y - m_pMainCollider->Get_Scale().y * 0.5f;

    if (CheckOnFloor(fTimeDelta, &fHeight))
    {
        if (m_bJump)
        {
            return;
        }
        else if (m_bFall)
        {
            vPosition.y += m_fVelocity * fTimeDelta;
            fBottom = vPosition.y - m_pMainCollider->Get_Scale().y * 0.5f;
            if (fBottom <= fHeight)
            {
                m_bFall = false;
                m_bGround = true;
                vPosition.y = fHeight + m_pMainCollider->Get_Scale().y * 0.5f;
            }
        }
        else
        {
            fBottom = vPosition.y - m_pMainCollider->Get_Scale().y * 0.5f;
            if (fBottom > fHeight)
            {
                m_bFall = true;
                vPosition.y += m_fVelocity * fTimeDelta;
            }
            else
            {
                m_bFall = false;
                vPosition.y = fHeight +  m_pMainCollider->Get_Scale().y * 0.5f;                
            }
        }
    }
    else
    {
        vPosition.y += m_fVelocity * fTimeDelta;
        m_bFall;
    }

    m_pTransformCom->Set_Pos(vPosition);
}

void CSoda::Update_Jump(const _float& fTimeDelta)
{
    m_fJumpTime += fTimeDelta;
    float t = m_fJumpTime / m_fJumpDuration;

    if (t >= 1.f)
    {
        t = 1.f;
        m_bJump = false;
        m_bFall = true;
    }

    float easeOutQuad = 1.f - (1.f - t) * (1.f - t);

    _vec3 vPosition = m_vJumpStartPos + (m_vJumpDir * t);
    vPosition.y += (easeOutQuad * m_fJumpHeight);

    SetPos(vPosition);
}

void CSoda::Set_JumpDir()
{
    m_vJumpDir = { 0.f, 0.f, 0.f };

    CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");
    if (!pLayer)
        return;
            
    _vec3 vPlayerPos = { 0.f,0.f,0.f };
    _vec3 vMyPos = *m_pTransformCom->Get_Info(INFO_POS);
    m_vJumpStartPos = vMyPos;
    
    CGameObject* pPlayerObj = pLayer->Get_Object(OBJ_PLAYER);

    if (nullptr == pPlayerObj)
        return;

    CTransform* pTransform = dynamic_cast<CTransform*>(pPlayerObj->Get_Component(ID_DYNAMIC, L"Com_Transform"));

    if (nullptr == pTransform)
        return;

    pTransform->Get_Info(INFO_POS, &vPlayerPos);

    m_vJumpDir = vPlayerPos - vMyPos;
    D3DXVec3Normalize(&m_vJumpDir, &m_vJumpDir);
    m_vJumpDir *= 15.f;

}
