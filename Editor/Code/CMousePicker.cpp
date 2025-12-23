#include "pch.h"
#include "CMousePicker.h"

CMousePicker::CMousePicker()
    : m_vRayPos(0.f, 0.f, 0.f)
    , m_vRayDir(0.f, 0.f, 1.f)
{
}

CMousePicker::~CMousePicker()
{
}

void CMousePicker::Update(const _matrix& matView, const _matrix& matProj, POINT ptMouse)
{
    Screen_To_WorldRay(matView, matProj, ptMouse);
}

void CMousePicker::Screen_To_WorldRay(const _matrix& matView, const _matrix& matProj, POINT ptMouse)
{
    // 1. 스크린 좌표를 NDC ( Normalized Device Coordinates )로 변환 ( -1 ~ +1)
    _float fNdcX = ((_float)ptMouse.x / (WINCX * 0.5f)) - 1.f;
    _float fNdcY = -((_float)ptMouse.y / (WINCY * 0.5f)) + 1.f;  // Y축 반전

    // 2. View * Projection 역행렬 계산
    _matrix matViewProj = matView * matProj;
    _matrix matInvViewProj;
    D3DXMatrixInverse(&matInvViewProj, nullptr, &matViewProj);

    // 3. Near Plane과 Far Plane 상의 점 생성
    _vec3 vNear(fNdcX, fNdcY, 0.f);  // Near Plane (Z=0)
    _vec3 vFar(fNdcX, fNdcY, 1.f);   // Far Plane (Z=1)

    // 4. 월드 좌표로 변환
    _vec3 vWorldNear, vWorldFar;
    D3DXVec3TransformCoord(&vWorldNear, &vNear, &matInvViewProj);
    D3DXVec3TransformCoord(&vWorldFar, &vFar, &matInvViewProj);

    // 5. Ray 시작점과 방향 계산
    m_vRayPos = vWorldNear;
    m_vRayDir = vWorldFar - vWorldNear;
    D3DXVec3Normalize(&m_vRayDir, &m_vRayDir);
}

CMousePicker* CMousePicker::Create()
{
    CMousePicker* pInstance = new CMousePicker;

    // Ready 함수가 없으므로 바로 반환
    return pInstance;
}

void    CMousePicker::Free()
{

}