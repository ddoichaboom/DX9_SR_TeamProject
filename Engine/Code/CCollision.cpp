#include "CCollision.h"
#include "CTransform.h"

CCollision::CCollision()
	:m_pCollider(nullptr), m_bCanCollision(true)
{
}

CCollision::CCollision(LPDIRECT3DDEVICE9 pGraphicDev)
	:CComponent(pGraphicDev), m_pCollider(nullptr), m_bCanCollision(true)
{
}

CCollision::CCollision(const CCollision& rhs)
	:CComponent(rhs), m_pCollider(nullptr), m_bCanCollision(true)
{
}

CCollision::~CCollision()
{
}

void CCollision::CreateCollider(CTransform* _prtTransComp)
{
	if (m_pCollider)
	{
		Safe_Release(m_pCollider);
		m_pCollider = nullptr;
	}
	m_pCollider = CCollider::Create(m_pGraphicDev, _prtTransComp);
}


_int CCollision::Update_Component(const _float& fTimeDelta)
{
	if (m_pCollider)
	{
		m_pCollider->Update_GameObject(fTimeDelta);
	}
    return 0;
}

void CCollision::LateUpdate_Component()
{
	if (m_pCollider)
	{
		m_pCollider->LateUpdate_GameObject(0);
	}
}


void CCollision::SetCollision(CollisionInfo info)
{
    if (m_pCollider) m_pCollider->Collision(info);
}

void CCollision::Collision_Base(CCollider* _other)
{
	if (!m_pCollider || !m_bCanCollision) return;
	if (CheckCollision(_other))
	{
		//CollisionInfo info = { _other,{0,0,0} };
		m_pCollider->Collision({ _other,{0,0,0} });
		_other->Collision({m_pCollider,{0,0,0}});
	}
}

bool CCollision::CheckCollision(CCollider* _other)
{
	//회전이 없기때문에 로컬의 최소, 최대 정점이 월드에서도 최소, 최대점임
	//AABB 
	_vec3 myMin = { -1.f, -1.f, -1.f };
	_vec3 myMax = { 1.f, 1.f, 1.f };
	_matrix MyMatrix = m_pCollider->GetWorldMatrix();
	D3DXVec3TransformCoord(&myMin, &myMin, &MyMatrix);
	D3DXVec3TransformCoord(&myMax, &myMax, &MyMatrix);

	_vec3 otherMin = { -1.f, -1.f, -1.f };
	_vec3 otherMax = { 1.f, 1.f, 1.f };
	_matrix otherMatrix = _other->GetWorldMatrix();
	D3DXVec3TransformCoord(&otherMin, &otherMin, &otherMatrix);
	D3DXVec3TransformCoord(&otherMax, &otherMax, &otherMatrix);

	bool result = myMin.x <= otherMax.x && otherMin.x <= myMax.x;
	result &= myMin.y <= otherMax.y && otherMin.y <= myMax.y;
	result &= myMin.z <= otherMax.z && otherMin.z <= myMax.z;

	return result;
}

//겹치는 부분을 파악하기 위한 충돌
//벽 충돌 or Collider가 있는 오브젝트 충돌용 
bool CCollision::CheckCollision_Diff(CCollider* _other, _vec3* diff)
{
	if (!m_pCollider) return false;

	_vec3 origin = { 0.f, 0.f, 0.f };
	_matrix MyMatrix = m_pCollider->GetWorldMatrix();
	D3DXVec3TransformCoord(&origin, &origin, &MyMatrix); // 원점 월드 위치 

	// 로컬의 각 축의 너비는 2, 절반은 1이므로 반지름 == 크기 로 대체가능 
	_vec3 radius = m_pCollider->Get_Scale(); 


	_vec3 otherOrigin = { 0.f, 0.f, 0.f };
	_matrix otherMatrix = _other->GetWorldMatrix();
	D3DXVec3TransformCoord(&otherOrigin, &otherOrigin, &otherMatrix);
	_vec3 otherRadius = _other->Get_Scale();


	float fHorizontal = fabsf(origin.x - otherOrigin.x );
	float fVertical = fabsf(origin.y - otherOrigin.y);
	float fDepth = fabsf(origin.z - otherOrigin.z);

	_vec3 totalRadius = radius + otherRadius;

	if ((totalRadius.x > fHorizontal) && (totalRadius.y > fVertical) && (totalRadius.z >fDepth))
	{
		diff->x = totalRadius.x - fHorizontal;
		diff->y = totalRadius.y - fVertical;
		diff->z = totalRadius.z - fDepth;
		return true;
	}
	return false;

}

void CCollision::GetRay(HWND hWnd, _vec3* pOutRayWorldPos, _vec3* pOutRayWorldDir)
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

bool CCollision::Collision_Mouse(HWND hWnd)
{
    if (!m_pCollider || !m_bCanCollision) return false;

    const float eps = 1e-6f;
    _vec3 vRayPos, vRayDir;
    GetRay(hWnd, &vRayPos, &vRayDir);

    _matrix mat = m_pCollider->GetWorldMatrix();

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

bool CCollision::Collision_Mouse_Other(HWND hWnd, CCollision* _pCollision)
{
    if (!_pCollision || _pCollision->CanCollision()) return false;
    CCollider* pCollider = _pCollision->GetCollider();
    if (!pCollider) return false;

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

CCollision* CCollision::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCollision* pCollision = new CCollision(pGraphicDev);
	return pCollision;
}

CComponent* CCollision::Clone()
{
	return new CCollision(*this);
}

void CCollision::Free()
{
	if (m_pCollider) Safe_Release(m_pCollider);
	CComponent::Free();
}