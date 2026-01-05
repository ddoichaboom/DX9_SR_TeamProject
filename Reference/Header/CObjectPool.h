#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)

class CGameObject;

class ENGINE_DLL IBasePool
{
public:
	virtual void Return_Object(CGameObject* pObj) PURE;
};

template <class T> 
class CObjectPool : public CBase, public IBasePool
{
protected:
	explicit CObjectPool(LPDIRECT3DDEVICE9 pGraphicDev, _uint _size)
		: m_pGraphicDev(pGraphicDev),m_iSize(_size)
	{
		m_pGraphicDev->AddRef();
	}
	virtual ~CObjectPool() {}

protected:
	HRESULT	Ready_ObjectPool()
	{
		m_vPool.reserve(m_iSize);
		for (_uint i = 0; i < m_iSize; i++)
		{
			T* obj = T::Create(m_pGraphicDev);
			if (!obj) 
			{
				MSG_BOX("Object Create Faield in Object Pool");
				return E_FAIL;
			}
			obj->SetPool(this);
			m_vPool.push_back(obj);
			m_qWaitingObj.push(obj);
		}
		return S_OK;
	}
public:
	//대기중인 비활성화 객체 반환 
	T* Get_FreeObject()
	{
		if (m_qWaitingObj.empty()) return nullptr;
		T* _obj = m_qWaitingObj.front(); 
		m_qWaitingObj.pop();

		_obj->Activate();
		return _obj;
	}
	//사용이 끝난 객체를 풀에 반납 
	void Return_Object(CGameObject* pObj) override
	{
		if (!pObj) return;
		T* _obj = static_cast<T*>(pObj);
		_obj->Deactivate();
		m_qWaitingObj.push(_obj);
	}

public:
	static CObjectPool<T>* Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint _size)
	{
		CObjectPool<T>* pObjectPool = new CObjectPool<T>(pGraphicDev, _size);
		if (FAILED(pObjectPool->Ready_ObjectPool()))
		{
			Safe_Release(pObjectPool);
			MSG_BOX("ObjectPool Create Failed");
			return nullptr;
		}
		return pObjectPool;
	}
	pair<typename vector<T*>::iterator, typename vector<T*>::iterator> GetObjectsRange()
	{
		return make_pair(m_vPool.begin(), m_vPool.end());
	}

protected:
	virtual void	Free()
	{
		for_each(m_vPool.begin(), m_vPool.end(), CDeleteObj());
		m_vPool = vector<T*>();
		m_qWaitingObj = queue<T*>();
	}

protected:
	LPDIRECT3DDEVICE9 m_pGraphicDev;

	vector<T*> m_vPool;
	queue<T*> m_qWaitingObj;
	_uint m_iSize = 0;
};

END
