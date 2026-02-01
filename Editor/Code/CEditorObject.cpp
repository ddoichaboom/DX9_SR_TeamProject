#include "pch.h"
#include "CEditorObject.h"

#include "CTransform.h"
#include "CVIBuffer.h"
#include "CTexture.h"

CEditorObject::CEditorObject(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
    , m_pGraphicDev(pGraphicDev)
    , m_pTransformCom(nullptr)
    , m_pBufferCom(nullptr)
    , m_pTextureCom(nullptr)
    , m_bSelected(false)
    , m_wstrName(L"EditorObject")
    , m_eObjType(EDITOR_OBJ_END)
{
    m_pGraphicDev->AddRef();
    m_iRoomIndex = 0;
}

CEditorObject::~CEditorObject()
{
}

HRESULT CEditorObject::Ready_GameObject()
{
    return S_OK;
}

_int CEditorObject::Update_GameObject(const _float& fTimeDelta)
{
    // Transform 업데이트
    if (m_pTransformCom)
        Engine::CGameObject::Update_GameObject(fTimeDelta);

    return 0;
}

void CEditorObject::LateUpdate_GameObject(const _float& fTimeDelta)
{
    if (m_pTransformCom)
        Engine::CGameObject::LateUpdate_GameObject(fTimeDelta);
}

// Transform Setter
void CEditorObject::Set_Position(_vec3 vPos)
{
    if (nullptr == m_pTransformCom)
        return; 

    m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

}

void CEditorObject::Set_Rotation(_vec3 vRot)
{
    if (nullptr == m_pTransformCom)
        return;

    m_pTransformCom->Set_Angle(vRot.x, vRot.y, vRot.z);

}

void CEditorObject::Set_Scale(_vec3 vScale)
{
    if (nullptr == m_pTransformCom)
        return;

    m_pTransformCom->Set_Scale(vScale.x, vScale.y, vScale.z);

}

// Transform Getter
_vec3 CEditorObject::Get_Position() 
{
    if (m_pTransformCom)
    {
        _vec3 vPos;
        m_pTransformCom->Get_Info(INFO_POS, &vPos);
        
        return vPos;
    }

    return _vec3(0.f, 0.f, 0.f);
}

_vec3 CEditorObject::Get_Rotation()
{
    if (m_pTransformCom)
    {
        return m_pTransformCom->Get_Angle();
    }
    return _vec3(0.f, 0.f, 0.f);
}

_vec3 CEditorObject::Get_Scale()
{
    if (m_pTransformCom)
    {
        return m_pTransformCom->Get_Scale();
    }
    return _vec3(1.f, 1.f, 1.f);
}



const _matrix* CEditorObject::Get_WorldMatrix() const
{
    if (m_pTransformCom)
        return m_pTransformCom->Get_World();

    return nullptr;
}

void CEditorObject::Free()
{
    //Safe_Release(m_pBufferCom);
    //Safe_Release(m_pTransformCom);
    //Safe_Release(m_pGraphicDev);

    Engine::CGameObject::Free();
}