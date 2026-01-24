#pragma once
#include "CInteractObject.h"

namespace Engine
{
	class CCollider;
}

class CSoda : public CInteractObject    
{
protected:
	explicit						CSoda(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit						CSoda(const CSoda& rhs);
	virtual							~CSoda();

public :
	static		CSoda*				Create(PDIRECT3DDEVICE9 pGraphicDev);	
	static		TextureSource&		GetTextureSources(){ return m_vTextureSource; }
	

public:
	virtual		HRESULT				Ready_GameObject() override;
	virtual		_int				Update_GameObject(const _float& fTimeDelta) override;
	virtual		void				LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual		void				Render_GameObject() override;

protected:
	virtual		HRESULT				Add_Component() override;
	virtual		void				Free() override;

public :
	virtual		void				Activate() override;
	virtual		void				Deactivate() override;	

protected :
	void			OnCollision(CollisionInfo info);

	void				Gravity(const _float& fTimeDelta);
	_bool				CheckOnFloor(const _float& fTimeDelta, _float* pHeight);
	void				Set_OnFloor(const _float& fTimeDelta);
	void				Update_Jump(const _float& fTimeDelta);

public :
	void				Set_JumpDir();

protected:
	static TextureSource    m_vTextureSource;

	Engine::CCollider* m_pMainCollider;
	const _tchar* m_szMainColliderName = L"ColMain";

	_bool	m_bJump;
	_bool	m_bFall;
	_bool   m_bGround;
	_float	m_fVelocity;


	_vec3	m_vJumpStartPos;
	_vec3	m_vJumpDir;

	_float	m_fJumpTime;
	_float	m_fJumpDuration;
	_float	m_fJumpHeight;
};

