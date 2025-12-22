#pragma once
#include "CBase.h"

class CMousePicker : public CBase
{
private:
    explicit CMousePicker();
    virtual ~CMousePicker();

public:
    // Ray 정보 업데이트 (전체 화면 좌표 기준)
    void Update(const _matrix& matView, const _matrix& matProj, POINT ptMouse);

    // Ray 정보 가져오기
    _vec3 Get_RayPos() const { return m_vRayPos; }
    _vec3 Get_RayDir() const { return m_vRayDir; }

private:
    // Screen to World Ray 변환
    void Screen_To_WorldRay(const _matrix& matView, const _matrix& matProj, POINT ptMouse);

private:
    _vec3   m_vRayPos;  // Ray 시작점 (월드 좌표)
    _vec3   m_vRayDir;  // Ray 방향 (정규화됨)

public:
    static CMousePicker* Create();

private:
    virtual void Free() override;
};