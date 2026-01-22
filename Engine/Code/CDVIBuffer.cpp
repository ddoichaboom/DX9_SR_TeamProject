#include "CDVIBuffer.h"

CDVIBuffer::CDVIBuffer(LPDIRECT3DDEVICE9 pGraphicDev, _ulong maxParticleCount, _ulong batchSize)
	: CComponent(pGraphicDev), m_pVB(nullptr),  m_dwFVF(FVF_PTC), m_dwVtxSize(sizeof(VTXPTC))
	, m_dwParticleCnt(maxParticleCount), m_dwVtxOffset(0), m_dwVtxBatchSize(batchSize), m_dwVtxCnt(0)
{
}

CDVIBuffer::CDVIBuffer(const CDVIBuffer& rhs)
	: CComponent(rhs), m_pVB(rhs.m_pVB),m_dwFVF(rhs.m_dwFVF), m_dwVtxSize(rhs.m_dwVtxSize)
	, m_dwParticleCnt(rhs.m_dwParticleCnt), m_dwVtxOffset(0), m_dwVtxBatchSize(rhs.m_dwVtxBatchSize), m_dwVtxCnt(rhs.m_dwVtxCnt)
{
	m_pVB->AddRef();
}

CDVIBuffer::~CDVIBuffer()
{
}

HRESULT	CDVIBuffer::Ready_Buffer()
{
	m_dwVtxCnt = VtxN * m_dwParticleCnt;
	_ulong TotatlVtxSize = m_dwVtxCnt * m_dwVtxSize;
	//동적 정점 버퍼
	if (FAILED(m_pGraphicDev->CreateVertexBuffer
		(TotatlVtxSize, 
		D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY
		, FVF_PTC
		, D3DPOOL_DEFAULT
		, &m_pVB
		, NULL))) return E_FAIL;

	return S_OK;
}

void CDVIBuffer::Render_Buffer(list<Particle*>& _particles)
{
	if (_particles.empty()) return;

	//빌보드 
	_matrix View, InvView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &View);
	D3DXMatrixInverse(&InvView, NULL, &View);

	_vec3 vRight(InvView._11, InvView._12, InvView._13);
	_vec3 vUp(InvView._21, InvView._22, InvView._23);
	_vec3 vLook(InvView._31, InvView._32, InvView._33);

	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, m_dwVtxSize);
	m_pGraphicDev->SetFVF(FVF_PTC);


	//버퍼가 다 찼다면 처음으로 
	if (m_dwVtxOffset >= m_dwParticleCnt) m_dwVtxOffset = 0;
	VTXPTC* vp = NULL;

	//offset이 0이면 기존 메모리 놔두고 새 메모리 얻는다 (DISCARD) 
	//0이 아니면 사용중인 메모리의 다음 빈 공간을 쓰겠다(겹치게 하지않겠다) (NOOVERWRITE) 
	m_pVB->Lock(m_dwVtxOffset * sizeof(VTXPTC) * VtxN,
		m_dwVtxBatchSize * sizeof(VTXPTC) * VtxN, (void**)&vp
		, m_dwVtxOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

	DWORD numParticlesInBatch = 0;

	for (auto iter = _particles.begin(); iter != _particles.end(); iter++)
	{
		if ((*iter)->bIsAlive)
		{
			//방향이 있다면 재구성
			if ((*iter)->bDirection)
			{
				vUp = (*iter)->vDirection;
				D3DXVec3Cross(&vRight, &vUp, &vLook);//vLook
				D3DXVec3Normalize(&vRight, &vRight);
			}

			_vec2 halfSize = (*iter)->vSize * 0.5f;

			vp[0].vPosition = (*iter)->vPosition - (vRight * halfSize.x) + (vUp * halfSize.y);
			vp[0].dwColor = (*iter)->color;
			vp[0].vTexUV = (*iter)->vStartUV;

			vp[1].vPosition = (*iter)->vPosition + (vRight * halfSize.x) - (vUp * halfSize.y);
			vp[1].dwColor = (*iter)->color;
			vp[1].vTexUV = (*iter)->vEndUV;

			vp[2].vPosition = (*iter)->vPosition - (vRight * halfSize.x) - (vUp * halfSize.y);
			vp[2].dwColor = (*iter)->color;
			vp[2].vTexUV.x = (*iter)->vStartUV.x;
			vp[2].vTexUV.y = (*iter)->vEndUV.y;


			vp[3].vPosition = (*iter)->vPosition - (vRight * halfSize.x) + (vUp * halfSize.y);
			vp[3].dwColor = (*iter)->color;
			vp[3].vTexUV = (*iter)->vStartUV;

			vp[4].vPosition = (*iter)->vPosition + (vRight * halfSize.x) + (vUp * halfSize.y);
			vp[4].dwColor = (*iter)->color;
			vp[4].vTexUV.x = (*iter)->vEndUV.x;
			vp[4].vTexUV.y = (*iter)->vStartUV.y;

			vp[5].vPosition = (*iter)->vPosition + (vRight * halfSize.x) - (vUp * halfSize.y);
			vp[5].dwColor = (*iter)->color;
			vp[5].vTexUV = (*iter)->vEndUV;

			vp += VtxN;
			numParticlesInBatch++;

			//현재 세그먼트가 모두 찼다면 복사한 정점 그리기 
			if (numParticlesInBatch == m_dwVtxBatchSize)
			{
				m_pVB->Unlock();

				m_pGraphicDev->DrawPrimitive(D3DPT_TRIANGLELIST, m_dwVtxOffset* VtxN, m_dwVtxBatchSize * 2);
				// m_pGraphicDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwVtxCnt, 0, m_dwTriCnt);
				m_dwVtxOffset += m_dwVtxBatchSize;

				//마지막 세그먼트 였다면 처음으로 돌아감 
				if (m_dwVtxOffset >= m_dwParticleCnt) m_dwVtxOffset = 0;

				m_pVB->Lock(m_dwVtxOffset * sizeof(VTXPTC) * VtxN,
					m_dwVtxBatchSize * sizeof(VTXPTC) * VtxN, (void**)&vp
					, m_dwVtxOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

				numParticlesInBatch = 0;
			}
		}
	}
	m_pVB->Unlock();

	//세그먼트를 모두 채우지 못하여 남아있는 정점이 있다면 
	if (numParticlesInBatch)
	{
		m_pGraphicDev->DrawPrimitive(D3DPT_TRIANGLELIST, m_dwVtxOffset * VtxN, numParticlesInBatch * 2);
	}
	m_dwVtxOffset += m_dwVtxBatchSize;

}

CDVIBuffer* CDVIBuffer::Create(LPDIRECT3DDEVICE9 pGraphicDev, _ulong maxCount, _ulong batchSize)
{
	CDVIBuffer* pDVIBuffer = new CDVIBuffer(pGraphicDev, maxCount, batchSize);
	if (FAILED(pDVIBuffer->Ready_Buffer()))
	{
		Safe_Release(pDVIBuffer);
		MSG_BOX("DVIBuffer Create Failed");
		return nullptr;
	}

	return pDVIBuffer;
}

void CDVIBuffer::Free()
{
	CComponent::Free();
}

CComponent* CDVIBuffer::Clone()
{
	return new CDVIBuffer(*this);
}
