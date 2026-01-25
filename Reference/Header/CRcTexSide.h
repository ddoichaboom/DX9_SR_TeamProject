#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CRcTexSide : public CVIBuffer
{
protected:
	explicit CRcTexSide();
	explicit CRcTexSide(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CRcTexSide(const CRcTexSide& rhs);
	virtual ~CRcTexSide();

public:
	virtual		HRESULT		Ready_Buffer();
	virtual		void		Render_Buffer();

public:
	static	CRcTexSide* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();

private:
	virtual		void	Free();
};

END