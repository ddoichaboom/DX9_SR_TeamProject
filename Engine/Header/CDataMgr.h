#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "CState.h"

/*
1. State 정보
2. Texture, Animation 정보

위의 데이터를 보관하고 다루는 Mgr
템플릿인 이유는 타입별로 데이터를 다루기 위함
*/

BEGIN(Engine)

template <typename T>
class  CDataMgr
{
public:
	CDataMgr(const CDataMgr& mgr) = delete;
	CDataMgr& operator=(const CDataMgr& mgr) = delete;
private:
	static CDataMgr* m_pInstance;

public:                            
	static CDataMgr* GetInstance()
	{
		if (m_pInstance == NULL)
		{
			m_pInstance = new CDataMgr;
		}
		return m_pInstance;
	}
	static void DestroyInstance()
	{
		if (m_pInstance != NULL)
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}

private:
	explicit CDataMgr() {}
	virtual ~CDataMgr() { Free(); }

	//State
public:
	void AddState(_uint _id, CState<T>* _state)
	{
		if (m_pStates.find(_id) != m_pStates.end()) return;
		m_pStates[_id] = _state;
	}

	CState<T>* GetState(_uint _id)
	{
		auto iter = m_pStates.find(_id);
		if (iter == m_pStates.end()) return nullptr;
		else return iter->second;
	}

	bool IsStateEmpty() { return m_pStates.empty(); }

private:
	virtual void	Free()
	{
		for (auto& state : m_pStates)
		{
			Safe_Delete(state.second);
		}
		m_pStates.clear();
	}

private:
	map<_uint, CState<T>*> m_pStates;

};

template <typename T>
CDataMgr<T>* CDataMgr<T>::m_pInstance = NULL;

END
