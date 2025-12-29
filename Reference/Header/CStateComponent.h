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
	_int Update_Component(const _float& fTimeDelta) override;
	static CStateComponent* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	CComponent* Clone() override;

public:
	void SetOnwer(CGameObject* _owner);
	CBaseState* GetCurrentState() { return m_pCurState; };
	_uint GetCurrentStateID() { return m_iCurStateID; };

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
		m_pCurState->Begin(m_pOwner);
		m_iCurStateID = _nextStateID;
	}

private:
	virtual void Free();

private:
	CGameObject* m_pOwner;
	CBaseState* m_pCurState;
	_uint m_iCurStateID;
};

END
