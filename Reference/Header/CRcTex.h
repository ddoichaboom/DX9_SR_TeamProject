#pragma once

#include "CVIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CRcTex : public CVIBuffer
{
protected:
	explicit CRcTex();
	explicit CRcTex(LPDIRECT3DDEVICE9	pGraphicDev);
	explicit CRcTex(const CRcTex& rhs);
	virtual ~CRcTex();

public :
	const _vec3* Get_VtxPos() { return m_pPos; }

public:
	virtual		HRESULT		Ready_Buffer();
	virtual		void		Render_Buffer();


public:
	static CRcTex* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();
private:
	virtual		void	Free();

private :
	_vec3*		m_pPos;
};

END
