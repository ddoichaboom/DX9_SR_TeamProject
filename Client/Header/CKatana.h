#pragma once
#include "CWeapon.h"
#include "Engine_Define.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CAnimation;
	class CStateComponent;
}

class CPlayerPart;
class CTrail;

class CKatana : public CWeapon    
{
private :
	enum KATANA_COMBO : _byte
	{
		COMBO_NONE,
		COMBO_1,
		COMBO_2,
		COMBO_3,
		COMBO_END
	};


protected:
	explicit		CKatana(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit		CKatana(const CKatana& rhs);
	virtual			~CKatana();

public :
	virtual		HRESULT		Ready_GameObject();
	virtual		_int		Update_GameObject(const _float& fTimeDelta);
	virtual		void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual		void		Render_GameObject();
	
public:
	virtual		void		ChangeState(_uint nextStateID);
	virtual		_bool		Can_Fire() override;
	virtual		void		Fire() override;
	virtual		void		Reload() override;
	
public:
	static		CKatana*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual		HRESULT		Add_Component();
	virtual		void		Free();

public:
	static _uint GetStateID(_byte _state, _byte _subState)
	{
		return ((_uint)_subState << 4) | (_uint)_state;
	}
	static void		CreateStateData();
	static vector<TextureSource>& GetTextureSources()
	{
		return m_vTextureSource;
	}
	static vector<AnimationSource>& GetAnimSources()
	{
		return m_vAnimSource;
	}

protected:
	//State Function 
	void Begin_Idle();
	void Idle();

	void Begin_Intro();
	void Intro();
	void End_Intro();

	void Begin_Attack1();
	void Begin_Attack2();
	void Begin_Attack3();

	void Attack();

	void End_Attack();

	void Start_Combo();

protected:
	static vector<TextureSource>	m_vTextureSource;
	static vector<AnimationSource>	m_vAnimSource;
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CAnimation* m_pAnimationCom;
	Engine::CStateComponent* m_pStateCom;

private :
	_float		m_fAniTime;
	_float		m_fDelayTime;
	_bool		m_bDelay;
	_vec3		m_vStartPos;
	_vec3		m_vEndPos;
	_vec3		m_vConvertScale;

	KATANA_COMBO	m_eCombo;
	_bool			m_bCanCombo;
	_bool			m_bComboBuffered;

	_float m_fAniSpeed;
	//πÊΩ¬»Ò ¿Ã∆Â∆Æ √ﬂ∞° 
	CTrail* m_pTrail = nullptr;

public:
	static wstring  szKatanaIntroSFX;
	static wstring  szKatanaAttackSFX;
	
};

