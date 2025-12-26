#pragma once
#include "CComponent.h"
#include "CState.h"

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
	void ChangeState(CBaseState* _nextState);
	CBaseState* GetCurrentState() { return m_pCurState; };
private:
	virtual void Free();

private:
	CGameObject* m_pOwner;
	CBaseState* m_pCurState;
};

END
