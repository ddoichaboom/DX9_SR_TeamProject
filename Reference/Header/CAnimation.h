#pragma once
#include "CComponent.h"
#include "Engine_Define.h"
#include <deque>

#pragma region 참고사항
/* 
* AnimationDesc::Ratio (Default = 0.f)
	- animDesc 의 ratio는 디폴트가 0
	- 종료할 프레임의 비율을 나타냄. 0.8이면 80퍼까지 진행 후 m_bCanEnd = true처리 

* Update_State 
	- 들어온 상태값에 맞게 Animation Deque에 보관하는 함수 
	- 텍스쳐/애니메이션에서 MakeStateID(_State, SubState) 로 
	  특정 _state의 Begin,End 버전 텍스쳐/애니메이션을 삽입 가능 
	- State가 전환되면 자동으로 기존 _state의 End버전 애니메이션과 
		새 _state의 Begin, 기본 State를 deque에 삽입한다.

+ AnimationComp는 현재 플레이 애니메이션의 SubState(BEGIN, NONE 기본 ,END) 정보를 저장함. 
	이 함수로 if(substate!=SUB_BEGIN)  Begin애니메이션이 끝나고 처리할 작업~ 이런식으로 구현 

* PlayOnce
	- StateComp->ChangeState를 하지않고 (현재 state값을 바꾸지않고) 
	단 한번만 애니메이션을 출력할때사용
	StateComp에서 state를 전환하고 Animaiton에서 Update_state를 하면 
	Begin과 End가 무조건 실행됨.
	- 기존 애니메이션을 무시하고 특정 애니메이션을 한번만 플레이 할 때 사용하기 
	(기존 애니메이션은 뒤로 미뤄짐)
	

*PlayNextAnim 
	dequq에 있는 다음 애니메이션 출력 

+ 한 상태의 애니메이션 전후로 특정 애니메이션이 플레이되어야한다면 
	텍스쳐,애니메이션Source 단계에서 MakeStateID로 추가해두면 자동으로 나옵니다. 

+ 자동으로 나오는게 불편한 경우가 생기는데 이때는 Make를 하지말고 
	State에 바인딩 해둔 Begin,End 함수에서 조건을 줘서 PlayOnce를 하게하면 됩니다.  

+ 주의 할 점!!!!은 StateComp::changeState에서 기존 상태 EndFunc호출-> 새 상태 BeginFunc을 한 프레임에서 하기때문에 
	PlayOnce를 둘 다 호출하면 이상이 생김. ENd와 Begin함수가 동시에 실행되는점 유의하기!

*/
#pragma endregion


BEGIN(Engine)

class CTexture;
class ENGINE_DLL CAnimation : public CComponent
{
private:
	explicit CAnimation();
	explicit CAnimation(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAnimation(const CAnimation& rhs);
	virtual ~CAnimation();

private:
	virtual void Free();

private:
	AnimationDesc* MakeAnimationDesc(CTexture* _pTextureComp, AnimationSource& AnimSource);

public:
	virtual HRESULT Ready_Animation(CTexture* _pTextureComp,vector<AnimationSource> & _vAnimSource);
	virtual HRESULT Ready_Animation(CTexture* _pTextureComp, AnimationSource _AnimSource);
	_int			Update_Component(const _float& fTimeDelta) override;
	virtual void	Render_Animation();
	//Texture 관련 설정 해제하기 위함 
	virtual void	LateRender_Animation(); 

public:
	static CAnimation* Create(LPDIRECT3DDEVICE9 pGraphicDev, CTexture* _pTextureComp, vector<AnimationSource>& _vAnimSource);
	static CAnimation* Create(LPDIRECT3DDEVICE9 pGraphicDev, CTexture* _pTextureComp, AnimationSource _AnimSource);
	virtual CComponent* Clone();

public:
	void Update_State(const _uint _state);
	void PlayOnce(const _uint _state);
	void PlayNextAnim();

public:
	bool IsPlaying() { return m_bPlaying; }
	bool IsEnd() { return m_bEnd; }
	bool CanEnd() { return m_bCanEnd; }
	void Play();
	void PlayFromStart();
	void Pause();
	void Stop();

public:
	void ResetDeque();
	SUBSTATE GetSubState()
	{
		return m_CurAnimTask.subState;
	}

private:
	void Change_Animation(AnimTask& animTask);
public:
	//이전 버전 호환용
	void Change_Animation(_uint _state);
private:
	map<_uint, AnimationDesc*> m_mapAnimation;
	AnimTask m_CurAnimTask;
	_matrix m_UVMatrix;

	_uint m_iCurState;
	//현재 플레이 되고있는 애니메이션의 프레임 인덱스
	_vec2 m_vFrameIdx;
	_bool m_bPlaying;
	_bool m_bEnd;
	_float m_fTime;
	_bool m_bCanEnd;

private:
	deque<AnimTask> m_AnimDeq;

};

END

