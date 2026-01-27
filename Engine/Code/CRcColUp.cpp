#include "CRcColUp.h"

CRcColSide::CRcColSide()
{
}

CRcColSide::CRcColSide(LPDIRECT3DDEVICE9 pGraphicDev, D3DXCOLOR _color )
	:CVIBuffer(pGraphicDev),m_Color(_color)
{
}

CRcColSide::CRcColSide(const CRcColSide& rhs)
	: CVIBuffer(rhs)
{
}

CRcColSide::~CRcColSide()
{
}

HRESULT CRcColSide::Ready_Buffer()
{
	m_dwVtxSize = sizeof(VTXCOL);
	m_dwVtxCnt = 4;
	m_dwTriCnt = 2;
	m_dwFVF = FVF_COL;

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	VTXCOL* pVertex = NULL;

	// &pVertex : 버텍스 버퍼에 저장된 정점 중 첫 번째 주소를 얻어 옴.

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	// 오른쪽 위
	pVertex[0].vPosition = { 0.f, 1.f, 0.f };
	pVertex[0].dwColor = m_Color;

	pVertex[1].vPosition = { 2.f, 1.f, 0.f };
	pVertex[1].dwColor = m_Color;

	pVertex[2].vPosition = { 2.f, -1.f, 0.f };
	pVertex[2].dwColor = m_Color;

	pVertex[3].vPosition = { 0.f, -1.f, 0.f };
	pVertex[3].dwColor = m_Color;

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

void CRcColSide::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

CRcColSide* CRcColSide::Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXCOLOR _color)
{
	CRcColSide* pRcCol = new CRcColSide(pGraphicDev, _color);

	if (FAILED(pRcCol->Ready_Buffer()))
	{
		Safe_Release(pRcCol);
		MSG_BOX("pRcColUp Create Failed");
		return nullptr;
	}

	return pRcCol;
}

CComponent* CRcColSide::Clone()
{
	return new CRcColSide(*this);
}

void CRcColSide::Free()
{
	CVIBuffer::Free();
}
