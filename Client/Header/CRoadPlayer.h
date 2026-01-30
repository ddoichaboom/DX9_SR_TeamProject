#pragma once
#include "CCharacter.h"
#include "Engine_Enum.h"
#include "CEventMgr.h"

namespace Engine
{	
	class CCollider;
}

class CMinigun;

class CRoadPlayer : public CCharacter, public IListener
{
protected:
	explicit			CRoadPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit			CRoadPlayer(const CRoadPlayer& rhs);
	virtual				~CRoadPlayer();

public :
	static CRoadPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	void				OnEvent(EVENT_TYPE _type, EventData* _pData) override;

public:
	virtual	HRESULT		Ready_GameObject() override;
	virtual	_int		Update_GameObject(const _float& fTimeDelta) override;
	virtual	void		LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual	void		Render_GameObject() override;

protected:	
	virtual	HRESULT		Add_Component() override;
	virtual	void		Free();

	void				OnCollision(CollisionInfo info);

	void				Key_Input(const _float& fTimeDelta);

public:
	virtual	void		Activate() override;
	virtual	void		Deactivate() override;

	void				Shoot();

protected:
	Engine::CCollider* m_pMainCollider;
	const _tchar* m_szMainColliderName = L"ColMain";


protected :
	CMinigun* m_pMinigun;

	_float	  m_fMoveSpeed;
	_bool	  m_bStageEnd;
};

