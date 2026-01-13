#include "CTransform.h"

CTransform::CTransform()
    : m_vScale(1.f, 1.f, 1.f), m_vAngle(0.f, 0.f, 0.f)
{
    ZeroMemory(m_vInfo, sizeof(_vec3) * INFO_END);
    D3DXMatrixIdentity(&m_matWorld);
}

CTransform::CTransform(LPDIRECT3DDEVICE9 pGraphicDev)
    : CComponent(pGraphicDev)
    , m_vScale(1.f, 1.f, 1.f), m_vAngle(0.f, 0.f, 0.f)
{
    ZeroMemory(m_vInfo, sizeof(_vec3) * INFO_END);
    D3DXMatrixIdentity(&m_matWorld);
}

CTransform::CTransform(const CTransform& rhs)
    :CComponent(rhs), m_vScale(rhs.m_vScale), m_vAngle(rhs.m_vAngle)
{
    for (_uint i = 0; i < INFO_END; ++i)
    {
        m_vInfo[i] = rhs.m_vInfo[i];
    }
    m_matWorld = rhs.m_matWorld;
}

CTransform::~CTransform()
{
}

HRESULT CTransform::Ready_Transform()
{
    D3DXMatrixIdentity(&m_matWorld);

    for (_uint i = 0; i < INFO_END; ++i)
    {
        memcpy(&m_vInfo[i], &m_matWorld.m[i][0], sizeof(_vec3));
    }

    return S_OK;
}

_int CTransform::Update_Component(const _float& fTimeDelta)
{
    D3DXMatrixIdentity(&m_matWorld);

    // 크기 변환

    for (_uint i = 0; i < INFO_POS; ++i)
    {
        memcpy(&m_vInfo[i], &m_matWorld.m[i][0], sizeof(_vec3));
    }

    for (_uint i = 0; i < INFO_POS; ++i)
    {
        D3DXVec3Normalize(&m_vInfo[i], &m_vInfo[i]);

        m_vInfo[i] *= *((_float*)&m_vScale + i);
    }

    // 회전 변환

    _matrix matRot[ROT_END];

    D3DXMatrixRotationX(&matRot[ROT_X], D3DXToRadian(m_vAngle.x));
    D3DXMatrixRotationY(&matRot[ROT_Y], D3DXToRadian(m_vAngle.y));
    D3DXMatrixRotationZ(&matRot[ROT_Z], D3DXToRadian(m_vAngle.z));

    for (_uint i = 0; i < INFO_POS; ++i)
    {
        for (_uint j = 0; j < ROT_END; ++j)
        {
            D3DXVec3TransformNormal(&m_vInfo[i], &m_vInfo[i], &matRot[j]);
        }
    }

    //  월드 행렬 구성

    for (_uint i = 0; i < INFO_END; ++i)
    {
        memcpy(&m_matWorld.m[i][0], &m_vInfo[i], sizeof(_vec3));
    }


    return 0;
}

void CTransform::LateUpdate_Component()
{

}

void CTransform::Chase_Target(const _vec3* pTargetPos, const _float& fTimeDelta, const _float& fSpeed)
{
    _vec3   vDir = *pTargetPos - m_vInfo[INFO_POS];

    m_vInfo[INFO_POS] += *D3DXVec3Normalize(&vDir, &vDir) * fTimeDelta * fSpeed;

    _matrix matScale, matRot, matTrans;

    D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
    D3DXMatrixTranslation(&matTrans,
        m_vInfo[INFO_POS].x,
        m_vInfo[INFO_POS].y,
        m_vInfo[INFO_POS].z);

    matRot = *Compute_LookAtTarget(pTargetPos);


    m_matWorld = matScale * matRot * matTrans;

}

_matrix* CTransform::Compute_LookAtTarget(const _vec3* pTargetPos)
{
    _vec3   vDir = *pTargetPos - m_vInfo[INFO_POS];

    _matrix matRot;
    _vec3   vUp,vAxis;

    //D3DXVec3Cross(&vAxis, &m_vInfo[INFO_UP], &vDir);
    //
    //D3DXVec3Normalize(&vUp, &m_vInfo[INFO_UP]);
    //D3DXVec3Normalize(&vDir, &vDir);
    //
    //float fDot = D3DXVec3Dot(&vUp, &vDir);
    //float fAngle = acosf(fDot);
    //
    //D3DXMatrixRotationAxis(&matRot, &vAxis, fAngle);
    //
    //return &matRot;


    return D3DXMatrixRotationAxis(&matRot, 
        D3DXVec3Cross(&vAxis, &m_vInfo[INFO_UP], &vDir),
        acosf(D3DXVec3Dot(D3DXVec3Normalize(&vUp, &m_vInfo[INFO_UP]), 
                          D3DXVec3Normalize(&vDir, &vDir))));
}

_vec3 CTransform::Get_Info_World(INFO info) const
{
    _vec3 vInfo;
    memcpy(&vInfo, &m_matWorld.m[info][0], sizeof(_vec3));
    return vInfo;
}

void CTransform::Set_Info_World(INFO info, _vec3* pVector)
{
    memcpy(&m_matWorld.m[info][0], pVector, sizeof(_vec3));
}

_vec3 CTransform::Get_Scale_World() const
{
    _vec3 vRight, vUp, vLook;
    _float fX, fY, fZ;
    memcpy(&vRight, &m_matWorld.m[0][0], sizeof(_vec3));
    memcpy(&vUp, &m_matWorld.m[1][0], sizeof(_vec3));
    memcpy(&vLook, &m_matWorld.m[2][0], sizeof(_vec3));

    fX = D3DXVec3Length(&vRight);
    fY = D3DXVec3Length(&vUp);
    fZ = D3DXVec3Length(&vLook);

    return _vec3(fX, fY, fZ);
}

void CTransform::Set_Scale_World(_float fX, _float fY, _float fZ)
{
    _vec3 vRight, vUp, vLook;    

    memcpy(&vRight, &m_matWorld.m[0][0], sizeof(_vec3));
    memcpy(&vUp, &m_matWorld.m[1][0], sizeof(_vec3));
    memcpy(&vLook, &m_matWorld.m[2][0], sizeof(_vec3));
    vRight = *D3DXVec3Normalize(&vRight, &vRight) * fX;
    vUp = *D3DXVec3Normalize(&vUp, &vUp) * fY;
    vLook = *D3DXVec3Normalize(&vLook, &vLook) * fZ;

    memcpy(&m_matWorld.m[0][0], &vRight,sizeof(_vec3));
    memcpy(&m_matWorld.m[1][0], &vUp,sizeof(_vec3));
    memcpy(&m_matWorld.m[2][0], &vLook,sizeof(_vec3));    
}

_bool CTransform::Check_OnRange(_vec3* pPosition, _float* pHeight)
{    
    _float fHalfX = m_vScale.x;
    _float fHalfZ = m_vScale.y;
    _float fMinX = m_vInfo[INFO_POS].x - fHalfX;
    _float fMaxX = m_vInfo[INFO_POS].x + fHalfX;
    _float fMinZ = m_vInfo[INFO_POS].z - fHalfZ;    
    _float fMaxZ = m_vInfo[INFO_POS].z + fHalfZ;

    if (pPosition->x <= fMaxX && pPosition->x >= fMinX && pPosition->z <= fMaxZ && pPosition->z >= fMinZ)
    {    
        if (pPosition->y >= m_vInfo[INFO_POS].y)
        {
            *pHeight = m_vInfo[INFO_POS].y;
            return true;
        }        
    }
    return false;
}

void CTransform::Reset()
{
    D3DXMatrixIdentity(&m_matWorld);
    for (_uint i = 0; i < INFO_END; ++i)
    {
        memcpy(&m_vInfo[i], &m_matWorld.m[i][0], sizeof(_vec3));
    }
    m_vAngle = { 0.f, 0.f, 0.f };
}

CTransform* CTransform::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CTransform* pTransform = new CTransform(pGraphicDev);

    if (FAILED(pTransform->Ready_Transform()))
    {
        Safe_Release(pTransform);
        MSG_BOX("pTransform Create Failed");
        return nullptr;
    }

    return pTransform;
}

CComponent* CTransform::Clone()
{
    return new CTransform(*this);
}

void CTransform::Free()
{
    CComponent::Free();
}

