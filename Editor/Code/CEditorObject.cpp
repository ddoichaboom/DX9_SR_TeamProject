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
{
    m_pGraphicDev->AddRef();
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

_vec3 CEditorObject::Get_Min()
{
    if (!m_pTransformCom)
        return _vec3(0.f, 0.f, 0.f);

    // 1. 버퍼의 로컬 정점 ( 단위 크기: -1 ~ +1)
    //      버퍼는 -1 ~ +1 범위 이므로, AABB 계산을 위해 8개 정점 정의
    _vec3 vLocalCorners[8] =
    {
        _vec3(-1.f, -1.f, -1.f),
        _vec3(1.f, -1.f, -1.f),
        _vec3(-1.f, 1.f, -1.f),
        _vec3(1.f, 1.f, -1.f),
        _vec3(-1.f, -1.f, 1.f),
        _vec3(1.f, -1.f, 1.f),
        _vec3(-1.f, 1.f, 1.f),
        _vec3(1.f, 1.f, 1.f)
    };

    // 2. 월드 변환 행렬 가져오기
    const _matrix* pWorldMatrix = m_pTransformCom->Get_World();

    // 3. 모든 정점을 월드 공간으로 변환하고 Min 찾기
    _vec3 vMin(FLT_MAX, FLT_MAX, FLT_MAX);

    for (int i = 0; i < 8; ++i)
    {
        _vec3 vWorldCorner;
        D3DXVec3TransformCoord(&vWorldCorner, &vLocalCorners[i], pWorldMatrix);

        vMin.x = min(vMin.x, vWorldCorner.x);
        vMin.y = min(vMin.y, vWorldCorner.y);
        vMin.z = min(vMin.z, vWorldCorner.z);
    }

    return vMin;
}

//_vec3 CEditorObject::Get_Min()
//{
//    _vec3   vPos = Get_Position();
//    _vec3   vSacle = Get_Scale();
//
//    return vPos - vSacle * 0.5f;
//}

_vec3 CEditorObject::Get_Max()
{
    if (!m_pTransformCom)
        return _vec3(0.f, 0.f, 0.f);

    // 1. 버퍼의 로컬 정점 (단위 크기: -1 ~ +1)
    _vec3 vLocalCorners[8] =
    {
        _vec3(-1.f, -1.f, -1.f),
        _vec3(+1.f, -1.f, -1.f),
        _vec3(-1.f, +1.f, -1.f),
        _vec3(+1.f, +1.f, -1.f),
        _vec3(-1.f, -1.f, +1.f),
        _vec3(+1.f, -1.f, +1.f),
        _vec3(-1.f, +1.f, +1.f),
        _vec3(+1.f, +1.f, +1.f)
    };

    // 2. 월드 변환 행렬 가져오기 (이미 Scale 포함!)
    const _matrix* pWorldMatrix = m_pTransformCom->Get_World();

    // 3. 모든 정점을 월드 공간으로 변환하고 Max 찾기
    _vec3 vMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    for (int i = 0; i < 8; ++i)
    {
        _vec3 vWorldCorner;
        D3DXVec3TransformCoord(&vWorldCorner, &vLocalCorners[i], pWorldMatrix);

        vMax.x = max(vMax.x, vWorldCorner.x);
        vMax.y = max(vMax.y, vWorldCorner.y);
        vMax.z = max(vMax.z, vWorldCorner.z);
    }

    return vMax;
}

//_vec3 CEditorObject::Get_Max()
//{
//    _vec3   vPos = Get_Position();
//    _vec3   vSacle = Get_Scale();
//
//    return vPos + vSacle * 0.5f;
//}

// Transform Setter
void CEditorObject::Set_Position(_vec3 vPos)
{
    if (m_pTransformCom)
    {
        m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
    }
}

void CEditorObject::Set_Rotation(_vec3 vRot)
{
    if (m_pTransformCom)
    {
        m_pTransformCom->Set_Angle(vRot.x, vRot.y, vRot.z);
    }
}

void CEditorObject::Set_Scale(_vec3 vScale)
{
    if (m_pTransformCom)
    {
        m_pTransformCom->Set_Scale(vScale.x, vScale.y, vScale.z);
    }
}

// Transform Getter
_vec3 CEditorObject::Get_Position()
{
    if (m_pTransformCom)
    {
        return *m_pTransformCom->Get_Info(Engine::INFO_POS);
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

// 텍스처 설정 (Phase 5+)
void CEditorObject::Set_Texture(Engine::CTexture* pTexture)
{
    if (m_pTextureCom)
    {
        m_pTextureCom->Release();
        m_pTextureCom = nullptr;
    }

    m_pTextureCom = pTexture;

    if (m_pTextureCom)
        m_pTextureCom->AddRef();
}

void CEditorObject::Free()
{
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pBufferCom);
    Safe_Release(m_pTransformCom);
    Safe_Release(m_pGraphicDev);

    Engine::CGameObject::Free();
}