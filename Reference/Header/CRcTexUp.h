#pragma once

#include "CVIBuffer.h"

BEGIN(Engine)
//정점의 위치가 0,0을 기준으로 윗쪽에 생성됨
//회전, 스케일을 중점이 아닌 끝점을 기준하기위함 
class ENGINE_DLL CRcTexUp : public CVIBuffer
{
protected:
	explicit CRcTexUp();
	explicit CRcTexUp(LPDIRECT3DDEVICE9	pGraphicDev);
	explicit CRcTexUp(const CRcTexUp& rhs);
	virtual ~CRcTexUp();

public:
	const _vec3* Get_VtxPos() { return m_pPos; }

public:
	virtual		HRESULT		Ready_Buffer();
	virtual		void		Render_Buffer();


public:
	static CRcTexUp* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();
private:
	virtual		void	Free();

private:
	_vec3* m_pPos;
};

END
