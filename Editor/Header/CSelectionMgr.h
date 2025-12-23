#pragma once
#include "CBase.h"

class CEditorObject;

class CSelectionMgr : public CBase
{
private:
    explicit CSelectionMgr();
    virtual ~CSelectionMgr();

public:
    // 선택 관리
    void Set_Selection(CEditorObject* pObj);
    CEditorObject* Get_Selection() { return m_pSelectedObject; }

    // Ray-AABB Intersection ( 가장 가까운 오브젝트 )
    CEditorObject* Pick_Object(const _vec3& vRayPos,
        const _vec3& vRayDir,
        list<CEditorObject*>& objectList);

    // 겹친 오브젝트 모두 찾기 
    void                Pick_Objects_All(const _vec3& vRayPos,
        const _vec3& vRayDir,
        const list<CEditorObject*>& ObjectList,
        list<CEditorObject*>& outPickedList);

    // 연속 클릭으로 순환 선택
    CEditorObject* Pick_Object_Cycle(const _vec3& vRayPos, const _vec3& vRayDir,
        const list<CEditorObject*>& ObjectList,
        const POINT& ptMouse,
        const POINT& ptPrevMouse,
        _int iClickRadius = 5);  // 픽셀 단위

private:
    // Ray-AABB 충돌 검사 (Slab Method)
    _bool               Intersect_RayAABB(const _vec3& vRayPos,
        const _vec3& vRayDir,
        const _vec3& vMin,
        const _vec3& vMax,
        _float* pDistance);

private:
    CEditorObject* m_pSelectedObject;
    POINT                   m_ptLastMouse;
    list<CEditorObject*>    m_OverlappedObjects;
    _uint                   m_iCycleIndex;

public:
    static CSelectionMgr* Create();

private:
    virtual void Free() override;
};

