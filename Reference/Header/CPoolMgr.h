#pragma once
#include "CBase.h"
#include "CObjectPool.h"
#include "Engine_Define.h"
#include <typeinfo>

// 클래스타입의 이름을 Key로 오브젝트 풀을 보관함 
// 오브젝트풀의 생성을 런타임 중 한 번 명시적으로 해주기 (반드시 컴포넌트 생성 뒤에) 
// 오브젝트 스스로 풀에 반환되므로 반환기능은 구현x함 

BEGIN(Engine)

class ENGINE_DLL CPoolMgr : public CBase
{
	DECLARE_SINGLETON(CPoolMgr)

private:
	explicit CPoolMgr();
	virtual ~CPoolMgr();

public:
	template<class T>
	T* Get_Object()
	{
		string szClassType = typeid(T).name(); // 클래스의 이름을 반환. CBullet이면 "CBullet" 

		auto iter = m_mapPool.find(szClassType);
		if (iter == m_mapPool.end()) return nullptr;

		CObjectPool<T>* pPool = static_cast<CObjectPool<T>*>(iter->second);
		T * pObj = pPool->Get_FreeObject();
		return pObj;
	}

	//명시적으로 호출하기
	// -> 생성 시점을 제어하기 위함 
	template<class T>
	HRESULT CreatePool(LPDIRECT3DDEVICE9 pGraphicDev)
	{
		//이미 생성됐으면 종료
		string szClassType = typeid(T).name();
		if (m_mapPool.find(szClassType) != m_mapPool.end()) return S_OK;

		//세팅해놓은 poolSize대로 생성 
		_uint iPoolSize = 0;
		auto iter = m_mapPoolSize.find(szClassType);
		if (iter == m_mapPoolSize.end()) iPoolSize = m_iDefaultPoolSize; // 세팅값이 없으면 디폴트 값
		else iPoolSize = iter->second;

		IBasePool * pool = CObjectPool<T>::Create(pGraphicDev, iPoolSize);
		if (!pool) return E_FAIL;

		m_mapPool.insert({ szClassType, pool });
		return S_OK;
	}

	template<class T> 
	void DeletePool()
	{
		string szClassType = typeid(T).name();
		auto iter = m_mapPool.find(szClassType);
		if (iter == m_mapPool.end()) return;
		Safe_Release(iter->second);
		m_mapPool.erase(iter);
	}

	//T타입 오브젝트풀이 생성이 됐는가 
	template<class T>
	bool HasPool()
	{
		string szClassType = typeid(T).name();
		return (m_mapPool.find(szClassType) != m_mapPool.end());
	}

	template<class T>
	void SetPoolSize(_uint _size)
	{
		string szClassType = typeid(T).name();
		m_mapPoolSize[szClassType] = _size;
	}

private:
	virtual void Free();

private:
	map<string, IBasePool*> m_mapPool;
	map<string, _uint> m_mapPoolSize;

	const _uint m_iDefaultPoolSize = 5;
};

END