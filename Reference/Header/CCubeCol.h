#pragma once
#include "CVIBuffer.h"

BEGIN(Engine)
class CCubeCol :
    public CVIBuffer
{
protected:
	explicit CCubeCol();
	explicit CCubeCol(LPDIRECT3DDEVICE9	pGraphicDev);
	explicit CCubeCol(const CCubeCol& rhs);
	virtual ~CCubeCol();

public:
	virtual		HRESULT		Ready_Buffer();
	virtual		void		Render_Buffer();

public:
	static CCubeCol* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();
	_vec3* GetVtx() { return m_pPos; }
private:
	virtual		void	Free();
	_vec3* m_pPos;
};

END