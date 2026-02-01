#include "pch.h"
#include "CDisplayObject.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

vector<TextureSource> CDisplayObject::m_vTextureSource =
{
    {JUMP_BORDER, L"../Bin/Resource/Texture/Object/DisplayObject/JUMP_BORDER.dds"},
    {CABLES, L"../Bin/Resource/Texture/Object/DisplayObject/CABLES.dds", true, 0, 3, 3, {1.f, 1.f}},
    {PALMS, L"../Bin/Resource/Texture/Object/DisplayObject/PALMS.dds"},
    {PASSARELA, L"../Bin/Resource/Texture/Object/DisplayObject/PASSARELA.dds"},
    {RUG, L"../Bin/Resource/Texture/Object/DisplayObject/RUG.dds", true, 0, 3, 3, {1.f, 1.f}},
    {SIGNS, L"../Bin/Resource/Texture/Object/DisplayObject/SIGNS.dds", true, 0, 6, 6, {1.f, 1.f}},
    {PLATE, L"../Bin/Resource/Texture/Object/DisplayObject/PLATE.dds"},
    {ROAD_CORNER, L"../Bin/Resource/Texture/Object/DisplayObject/ROAD_CORNER.dds"},
    {ROAD_PLATE, L"../Bin/Resource/Texture/Object/DisplayObject/ROAD_PLATE.dds", true, 0, 3, 3, {1.f, 1.f}},
    {TRAFFIC_LIGHTS, L"../Bin/Resource/Texture/Object/DisplayObject/TRAFFIC_LIGHTS.dds"},
    {TRAFFIC_SIGN_1, L"../Bin/Resource/Texture/Object/DisplayObject/TRAFFIC_SIGN_1.dds"},
    {TRAFFIC_SIGN_2, L"../Bin/Resource/Texture/Object/DisplayObject/TRAFFIC_SIGN_2.dds"},
    {TRAFFIC_SIGN_3, L"../Bin/Resource/Texture/Object/DisplayObject/TRAFFIC_SIGN_3.dds"},
    {WALL_WINDOW, L"../Bin/Resource/Texture/Object/DisplayObject/WALL_WINDOW.dds", true, 0, 1, 1, {1.f, 1.f}},
    {PASSTAIRS, L"../Bin/Resource/Texture/Object/DisplayObject/PASSTAIRS.dds"},
    {STREET_LIGHTS, L"../Bin/Resource/Texture/Object/DisplayObject/STREET_LIGHTS.dds"},
    {SCREEN_DISPLAY, L"../Bin/Resource/Texture/Object/DisplayObject/SCREEN_DISPLAY.dds", true, 0, 7, 7, {1.f, 1.f}},
    {NAKAMURA_TEXT, L"../Bin/Resource/Texture/Object/DisplayObject/NAKAMURA_PLAZA.dds"},
    {DOOR, L"../Bin/Resource/Texture/Object/DisplayObject/DOOR_1.dds"}
};

CDisplayObject::CDisplayObject(LPDIRECT3DDEVICE9 pGraphicDev)
    :   CGameObject(pGraphicDev)
    ,   m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
    ,   m_eObjectType(DOT_END), m_iTextureID(0)
{
    m_eOBJ_ID = OBJ_DISPLAY;
}

CDisplayObject::CDisplayObject(const CDisplayObject& rhs)
    :   CGameObject(rhs)
    ,   m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
    ,   m_eObjectType(DOT_END), m_iTextureID(0)
{
    m_eOBJ_ID = rhs.m_eOBJ_ID;
}

CDisplayObject::~CDisplayObject()
{
}

CDisplayObject* CDisplayObject::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CDisplayObject* pDisplay = new CDisplayObject(pGraphicDev);

    if (FAILED(pDisplay->Ready_GameObject()))
    {
        Safe_Release(pDisplay);
        MSG_BOX("Display Object Create Failed");
        return nullptr;
    }

    return pDisplay;
}

HRESULT CDisplayObject::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

_int CDisplayObject::Update_GameObject(const _float& fTimeDelta)
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

void CDisplayObject::LateUpdate_GameObject(const _float& fTimeDelta)
{
    //SetBillBoard();
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CDisplayObject::Render_GameObject()
{
    DWORD dOldCullMode;
    m_pGraphicDev->GetRenderState(D3DRS_CULLMODE, &dOldCullMode);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    if (m_pTextureCom)
        m_pTextureCom->Render_Texture();

    if (m_pBufferCom)
    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, dOldCullMode);
    m_pGraphicDev->SetTexture(0, nullptr);
}

HRESULT CDisplayObject::Add_Component()
{
    Engine::CComponent* pComponent = nullptr;

    //VIBuffer
    pComponent = m_pBufferCom = static_cast<Engine::CRcTex*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    //Transform
    pComponent = m_pTransformCom = static_cast<Engine::CTransform*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Transform", pComponent });

    //Texutre
    pComponent = m_pTextureCom = static_cast<Engine::CTexture*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_DisplayTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

void CDisplayObject::Free()
{
    CGameObject::Free();
}

_vec3 CDisplayObject::GetPos()
{
    if (!m_pTransformCom) return _vec3();
    return *m_pTransformCom->Get_Info(INFO_POS);
}


void CDisplayObject::SetPos(_vec3 _pos)
{
    if (!m_pTransformCom) return;
    m_pTransformCom->Set_Pos(_pos);
}

void CDisplayObject::Rotate(ROTATION _Axis, _float _degree)
{
    if (!m_pTransformCom) return;
    m_pTransformCom->Rotation(_Axis, _degree);
}

void CDisplayObject::SetScale(_vec3 _scale)
{
    if (!m_pTransformCom) return;
    m_pTransformCom->Set_Scale(_scale);
}

void CDisplayObject::SetTexture(_uint iTextureID)
{
    m_iTextureID = iTextureID;
    if (!m_pTextureCom) return;
    m_pTextureCom->Change_Texture(iTextureID);   
}

void CDisplayObject::SetTransformMatrix()
{
    if (!m_pTransformCom) return;
    m_pTransformCom->Update_Component(0.f);
}

void CDisplayObject::Set_Angle(const _vec3& vRot)
{
    if (!m_pTransformCom) return;
    m_pTransformCom->Set_Angle(vRot);
}

void CDisplayObject::Set_DisplayObjectType(DISPLAY_OBJECT_TYPE eObjectType, _uint iTextureId)
{
    m_eObjectType = eObjectType;

    if (m_pTextureCom)
    {
        m_pTextureCom->Change_Texture(static_cast<_int>(m_eObjectType));
        m_pTextureCom->Set_Frame(_vec2(iTextureId, 0));
    }
}

void CDisplayObject::Activate()
{
    CGameObject::Activate();
}

void CDisplayObject::Deactivate()
{
    CGameObject::Deactivate();
}

void CDisplayObject::SetBillBoard()
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
