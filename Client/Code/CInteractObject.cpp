#include "pch.h"
#include "CInteractObject.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CInteractObject::CInteractObject(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
    , m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr), m_pCollisionCom(nullptr)
    , m_eItemType(ITEM_NONE), m_iTextureID(0)
{
}

CInteractObject::CInteractObject(const CInteractObject& rhs)
    : CGameObject(rhs)
    , m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr), m_pCollisionCom(nullptr)
    , m_eItemType(ITEM_NONE), m_iTextureID(0)
{
}

CInteractObject::~CInteractObject()
{
}

HRESULT CInteractObject::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;    

    return S_OK;
}

// 업데이트는 하위클래스에서 구현 추천
_int CInteractObject::Update_GameObject(const _float& fTimeDelta)
{
    if (IsDead())
        return RET_DEAD;
    int iExit = CGameObject::Update_GameObject(fTimeDelta);    

    return iExit;
}

void CInteractObject::LateUpdate_GameObject(const _float& fTimeDelta)
{
    SetBillBoard();
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CInteractObject::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pTextureCom->Render_Texture();
    m_pBufferCom->Render_Buffer();
}

HRESULT CInteractObject::Add_Component()
{
    Engine::CComponent* pComponent = nullptr;

    //VIBuffer
    pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    //Transform
    pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    //Texutre - 자식 클래스에서 생성
    //pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
    //    (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_DisplayTexture"));
    //
    //if (nullptr == pComponent)
    //    return E_FAIL;
    //
    //m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    // Collision
    pComponent = m_pCollisionCom = dynamic_cast<Engine::CCollision*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Collision"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collision", pComponent });

    return S_OK;
}

void CInteractObject::Free()
{
    CGameObject::Free();
}

void CInteractObject::SetPos(_vec3 _pos)
{
    if (!m_pTransformCom) return;
    m_pTransformCom->Set_Pos(_pos);
}

void CInteractObject::Rotate(ROTATION _Axis, _float _degree)
{
    if (!m_pTransformCom) return;
    m_pTransformCom->Rotation(_Axis, _degree);
}

void CInteractObject::SetScale(_vec3 _scale)
{
    if (!m_pTransformCom) return;
    m_pTransformCom->Set_Scale(_scale);
}

void CInteractObject::SetTexture(_uint iTextureID)
{
    m_iTextureID = iTextureID;
    if (!m_pTextureCom) return;
    m_pTextureCom->Change_Texture(iTextureID);
}

void CInteractObject::Activate()
{
    CGameObject::Activate();
}

void CInteractObject::Deactivate()
{
    CGameObject::Deactivate();
}

void CInteractObject::SetBillBoard()
{
    _matrix matView, matBill;

    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
    D3DXMatrixInverse(&matView, NULL, &matView);
    _vec3 camPos;
    memcpy(&camPos, &matView.m[3], sizeof(_vec3));
    _vec3 myPos = *m_pTransformCom->Get_Info(INFO_POS);
    _vec3 myScale = m_pTransformCom->m_vScale;

    //방향 주의! 카메라의 방향을 처다봐야 뒷면이 랜더링 됨 
    _vec3 look = myPos - camPos;
    look.y = 0.0f;
    D3DXVec3Normalize(&look, &look);

    _vec3 right;
    _vec3 up = { 0.0f, 1.0f, 0.0f };
    D3DXVec3Cross(&right, &up, &look);
    D3DXVec3Normalize(&right, &right);

    D3DXVec3Cross(&up, &look, &right);
    D3DXVec3Normalize(&up, &up);

    D3DXMatrixIdentity(&matBill);
    right *= myScale.x;
    up *= myScale.y;
    look *= myScale.z;

    memcpy(&matBill.m[0], &right, sizeof(_vec3));
    memcpy(&matBill.m[1], &up, sizeof(_vec3));
    memcpy(&matBill.m[2], &look, sizeof(_vec3));
    memcpy(&matBill.m[3], &myPos, sizeof(_vec3));
    m_pTransformCom->Set_World(&matBill);
}
