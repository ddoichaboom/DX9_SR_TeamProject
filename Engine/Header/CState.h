#pragma once
#include "Engine_Define.h"
BEGIN(Engine)

class CGameObject;
class CBaseState
{
public:
	CBaseState() {}
	virtual ~CBaseState() {}
public:
	virtual void Begin(CGameObject* _owner) PURE;
	virtual void Update(CGameObject* _owner) PURE;
	virtual void End(CGameObject* _owner) PURE;
};

//T::* == T의 멤버 함수를 가리키는 포인터 타입
//보통 각 클래스의 멤버함수를 사용할테니 템플릿으로 정의
//StateComponent에서 템플릿 안쓰려면 BaseState가 필요함 
template<typename T>
class CState : public CBaseState
{
public:
	// 생성자에서 함수포인터와 바인딩 
	CState(void (T::*_BeginFunc)(), void (T::*_UpdateFunc)(), void (T::*_EndFunc)())
		: BeginFunc(_BeginFunc), UpdateFunc(_UpdateFunc), EndFunc(_EndFunc)	{}
	~CState() {}

public:
	void Begin(CGameObject* _owner) override
	{	
		if (_owner && BeginFunc) ((static_cast<T*>(_owner))->*BeginFunc)();
	}

	void Update(CGameObject* _owner) override
	{
		if (_owner && UpdateFunc) ((static_cast<T*>(_owner))->*UpdateFunc)();
	}

	void End(CGameObject* _owner) override
	{
		if (_owner && EndFunc) ((static_cast<T*>(_owner))->*EndFunc)();
	}
private:
	//함수포인터 
	void (T::*BeginFunc)();
	void (T::*UpdateFunc)();
	void (T::*EndFunc)();
};

END