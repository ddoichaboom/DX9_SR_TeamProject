#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)


//구독자 인터페이스. 메시지를 받을 대상은 이 인터페이스를 상속받아야함 
class IListener
{
public:
	virtual ~IListener() {}
	virtual void OnEvent(EVENT_TYPE _type, EventData* _pData) PURE;
};

class ENGINE_DLL CEventMgr : public CBase
{
	DECLARE_SINGLETON(CEventMgr)

private:
	explicit CEventMgr();
	virtual ~CEventMgr();

public:
	void Subscribe(EVENT_TYPE _event, IListener* _listener);
	void UnSubscribe(EVENT_TYPE _event, IListener* _listener);
	void Broadcast(EVENT_TYPE _event, EventData* _pData);
	void ClearAllSubscribe();
private:
	virtual void Free();

protected:
	map<EVENT_TYPE, list<IListener*>> m_mapListeners;

};

END