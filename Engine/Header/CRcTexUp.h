#pragma once

#include "CVIBuffer.h"

BEGIN(Engine)
//정점의 위치가 0,0을 기준으로 아래쪽에 생성됨
// TexUp..이름에 맡게 원래는 위쪽으로 생성됐으나 Beam용으로만 사용할듯하여 밑으로 변경함
//회전, 스케일을 중점이 아닌 끝점을 기준하기위함 
class ENGINE_DLL CRcTexUp : public CVIBuffer
{
protected:
	explicit CRcTexUp();
	explicit CRcTexUp(LPDIRECT3DDEVICE9	pGraphicDev);
	explicit CRcTexUp(const CRcTexUp& rhs);
	virtual ~CRcTexUp();

public:
	virtual		HRESULT		Ready_Buffer();
	virtual		void		Render_Buffer();


public:
	static CRcTexUp* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();
private:
	virtual		void	Free();
};

END
