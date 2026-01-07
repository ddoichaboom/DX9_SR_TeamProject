#include "CRcTexUp.h"

CRcTexUp::CRcTexUp()
{
}

CRcTexUp::CRcTexUp(LPDIRECT3DDEVICE9 pGraphicDev)
	:CVIBuffer(pGraphicDev)
{
}

CRcTexUp::CRcTexUp(const CRcTexUp& rhs)
	: CVIBuffer(rhs)
{
}

CRcTexUp::~CRcTexUp()
{
}

HRESULT CRcTexUp::Ready_Buffer()
{
	m_dwVtxSize = sizeof(VTXTEX);
	m_dwVtxCnt = 4;
	m_dwTriCnt = 2;
	m_dwFVF = FVF_TEX;

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	if (FAILED(CVIBuffer::Ready_Buffer()))
		return E_FAIL;

	VTXTEX* pVertex = NULL;

	// &pVertex : 버텍스 버퍼에 저장된 정점 중 첫 번째 주소를 얻어 옴.

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	// 오른쪽 위
	pVertex[0].vPosition = { -2.f, 0.f, 0.f };
	pVertex[0].vTexUV = { 0.f, 0.f };

	pVertex[1].vPosition = { 2.f, 0.f, 0.f };
	pVertex[1].vTexUV = { 1.f, 0.f };

	pVertex[2].vPosition = { 2.f, -2.f, 0.f };
	pVertex[2].vTexUV = { 1.f, 1.f };

	pVertex[3].vPosition = { -2.f, -2.f, 0.f };
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

void CRcTexUp::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

CRcTexUp* CRcTexUp::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRcTexUp* pRcTexUp = new CRcTexUp(pGraphicDev);

	if (FAILED(pRcTexUp->Ready_Buffer()))
	{
		Safe_Release(pRcTexUp);
		MSG_BOX("pRcTexUp Create Failed");
		return nullptr;
	}

	return pRcTexUp;
}

CComponent* CRcTexUp::Clone()
{
	return new CRcTexUp(*this);
}

void CRcTexUp::Free()
{
	CVIBuffer::Free();
}
