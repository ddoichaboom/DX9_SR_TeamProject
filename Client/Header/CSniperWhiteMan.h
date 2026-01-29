#pragma once
#include "CMonster.h"

namespace Engine
{
	class CCollider;
}
class CBeam;

class CSniperWhiteMan : public CMonster
{
protected:
	explicit		CSniperWhiteMan(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit		CSniperWhiteMan(const CSniperWhiteMan& rhs);
	virtual			~CSniperWhiteMan();
public:
	//아래 정적 함수들은 캐릭터 클래스마다 정의하기 + 정적 배열 변수도 추가하기
	static void		CreateStateData();
	static vector<TextureSource>& GetTextureSources()
	{
		return m_vTextureSource;
	}
	static vector<AnimationSource>& GetAnimSources()
	{
		return m_vAnimSource;
	}

	static CSniperWhiteMan* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CSniperWhiteMan* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);


public:
	HRESULT			Ready_GameObject() override;
	_int			Update_GameObject(const _float& fTimeDelta) override;
	void			LateUpdate_GameObject(const _float& fTimeDelta) override;
	void			Render_GameObject() override;

protected:
	//ChangeState함수도 오브젝트마다 오버라이딩해서 구현해주세요!
	//템플릿이라서 직접 타입을 넣어줘야함 
	void			ChangeState(_uint nextStateID) override;
	HRESULT			Add_Component() override;
	void			UpdateBeam(const _float& fTimeDelta);
protected:
	virtual void	Free();
	void			OnBodyCollision(CollisionInfo info);

protected:
	//State Function 
	void			Idle();

	void			Begin_Attack();
	void			Idle_Attack();

	void			Shoot();
	void			Hit();
	void			Dead();

	void			OnAnimationChange(_float _animAspect);
public:
	void			Activate() override;
	//void			Deactivate() override;

protected:
	static vector<TextureSource> m_vTextureSource;
	static vector<AnimationSource> m_vAnimSource;

protected:
	_float			m_fAttackDelayTime = 3.0f;
	CCollider*		m_pBodyCollider;
	const _tchar*	m_szBodyColliderName = L"ColBody";

	_vec3			m_vPlayerPos{};
	_vec3			m_vMyPos{};

	CBeam*			m_pBeam;

	_float			m_fBeamDist = 100.f;
	_float			m_fBeamLen = 1000.f;
	_vec3			m_vBeamDestPos{};
	_vec3			m_vBeamDir{};

	_float			m_fBeamAngle = 0.f;
	_float			m_fRotSpeed = 1.f;

	_bool			m_bShoot = false;
	_bool			m_bTargeting = false;
	_bool			m_bTrace = false;
	_bool			m_bFirstFrame = false;
	_float			m_fLerpTime = 0.5f;
	_vec3			m_vHandPos = { -0.5f,6.5f,0.f };

	_float			m_fStartAngle = 0.f;
	_float			m_fEndAngle = 0.f;
	bool			m_bAngleReverse = false;

public:
	static wstring szWhiteManDead;
	static wstring szWhiteManBody;
	static wstring szWhiteManHead;
	static wstring szWhiteManShot;

};



