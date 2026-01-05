#include "CPoolMgr.h"

IMPLEMENT_SINGLETON(CPoolMgr)

CPoolMgr::CPoolMgr()
{
}

CPoolMgr::~CPoolMgr()
{
	Free();
}

void CPoolMgr::Free()
{
    for_each(m_mapPool.begin(), m_mapPool.end(), CDeleteMap());
    m_mapPool.clear();
}
