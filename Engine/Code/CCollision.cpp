#include "CCollision.h"
#include "CTransform.h"

CCollision::CCollision()
{
}

CCollision::CCollision(LPDIRECT3DDEVICE9 pGraphicDev)
	:CComponent(pGraphicDev)
{
}

CCollision::CCollision(const CCollision& rhs)
	:CComponent(rhs)
{
}

CCollision::~CCollision()
{
}

CCollider* CCollision::CreateCollider(CTransform* _prtTransComp, const _tchar* _name)
{
	CCollider * pCollider = CCollider::Create(m_pGraphicDev, _prtTransComp);
    if (m_mapCollider.find(_name) != m_mapCollider.end())
    {
        Safe_Release(pCollider);
        return nullptr;
    }
    m_mapCollider.insert({ _name, pCollider });
    return pCollider;
}


_int CCollision::Update_Component(const _float& fTimeDelta)
{
    for (auto mp : m_mapCollider)
    {
         mp.second->Update_GameObject(fTimeDelta);
    }
    return 0;
}

void CCollision::LateUpdate_Component()
{
    for (auto mp : m_mapCollider)
    {
          mp.second->LateUpdate_GameObject(0);
    }
}


void CCollision::SetCollision(CollisionInfo info, const _tchar* _name)
{
    auto iter = m_mapCollider.find(_name);
    if (iter != m_mapCollider.end())
    {
        iter->second->Collision(info);
    }
}

void CCollision::Collision_Base(CCollider* _aCol, CCollider* _bCol)
{
    if (!_aCol || !_bCol) return;
    if (!_aCol->CanCollision() || !_bCol->CanCollision()) return;

	if (CheckCollision(_aCol, _bCol))
	{
        _aCol->Collision({ _bCol,{0,0,0} });
        _bCol->Collision({ _aCol,{0,0,0}});
	}
}

bool CCollision::CheckCollision(CCollider* _aCol, CCollider* _bCol)
{
    if (!_aCol || !_bCol) return false;
	//회전이 없기때문에 로컬의 최소, 최대 정점이 월드에서도 최소, 최대점임
	//AABB 
	_vec3 myMin = { -1.f, -1.f, -1.f };
	_vec3 myMax = { 1.f, 1.f, 1.f };
	_matrix MyMatrix = _aCol->GetWorldMatrix();
	D3DXVec3TransformCoord(&myMin, &myMin, &MyMatrix);
	D3DXVec3TransformCoord(&myMax, &myMax, &MyMatrix);

	_vec3 otherMin = { -1.f, -1.f, -1.f };
	_vec3 otherMax = { 1.f, 1.f, 1.f };
	_matrix otherMatrix = _bCol->GetWorldMatrix();
	D3DXVec3TransformCoord(&otherMin, &otherMin, &otherMatrix);
	D3DXVec3TransformCoord(&otherMax, &otherMax, &otherMatrix);

	bool result = myMin.x <= otherMax.x && otherMin.x <= myMax.x;
	result &= myMin.y <= otherMax.y && otherMin.y <= myMax.y;
	result &= myMin.z <= otherMax.z && otherMin.z <= myMax.z;

	return result;
}

//겹치는 부분을 파악하기 위한 충돌
//벽 충돌 or Collider가 있는 오브젝트 충돌용 
bool CCollision::CheckCollision_Diff(CCollider* _aCol, CCollider* _bCol, _vec3* diff)
{
    if (!_aCol || !_bCol) return false;

	_vec3 origin = { 0.f, 0.f, 0.f };
	_matrix MyMatrix = _aCol->GetWorldMatrix();
	D3DXVec3TransformCoord(&origin, &origin, &MyMatrix); // 원점 월드 위치 

	// 로컬의 각 축의 너비는 2, 절반은 1이므로 반지름 == 크기 로 대체가능 
	_vec3 radius = _aCol->Get_Scale();


	_vec3 otherOrigin = { 0.f, 0.f, 0.f };
	_matrix otherMatrix = _bCol->GetWorldMatrix();
	D3DXVec3TransformCoord(&otherOrigin, &otherOrigin, &otherMatrix);
	_vec3 otherRadius = _bCol->Get_Scale();


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

bool CCollision::Collision_Mouse(HWND hWnd, LPDIRECT3DDEVICE9 _pGraphicDev, CCollider* _col)
{
    if (!_col || !_col->CanCollision() ||  !_pGraphicDev) return false;
    const float eps = 1e-6f;
    _vec3 vRayPos, vRayDir;
    GetRay(hWnd, _pGraphicDev ,&vRayPos, &vRayDir);
    return Collision_Ray(_col, vRayPos, vRayDir);
}

bool CCollision::Collision_Ray(CCollider* _col, _vec3 _RayPos, _vec3 _RayDir)
{
    static const float eps = 1e-6f;
    _matrix mat = _col->GetWorldMatrix();

    D3DXMatrixInverse(&mat, NULL, &mat);
    D3DXVec3TransformCoord(&_RayPos, &_RayPos, &mat);
    D3DXVec3TransformNormal(&_RayDir, &_RayDir, &mat);
    D3DXVec3Normalize(&_RayDir, &_RayDir);

    _vec3 minPoint = { -1.f, -1.f, -1.f };
    _vec3 maxPoint = { 1.f, 1.f, 1.f };

    float rMin = -FLT_MIN, rMax = FLT_MAX;
    float tMinx = -FLT_MIN, tMaxx = FLT_MAX,
        tMiny = -FLT_MIN, tMaxy = FLT_MAX, tMinz = -FLT_MIN, tMaxz = FLT_MAX;


    if (fabsf(_RayDir.x) >= eps)
    {
        tMinx = (minPoint.x - _RayPos.x) / _RayDir.x;
        tMaxx = (maxPoint.x - _RayPos.x) / _RayDir.x;

        if (tMinx > tMaxx) swap(tMinx, tMaxx);
    }
    else
    {
        if (_RayPos.x < minPoint.x || _RayPos.x > maxPoint.x) return false;
    }

    if (fabsf(_RayDir.y) >= eps)
    {
        tMiny = (minPoint.y - _RayPos.y) / _RayDir.y;
        tMaxy = (maxPoint.y - _RayPos.y) / _RayDir.y;

        if (tMiny > tMaxy) swap(tMiny, tMaxy);
    }
    else
    {
        if (_RayPos.y < minPoint.y || _RayPos.y > maxPoint.y) return false;
    }

    if (fabsf(_RayDir.z) >= eps)
    {
        tMinz = (minPoint.z - _RayPos.z) / _RayDir.z;
        tMaxz = (maxPoint.z - _RayPos.z) / _RayDir.z;

        if (tMinz > tMaxz) swap(tMinz, tMaxz);
    }
    else
    {
        if (_RayPos.z < minPoint.z || _RayPos.z > maxPoint.z)
            return false;
    }

    rMin = max(tMinx, max(tMiny, tMinz));
    rMax = min(tMaxx, min(tMaxy, tMaxz));

    return rMin <= rMax;
}

void CCollision::GetRay(HWND hWnd, LPDIRECT3DDEVICE9 _pGraphicDev, _vec3* pOutRayWorldPos, _vec3* pOutRayWorldDir)
{
    if (!_pGraphicDev) return;

    POINT       ptMouse{};
    GetCursorPos(&ptMouse);
    ScreenToClient(hWnd, &ptMouse);
    _vec3   vMousePos;

    D3DVIEWPORT9            ViewPort;
    ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));

    _pGraphicDev->GetViewport(&ViewPort);

    vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
    vMousePos.y = ptMouse.y / -(ViewPort.Height * 0.5f) + 1.f;
    vMousePos.z = 0.f;

    D3DXMATRIX      matProj;
    _pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
    D3DXMatrixInverse(&matProj, 0, &matProj);
    D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

    D3DXMATRIX      matView;
    _pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
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


CCollision* CCollision::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCollision* pCollision = new CCollision(pGraphicDev);
	return pCollision;
}

CComponent* CCollision::Clone()
{
	return new CCollision(*this);
}

void CCollision::Reset()
{
    for (auto& mp : m_mapCollider)
    {
        mp.second->OnCollision();
    }
}

void CCollision::Free()
{
    for (auto& mp : m_mapCollider)
    {
        Safe_Release(mp.second);
    }
    m_mapCollider.clear();
	CComponent::Free();
}