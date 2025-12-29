#pragma once
#include "CComponent.h"
#include "Engine_Define.h"

BEGIN(Engine)
class CCollider;

class ENGINE_DLL CCalculator : public CComponent
{
private:
	explicit CCalculator(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCalculator(const CCalculator& rhs);
	virtual ~CCalculator();

public:
	//아래 함수들 Collision Component 로 옮김 
	//World Space
	//void	GetRay(HWND hWnd, _vec3* pOutRayWorldPos, _vec3* pOutRayWorldDir);
	//bool	Check_PickedCollider(HWND hWnd, CCollider* pCollider);

public:
	static CCalculator* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();

private:
	virtual void Free();
};
END