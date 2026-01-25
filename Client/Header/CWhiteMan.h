#pragma once
#include "CMonster.h"
#include "CObjectPool.h"
#pragma region 참고
/*  
1. CreateStateData 
	- DataMgr에 State 객체 삽입.
	- 현 클래스의 함수를 State에 바인딩하기 위해 함수 포인터 넣어주기 
	- CState<클래스타입>* State = new CState<클래스타입>(state 시작할때 호출될 함수 주소, Run일때 호출될함수, 끝날때 호출될 함수);
	- Mgr->AddState(상태값, State포인터) 
	- 상태값은 캐릭터마다 넣으시고 SubState를 이용하는 방법은 Monster를 참고해주세요

2.Ready_GameObject
	- Collider 생성 
		Collision컴포넌트 내부에 Collider 객체가 생성되고 
		그 객체를 가져와서 Transform 변경과 이 콜라이더가 충돌했을때 호출될 캐릭터 함수를 바인딩함 

3. LateUpdate_GameObject
	- 애니메이션 컴포넌트 Update_State 
	매 프레임마다 현재 상태를 업데이트해서 해당 상태에 맞는 애니메이션을 자동 변환 
	애니메이션 내에서 같은 상태값이 들어오면 변환x 

	+Ratio이 설정됐는데 아직 ratio가 지나지않은 애니메이션이 플레이 중이였다면 새 상태값이 들어와도 변환하지 않으므로 
	매프레임마다 새 상태값을 넣어줘야 이전 애니메이션이 끝나면 변환됨 


따로 Animation 변환 호출하지않고 상태값을 세분화해서 
ChangeState(상태) 이 함수만 호출해주시면 됩니다. 

예를들어
CState를 MS_BEGINATTACK / MS_ATTACK / MS_ENDATTACK 상태 별로 State를 만들고 
BeingAttack용 State의 Update쪽에 바인딩 된 함수에 
	ChangeState(MS_ATTACK) 
	or
	if(m_pAnimationCom->CanEnd()) ChangeState(MS_ATTACK) 
	위는(if버전) 선택사항! 이거 없어도 Ratio 설정된 애니메이션이면 끝나야 넘어가요 
	참고로 Ratio 설정은 AnimaionDesc 쪽에서 입력해주시면 됩니다. 디폴트는 0.f == 바로 넘어감 

Attack용 State의 Update쪽 함수에서는 
	if(마우스버튼이 Up) ChangeState(MS_ENDATTACK) 

EndAttack용 State에서는 
	Update 에서는 ChangeState(MS_IDLE) 
	End 바인딩된 함수에서는 필요하면.. Attack이 끝났을 때 초기화해야할 변수 등 넣는 식 

이렇게 하면됩니다... 자동화 하려면 State세분화밖에 없는듯

*/
#pragma endregion

namespace Engine
{
	class CCollider;
}

class CBullet;
class CWhiteMan :
	public CMonster
{
protected:
	enum { DEST_NONE, DEST_GROUND, DEST_WALL, DEST_END};
protected:
	explicit		CWhiteMan(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit		CWhiteMan(const CWhiteMan& rhs);
	virtual			~CWhiteMan();
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

	static CWhiteMan* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CWhiteMan* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);


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

protected:
	virtual void	Free();
	void			OnHeadCollision(CollisionInfo info);
	void			OnBodyCollision(CollisionInfo info);

	void			CreateBloodAndExp();
protected:
	//State Function 
	void			Idle();

	void			Begin_Attack();
	void			Idle_Attack();
	void			End_Attack();

	void			Shoot();
	void			Hit();
	void			Launch() override;
	void			Dead();

	void			Explore();
	void			Elect();
	void			Slice();

	void			FlyBack_Begin();
	void			FlyBack();
	void			Fly_BlockWall();
	void			Fly_FallGournd();

	void			OnAnimationChange(_float _animAspect);
public:
	void			Activate() override;
	//void			Deactivate() override;

protected:
	static vector<TextureSource> m_vTextureSource;
	static vector<AnimationSource> m_vAnimSource;

protected:
	_float			m_fAttackDelayTime = 3.0f;

	//_float			m_fFlyBackTime = 0.5f;
	_float			m_fFlyBackTime = 1.f;
	_float			m_fFlyBackSpeed= 4.f;
	_vec3			m_FlyDir = {};
	_float			m_fHeadPosOffset = -5.f;
	CCollider*		m_pHeadCollider;
	const _tchar*	m_szHeadColliderName = L"ColHead";
	
	CCollider*		m_pBodyCollider;
	const _tchar*	m_szBodyColliderName = L"ColBody";

	static _uint ID_SLICE_DEAD;
	static _uint ID_ELECT_DEAD;
	static _uint ID_HEAD_DEAD;
	static _uint ID_EXP_DEAD;

	static _uint ID_FLYBACK_BEGIN;
	static _uint ID_FLYBACK_END_WALL;
	static _uint ID_FLYBACK_END_GROUND;
	
	bool m_bLaunchEnd = false;

public :
	static wstring szWhiteManDead;
	static wstring szWhiteManBody;
	static wstring szWhiteManHead;
	static wstring szWhiteManShot;

};

