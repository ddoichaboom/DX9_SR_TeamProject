#pragma once
#include "CComponent.h"
#include "CState.h"
#include "CDataMgr.h"

BEGIN(Engine)
class CGameObject; 

class ENGINE_DLL CStateComponent :
    public CComponent
{
private:
	explicit CStateComponent(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CStateComponent(const CStateComponent& rhs);
	virtual ~CStateComponent();
public:
	static _uint MakeStateID(_uint _state, SUBSTATE _sub)
	{
		return ((_uint)_sub << 8) | (_uint)_state;
	}
public:
	_int Update_Component(const _float& fTimeDelta) override;
	static CStateComponent* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	CComponent* Clone() override;

public:
	void SetOnwer(CGameObject* _owner);
	CBaseState* GetCurrentState() { return m_pCurState; };
	_uint GetCurrentStateID() { return m_iCurStateID; };
	_uint GetPrevStateID() { return m_iPrevStateID; };

public:
	template<typename T>
	void ChangeState(_uint _nextStateID)
	{
		if (_nextStateID == m_iCurStateID || !m_pOwner) return;

		CBaseState* state = CDataMgr<T>::GetInstance()->GetState(_nextStateID);
		if (!state) return;

		if (m_pCurState)
		{
			m_pCurState->End(m_pOwner);
		}
		m_pCurState = state;
		m_iPrevStateID = m_iCurStateID;
		m_iCurStateID = _nextStateID;
		m_pCurState->Begin(m_pOwner);
	}

	void Reset() override;
private:
	virtual void Free();

private:
	CGameObject* m_pOwner;
	CBaseState* m_pCurState;
	_uint m_iCurStateID;
	_uint m_iPrevStateID;
};

END
