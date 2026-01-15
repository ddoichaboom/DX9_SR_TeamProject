#include "CEventMgr.h"

IMPLEMENT_SINGLETON(CEventMgr)

CEventMgr::CEventMgr()
{
}

CEventMgr::~CEventMgr()
{
}

void CEventMgr::Subscribe(EVENT_TYPE _event, IListener* _listener)
{
	m_mapListeners[_event].push_back(_listener);
}

void CEventMgr::UnSubscribe(EVENT_TYPE _event, IListener* _listener)
{
	if (m_mapListeners.find(_event) == m_mapListeners.end()) return;
	m_mapListeners[_event].remove(_listener);
}

void CEventMgr::Broadcast(EVENT_TYPE _event, EventData* _pData)
{
	if (m_mapListeners.find(_event) == m_mapListeners.end()) return;

	for (IListener* listener : m_mapListeners[_event])
	{
		listener->OnEvent(_event, _pData);
	}
}

void CEventMgr::ClearAllSubscribe()
{
	m_mapListeners.clear();
}

void CEventMgr::Free()
{
	ClearAllSubscribe();
}
