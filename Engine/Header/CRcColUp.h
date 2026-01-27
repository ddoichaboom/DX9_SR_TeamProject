#pragma once

#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CRcColSide : public CVIBuffer
{
protected:
	explicit CRcColSide();
	explicit CRcColSide(LPDIRECT3DDEVICE9	pGraphicDev, D3DXCOLOR _color = { 1,0,0,1 });
	explicit CRcColSide(const CRcColSide& rhs);
	virtual ~CRcColSide();

public:
	virtual		HRESULT		Ready_Buffer();
	virtual		void		Render_Buffer();


public:
	static CRcColSide* Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXCOLOR _color = {1,0,0,1});
	virtual CComponent* Clone();
private:
	virtual		void	Free();
	
private:
	D3DXCOLOR m_Color{};
};

END
