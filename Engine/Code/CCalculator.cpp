#include "CCalculator.h"
#include "CCollider.h"

CCalculator::CCalculator(LPDIRECT3DDEVICE9 pGraphicDev)
    : CComponent(pGraphicDev)
{
}

CCalculator::CCalculator(const CCalculator& rhs)
    : CComponent(rhs)
{
}

CCalculator::~CCalculator()
{

}

void CCalculator::GetRay(HWND hWnd, _vec3* pOutRayWorldPos, _vec3* pOutRayWorldDir)
{
    POINT       ptMouse{};
    GetCursorPos(&ptMouse);
    ScreenToClient(hWnd, &ptMouse);
    _vec3   vMousePos;

    D3DVIEWPORT9            ViewPort;
    ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));

    m_pGraphicDev->GetViewport(&ViewPort);

    vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
    vMousePos.y = ptMouse.y / -(ViewPort.Height * 0.5f) + 1.f;
    vMousePos.z = 0.f;

    D3DXMATRIX      matProj;
    m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
    D3DXMatrixInverse(&matProj, 0, &matProj);
    D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

    D3DXMATRIX      matView;
    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
    D3DXMatrixInverse(&matView, 0, &matView);

    _vec3       vRayPos{ 0.f, 0.f,0.f };
    _vec3       vRayDir = vMousePos - vRayPos;

    //월드 스페이스로 변환 
    D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);
    D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);
    D3DXVec3Normalize(&vRayDir, &vRayDir);

    *pOutRayWorldPos = vRayPos;
    *pOutRayWorldDir = vRayDir;
}

bool CCalculator::Check_PickedCollider(HWND hWnd, CCollider* pCollider)
{
    const float eps = 1e-6f;
    _vec3 vRayPos, vRayDir;
    GetRay(hWnd, &vRayPos, &vRayDir);

    _matrix mat = pCollider->GetWorldMatrix();

    D3DXMatrixInverse(&mat, NULL, &mat);
    D3DXVec3TransformCoord(&vRayPos, &vRayPos, &mat);
    D3DXVec3TransformNormal(&vRayDir, &vRayDir, &mat);
    D3DXVec3Normalize(&vRayDir, &vRayDir);

    _vec3 minPoint = { -1.f, -1.f, -1.f };
    _vec3 maxPoint = { 1.f, 1.f, 1.f };

    float rMin = -FLT_MIN, rMax = FLT_MAX;
    float tMinx = -FLT_MIN, tMaxx = FLT_MAX,
        tMiny = -FLT_MIN, tMaxy = FLT_MAX, tMinz = -FLT_MIN, tMaxz = FLT_MAX;


    if (fabsf(vRayDir.x) >= eps)
    {
        tMinx = (minPoint.x - vRayPos.x) / vRayDir.x;
        tMaxx = (maxPoint.x - vRayPos.x) / vRayDir.x;

        if (tMinx > tMaxx) swap(tMinx, tMaxx);
    }
    else
    {
        if (vRayPos.x < minPoint.x || vRayPos.x > maxPoint.x) return false;
    }

    if (fabsf(vRayDir.y) >= eps)
    {
        tMiny = (minPoint.y - vRayPos.y) / vRayDir.y;
        tMaxy = (maxPoint.y - vRayPos.y) / vRayDir.y;

        if (tMiny > tMaxy) swap(tMiny, tMaxy);
    }
    else
    {
        if (vRayPos.y < minPoint.y || vRayPos.y > maxPoint.y) return false;
    }

    if (fabsf(vRayDir.z) >= eps)
    {
        tMinz = (minPoint.z - vRayPos.z) / vRayDir.z;
        tMaxz = (maxPoint.z - vRayPos.z) / vRayDir.z;

        if (tMinz > tMaxz) swap(tMinz, tMaxz);
    }
    else
    {
        if (vRayPos.z < minPoint.z || vRayPos.z > maxPoint.z)
            return false;
    }

    rMin = max(tMinx, max(tMiny, tMinz));
    rMax = min(tMaxx, min(tMaxy, tMaxz));

    return rMin <= rMax;
}


CCalculator* CCalculator::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CCalculator* pCalculator = new CCalculator(pGraphicDev);
    return pCalculator;
}

CComponent* CCalculator::Clone()
{
    return new CCalculator(*this);
}

void CCalculator::Free()
{
    CComponent::Free();
}
