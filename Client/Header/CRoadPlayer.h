#pragma once
#include "CCharacter.h"
#include "Engine_Enum.h"

namespace Engine
{	
	class CCollider;
}

class CMinigun;

class CRoadPlayer : public CCharacter   
{
protected:
	explicit			CRoadPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit			CRoadPlayer(const CRoadPlayer& rhs);
	virtual				~CRoadPlayer();

public :
	static CRoadPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual	HRESULT		Ready_GameObject() override;
	virtual	_int		Update_GameObject(const _float& fTimeDelta) override;
	virtual	void		LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual	void		Render_GameObject() override;

protected:	
	virtual	HRESULT		Add_Component() override;
	virtual	void		Free();

	void				OnCollision(CollisionInfo info);

public:
	virtual	void		Activate() override;
	virtual	void		Deactivate() override;

protected:
	Engine::CCollider* m_pMainCollider;
	const _tchar* m_szMainColliderName = L"ColMain";


protected :
	CMinigun* m_pMinigun;
};

