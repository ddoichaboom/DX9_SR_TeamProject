#pragma once

#include "CTransform.h"
#include "CRcCol.h"
#include "CRcTex.h"
#include "CRcTexUp.h"
#include "CCubeCol.h"
#include "CTerrainTex.h"
#include "CCubeTex.h"
#include "CDVIBuffer.h"

#include "CTexture.h"
#include "CAnimation.h"
#include "CScrollTexture.h"
#include "CCubeTexture.h"
#include "CCalculator.h"
#include "CStateComponent.h"
#include "CCollision.h"

// Sample
#include "CTerrainTexture.h"

BEGIN(Engine)

class ENGINE_DLL CProtoMgr :  public CBase
{
	DECLARE_SINGLETON(CProtoMgr)

private:
	CProtoMgr();
	virtual ~CProtoMgr();

public:
	HRESULT			Ready_Prototype(const _tchar* pComponentTag, CComponent* pComponent);
	CComponent* Clone_Prototype(const _tchar* pComponentTag);


private:
	CComponent* Find_Prototype(const _tchar* pComponentTag);

private:
	map<const _tchar*, CComponent*>		m_mapPrototype;

private:
	virtual void Free();

};

END
