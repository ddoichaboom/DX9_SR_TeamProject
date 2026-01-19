#pragma once
#include "CComponent.h"

BEGIN(Engine)

//Dynamic Vertex Buffer Component
//Index 는 일단 제거 
class ENGINE_DLL CDVIBuffer : public CComponent
{
protected:
	explicit CDVIBuffer(LPDIRECT3DDEVICE9 pGraphicDev, _ulong maxParticleCount, _ulong batchSize);
	explicit CDVIBuffer(const CDVIBuffer& rhs);
	virtual ~CDVIBuffer();

public:
	virtual		HRESULT		Ready_Buffer();
	virtual		void		Render_Buffer(list<Particle*>& _particles);

	void					SetParticleCount(_ulong _cnt) { m_dwParticleCnt = _cnt; }
	void					SetBatchSize(_ulong _batchSize) { m_dwVtxBatchSize = _batchSize; }

public:
	static CDVIBuffer*		Create(LPDIRECT3DDEVICE9	pGraphicDev, _ulong maxCount, _ulong batchSize);
	void					Free() override;
	CComponent*				Clone() override;

protected:
	LPDIRECT3DVERTEXBUFFER9	m_pVB;
	// LPDIRECT3DINDEXBUFFER9	m_pIB;


	_ulong					m_dwFVF;
	_ulong					m_dwVtxCnt;
	_ulong					m_dwVtxSize;		//버텍스 크기 
	_ulong					m_dwParticleCnt;	//버텍스 버퍼가 저장할 수 있는 파티클 수 
	_ulong					m_dwVtxOffset;		//다음 세그먼트까지의	오프셋 (파티클 단위) 
	_ulong					m_dwVtxBatchSize;	//한 세그먼트에 정의된 파티클 수

	//_ulong					m_dwIdxSize;
	//_ulong					m_dwTriCnt;
	//D3DFORMAT				m_IdxFmt;

	const _int VtxN = 6;
	//const _int IdxN = 6;
};

END
