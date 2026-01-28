#pragma once
#include "CGameObject.h"
#include "Engine_Define.h"


namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;	
	class CStateComponent;
	class CAnimation;
}

class CRoadPlayer;
class CPannel;
class CChain;

class CMinigun : public CGameObject    
{
protected:
	enum MINIGUN_STATE : _byte
	{
		MS_IDLE			= 0,
		MS_START		= 1,
		MS_CYCLE		= 2,
		MS_ATTACK		= 3,
		MS_ATTACK_END	= 4
	};

protected:
	explicit			CMinigun(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit			CMinigun(const CMinigun& rhs);
	virtual				~CMinigun();

public:
	static CMinigun* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static void		CreateStateData();
	static vector<TextureSource>& GetTextureSources()
	{
		return m_vTextureSource;
	}
	static vector<AnimationSource>& GetAnimSources()
	{
		return m_vAnimSource;
	}

public:
	HRESULT				Ready_GameObject() override;
	_int				Update_GameObject(const _float& fTimeDelta) override;
	void				LateUpdate_GameObject(const _float& fTimeDelta) override;
	void				Render_GameObject() override;

	virtual	void		Activate() override;
	virtual	void		Deactivate() override;

protected:
	virtual	void		ChangeState(_uint nextStateID);
	virtual	HRESULT		Add_Component() override;
	virtual	void		Free();


	void				Key_Input(const _float& fTimeDelta);


protected:
	void				Begin_Idle();
	void				Idle();
	void				End_Idle();

	void				Begin_Start();
	void				Start();
	void				End_Start();

	void				Begin_Cycle();
	void				Cycle();
	void				End_Cycle();

	void				Begin_Attack();
	void				Attack();
	void				End_Attack();


	void				Begin_Attack_End();
	void				Attack_End();
	void				End_Attack_End();

protected:
	virtual		void    Rotate(ROTATION eType, const _float& fAngle);
	virtual		void    SetPos(_vec3 _pos);
	virtual		void	SetScale(_vec3 _scale);

public :
	void				Set_Parent(CRoadPlayer* pPlayer);

protected:
	static vector<TextureSource>	m_vTextureSource;
	static vector<AnimationSource>	m_vAnimSource;
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CStateComponent* m_pStateCom;
	Engine::CAnimation* m_pAnimationCom;

	CRoadPlayer* m_pParent;
	CPannel*	m_pPannel;
	CChain*		m_pChain;

	_float	m_fTime = 0.f;
	_vec3	m_vPosition;
	_vec3	m_vScale;

	_bool	m_bKeyPressing;

private :
	const wstring m_szMinigunLoop = L"Minigun_Loop_BGM.wav";
	const wstring m_szMinigunCycle = L"Minigun_Cycle_BGM.wav";
	const wstring m_szLoopStart = L"Minigun_Start_SFX.wav";
	const wstring m_szLoopEnd = L"Minigun_LoopEnd_SFX.wav";
	const wstring m_szFastEnd = L"Minigun_LoopEndFast_SFX.wav";
	
};

