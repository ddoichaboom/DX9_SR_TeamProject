#include "CRcTexSide.h"

CRcTexSide::CRcTexSide()
{
}

CRcTexSide::CRcTexSide(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

CRcTexSide::CRcTexSide(const CRcTexSide& rhs)
	: CVIBuffer(rhs)
{
}

CRcTexSide::~CRcTexSide()
{
}


HRESULT		CRcTexSide::Ready_Buffer()
{
	m_dwVtxSize = sizeof(VTXTEX);
	m_dwVtxCnt = 4;
	m_dwTriCnt = 2;
	m_dwFVF = FVF_TEX;

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	VTXTEX* pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	// 정점 0 : 좌상단 
	pVertex[0].vPosition = { 0.f, 1.f, 0.f };
	pVertex[0].vTexUV = { 0.f, 0.f };

	// 정점 1 : 우상단
	pVertex[1].vPosition = { 2.f, 1.f, 0.f };
	pVertex[1].vTexUV = { 1.f, 0.f };

	// 정점 2 : 우하단 
	pVertex[2].vPosition = { 2.f, -1.f, 0.f };
	pVertex[2].vTexUV = { 1.f, 1.f };

	// 정점 3 : 좌하단
	pVertex[3].vPosition = { 0.f, -1.f, 0.f };
	pVertex[3].vTexUV = { 0.f, 1.f };

	m_pVB->Unlock();

	INDEX32* pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	// 오른쪽 위
	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	// 왼쪽 아래
	pIndex[1]._0 = 0;
	pIndex[1]._1 = 2;
	pIndex[1]._2 = 3;

	m_pIB->Unlock();

	return S_OK;
}

void		CRcTexSide::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}


CRcTexSide* CRcTexSide::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRcTexSide* pRcTexSide = new CRcTexSide(pGraphicDev);

	if (FAILED(pRcTexSide->Ready_Buffer()))
	{
		Safe_Release(pRcTexSide);
		MSG_BOX("pRcTexSide Create Failed");
		return nullptr;
	}

	return pRcTexSide;
}

CComponent* CRcTexSide::Clone()
{
	return new CRcTexSide(*this);
}


void	CRcTexSide::Free()
{
	CVIBuffer::Free();
}