#pragma once
#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)
#pragma region 참고사항
/*
* Get_FreeObject() : 대기중인 비활성화 객체 반환
	_obj->Activate(); 호출
	활성화에서 해야할 작업이 있다면 object::Activate을 재정의

* Return_Object() : 사용이 끝난 객체를 풀에 반납
	풀에서 생성된 객체는 IBasePool* 타입 멤버변수에 본인이 속한 Pool의 주소를 들고있음(CGameObject::m_pPool)
	이 값에 따라 Dead일때 Pool의 주소가 있다면 풀에 오브젝트 스스로 반납함
	_obj->deactivaet마찬가지

+ CGameObject::Activate, Deactivate 확인하기
+ 진행 방식
	Stage 에서 Pool 생성하기. 반드시 컴포넌트 생성 후 Pool 생성하기
	pool에서 객체를 꺼내 Layer에 AddObject
	Dead시 자동으로 Pool에 반납됨
	+ 스테이지 종류 시 Layer에 있던 풀 원소는 모두 자동으로 반납되고, Pool에서 모든 원소 메모리 해제

+ 비활성화된 오브젝트가 없을시 새로 생성하여 반환하도록 수정함
*/
#pragma endregion

class CGameObject;
//
class ENGINE_DLL IBasePool : public CBase
{
public:
	virtual void Return_Object(CGameObject* pObj) PURE;
protected:
	virtual void	Free() PURE;
};


template <class T>
class CObjectPool : public IBasePool
{
protected:
	explicit CObjectPool(LPDIRECT3DDEVICE9 pGraphicDev, _uint _size)
		: m_pGraphicDev(pGraphicDev), m_iSize(_size)
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
			if (FAILED(PushNewObject()))
				return E_FAIL;
		}
		return S_OK;
	}
public:
	//대기중인 비활성화 객체 반환 
	T* Get_FreeObject()
	{
		//큐가 비어있다면 오브젝트 새로 추가하기
		if (m_qWaitingObj.empty())
		{
			if (FAILED(PushNewObject()))
				return nullptr;
		}

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
	HRESULT PushNewObject()
	{
		T* obj = T::Create(m_pGraphicDev);
		if (!obj)
		{
			MSG_BOX("Object Create Faield in Object Pool");
			return E_FAIL;
		}
		obj->SetPool(this); //현재 풀의 주소를 오브젝트가 소유 
		m_vPool.push_back(obj);
		m_qWaitingObj.push(obj);
		return S_OK;
	}

	virtual void	Free()
	{
		for_each(m_vPool.begin(), m_vPool.end(), CDeleteObj());
		m_vPool = vector<T*>();
		m_qWaitingObj = queue<T*>();
	}

protected:
	LPDIRECT3DDEVICE9 m_pGraphicDev;
	//Pool의 전체 원소
	vector<T*> m_vPool;
	//비활성화되어 대기중인 원소들 
	queue<T*> m_qWaitingObj;
	_uint m_iSize = 0;
};

END
