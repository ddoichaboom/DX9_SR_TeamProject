#pragma once
#include "CPlayerPart.h"
class CPlayer;
class CMiddlePart : public CPlayerPart
{
protected:
	explicit		CMiddlePart(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit		CMiddlePart(const CMiddlePart& rhs);
	virtual			~CMiddlePart();

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

	static CMiddlePart* Create(LPDIRECT3DDEVICE9 pGraphicDev);

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

	void Begin_Kick();
	void Kick();
	void End_Kick();

	void Begin_Drink();
	void Drink();
	void End_Drink();

	void Begin_Slide();
	void Slide();
	void End_Slide();

	void Begin_Intro();
	void Intro();

	void Begin_Intro2();
	void Intro2();

	void Begin_Shop();
	void Shopping();
	void End_Shop();

protected:
	static vector<TextureSource>	m_vTextureSource;
	static vector<AnimationSource>	m_vAnimSource;
	_vec3	m_vStartPos;
	_vec3	m_vEndPos;

	_vec3	m_vStartScale;
	_vec3	m_vEndScale;

	_float	m_fX;
	_float	m_fY;
	_float  m_fSizeX;
	_float  m_fSizeY;

	_int	m_iLoopTime;

	_float  m_fDelayTime;
	_bool	m_bDelay;

	_bool	m_bStateStop;
};