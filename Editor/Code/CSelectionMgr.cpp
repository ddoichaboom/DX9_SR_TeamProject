#include "pch.h"
#include "CSelectionMgr.h"
#include "CEditorObject.h"

CSelectionMgr::CSelectionMgr()
    : m_pSelectedObject(nullptr)
    , m_ptLastMouse{ 0, 0 }
    , m_iCycleIndex(0)
{
}

CSelectionMgr::~CSelectionMgr()
{
}

void CSelectionMgr::Set_Selection(CEditorObject* pObj)
{
    // 기존 선택 해제 
    if (m_pSelectedObject)
        m_pSelectedObject->Set_Selected(false);

    m_pSelectedObject = pObj;

    // 새로운 선택 적용
    if (m_pSelectedObject)
        m_pSelectedObject->Set_Selected(true);
}

CEditorObject* CSelectionMgr::Pick_Object(const _vec3& vRayPos, const _vec3& vRayDir, list<CEditorObject*>& objectList)
{
    CEditorObject* pPickedObject = nullptr;
    _float fMinDist = FLT_MAX;

    for (auto& pObj : objectList)
    {
        // 오브젝트의 월드 행렬 가져오기
        const _matrix* pWorldMatrix = pObj->Get_WorldMatrix();

        if (!pWorldMatrix)
            continue;
        
        // 월드 행렬의 역행렬 계산
        _matrix matInvWorld;
        D3DXMatrixInverse(&matInvWorld, nullptr, pWorldMatrix);

        // World Ray를 Local Ray로 변환
        _vec3 vLocalRayPos, vLocalRayDir;
        D3DXVec3TransformCoord(&vLocalRayPos, &vRayPos, &matInvWorld);
        D3DXVec3TransformNormal(&vLocalRayDir, &vRayDir, &matInvWorld);
        D3DXVec3Normalize(&vLocalRayDir, &vLocalRayDir);

        // 로컬 AABB (고정 값)
        _vec3 vLocalMin(-1.f, -1.f, -1.f);
        _vec3 vLocalMax(1.f, 1.f, 1.f);

        _float fDist = 0.f;
        if (Intersect_RayAABB(vLocalRayPos, vLocalRayDir, vLocalMin, vLocalMax, &fDist))
        {
            if (fDist < fMinDist)
            {
                fMinDist = fDist;
                pPickedObject = pObj;
            }
        }

    }

    return pPickedObject;
}

void CSelectionMgr::Pick_Objects_All(const _vec3& vRayPos,
    const _vec3& vRayDir,
    const list<CEditorObject*>& ObjectList,
    list<CEditorObject*>& outPickedList)
{
    outPickedList.clear();

    // 거리순으로 정렬하기 위한 임시 벡터
    vector<pair<_float, CEditorObject*>> distObjPairs;

    for (auto& pObj : ObjectList)
    {
        // 오브젝트의 월드 행렬 가져오기
        const _matrix* pWorldMatrix = pObj->Get_WorldMatrix();

        if (!pWorldMatrix)
            continue;

        // 월드 행렬의 역행렬 계산
        _matrix matInvWorld;
        D3DXMatrixInverse(&matInvWorld, nullptr, pWorldMatrix);

        // World Ray를 Local Ray로 변환
        _vec3 vLocalRayPos, vLocalRayDir;
        D3DXVec3TransformCoord(&vLocalRayPos, &vRayPos, &matInvWorld);
        D3DXVec3TransformNormal(&vLocalRayDir, &vRayDir, &matInvWorld);
        D3DXVec3Normalize(&vLocalRayDir, &vLocalRayDir);

        // 로컬 AABB (고정값)
        _vec3 vLocalMin(-1.f, -1.f, -1.f);
        _vec3 vLocalMax(1.f, 1.f, 1.f);

        _float fDist = 0.f;
        if (Intersect_RayAABB(vLocalRayPos, vLocalRayDir, vLocalMin, vLocalMax, &fDist))
        {
            distObjPairs.push_back(make_pair(fDist, pObj));
        }
    }

    // 거리 순 정렬 (가까운 순서)
    sort(distObjPairs.begin(), distObjPairs.end(),
        [](const pair<_float, CEditorObject*>& a, const pair<_float, CEditorObject*>& b)
        {
            return a.first < b.first;
        });

    // list로 변환
    for (auto& pair : distObjPairs)
    {
        outPickedList.push_back(pair.second);
    }
}

CEditorObject* CSelectionMgr::Pick_Object_Cycle(const _vec3& vRayPos,
    const _vec3& vRayDir,
    const list<CEditorObject*>& ObjectList,
    const POINT& ptMouse,
    const POINT& ptPrevMouse,
    _int iClickRadius)
{

    // 1.  마우스 스크린 좌표 거리 계산 (픽셀 단위)
    _int iDx = ptMouse.x - ptPrevMouse.x;
    _int iDy = ptMouse.y - ptPrevMouse.y;
    _float fMouseDist = sqrtf((_float)(iDx * iDy + iDy * iDy));

    if (fMouseDist > iClickRadius)
    {
        // 다른 위치 클릭 -> 새로운 선택 시작
        m_OverlappedObjects.clear();
        Pick_Objects_All(vRayPos, vRayDir, ObjectList, m_OverlappedObjects);

        m_iCycleIndex = 0;
        m_ptLastMouse = ptMouse;

        if (!m_OverlappedObjects.empty())
            return m_OverlappedObjects.front();
        else
            return nullptr;
    }
    else
    {
        // 같은 위치 클릭 → 순환
        if (m_OverlappedObjects.empty())
        {
            Pick_Objects_All(vRayPos, vRayDir, ObjectList, m_OverlappedObjects);
            m_iCycleIndex = 0;
        }

        if (m_OverlappedObjects.empty())
            return nullptr;

        // 다음 오브젝트로 순환
        m_iCycleIndex = (m_iCycleIndex + 1) % m_OverlappedObjects.size();

        // m_iCycleIndex 번째 오브젝트 반환
        auto iter = m_OverlappedObjects.begin();
        advance(iter, m_iCycleIndex);

        m_ptLastMouse = ptMouse;
        return *iter;
    }
}

_bool CSelectionMgr::Intersect_RayAABB(const _vec3& vRayPos, const _vec3& vRayDir, const _vec3& vMin, const _vec3& vMax, _float* pDistance)
{
    // Slab Method를 이용한 RAY-AABB Intersection

    _float tMin = -FLT_MAX;
    _float tMax = FLT_MAX;

    // X축
    if (fabsf(vRayDir.x) > 0.0001f)
    {
        _float t1 = (vMin.x - vRayPos.x) / vRayDir.x;
        _float t2 = (vMax.x - vRayPos.x) / vRayDir.x;

        if (t1 > t2)
            swap(t1, t2);

        tMin = max(tMin, t1);
        tMax = min(tMax, t2);

        if (tMin > tMax)
            return false;
    }
    else
    {
        // Ray가 X 축에 평행
        if ((vRayPos.x < vMin.x) || (vRayPos.x > vMax.x))
            return false;
    }

    // Y 축
    if (fabsf(vRayDir.y) > 0.0001f)
    {
        _float t1 = (vMin.y - vRayPos.y) / vRayDir.y;
        _float t2 = (vMax.y - vRayPos.y) / vRayDir.y;

        if (t1 > t2)
            swap(t1, t2);

        tMin = max(tMin, t1);
        tMax = min(tMax, t2);

        if (tMin > tMax)
            return false;
    }
    else
    {
        // Ray가 Y축에 평행 
        if ((vRayPos.y < vMin.y) || (vRayPos.y > vMax.y))
            return false;
    }

    // Z 축
    if (fabsf(vRayDir.z) > 0.0001f)
    {
        _float t1 = (vMin.z - vRayPos.z) / vRayDir.z;
        _float t2 = (vMax.z - vRayPos.z) / vRayDir.z;

        if (t1 > t2)
            swap(t1, t2);

        tMin = max(tMin, t1);
        tMax = min(tMax, t2);

        if (tMin > tMax)
            return false;
    }
    else
    {
        if ((vRayPos.z < vMin.z) || (vRayPos.z > vMax.z))
            return false;
    }

    // 충돌 발생 ( 카메라 앞쪽만 )
    if (tMax >= 0.f)
    {
        if (pDistance)
            *pDistance = (tMin >= 0.f) ? tMin : tMax;

        return true;
    }

    return false;
}

CSelectionMgr* CSelectionMgr::Create()
{
    CSelectionMgr* pInstance = new CSelectionMgr;

    return pInstance;
}

void CSelectionMgr::Free()
{
    m_OverlappedObjects.clear();
}
