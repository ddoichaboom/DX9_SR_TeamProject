#pragma once
#include "CPlayerPart.h"

class CPlayer;

class CRightPart : public CPlayerPart
{
protected:
	explicit		CRightPart(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit		CRightPart(const CRightPart& rhs);
	virtual			~CRightPart();

public:
	static void		CreateStateData();
	static vector<TextureSource>& GetTextureSources()
	{
		return m_vTextureSource;
	}
	static vector<AnimationSource>& GetAnimSources()
	{
		return m_vAnimSource;
	}

	static CRightPart* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	virtual _bool	Get_ActionAble() override;

public:
	HRESULT		Ready_GameObject() override;
	_int		Update_GameObject(const _float& fTimeDelta) override;
	void		LateUpdate_GameObject(const _float& fTimeDelta) override;
	void		Render_GameObject() override;

protected:
	HRESULT		Add_Component() override;
	virtual void Free();

public:
	void		ChangeState(_uint nextStateID) override;

protected:
	void Begin_Idle();
	void Idle();


	void Begin_Attack();
	void Attack();
	void End_Attack();

	void Begin_Reload();
	void Reload();
	void End_Reload();

	void Begin_Intro();
	void Intro();
	void End_Intro();

protected:
	static vector<TextureSource>	m_vTextureSource;
	static vector<AnimationSource>	m_vAnimSource;
	_vec3	m_vStartPos;
	_vec3	m_vEndPos;

	_bool	m_bReload;
	_bool	m_bAttack;

};
