#pragma once
#include "CComponent.h"
#include "Engine_Define.h"

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
	_int	Update_Component(const _float& fTimeDelta) override;
	virtual void	Render_Animation();
	//Texture 관련 설정 해제하기 위함 
	virtual void	LateRender_Animation(); 

public:
	static CAnimation* Create(LPDIRECT3DDEVICE9 pGraphicDev, CTexture* _pTextureComp, vector<AnimationSource>& _vAnimSource);
	static CAnimation* Create(LPDIRECT3DDEVICE9 pGraphicDev, CTexture* _pTextureComp, AnimationSource _AnimSource);
	virtual CComponent* Clone();

public:
	//매 프레임마다 현재 상태를 전달하면 상태값에 맞는 애니메이션으로 전환
	//+ Ratio설정에 따라 바로 전환이 안되게 막음 
	//오브젝트에서 매 프레임마다 호출하기! 
	void Update_State(_uint State);

public:
	void Change_Animation(const _uint _state);
	bool IsPlaying() { return m_bPlaying; }
	bool IsEnd() { return m_bEnd; }
	bool CanEnd() { return m_bCanEnd; }
	void Play();
	void PlayFromStart();
	void Pause();
	void Stop();

private:
	map<_uint, AnimationDesc*> m_mapAnimation;
	AnimationDesc* m_pCurAnimation;
	_matrix m_UVMatrix;

	_uint m_iCurState;
	//현재 플레이 되고있는 애니메이션의 프레임 인덱스
	_vec2 m_vFrameIdx;
	_bool m_bPlaying;
	_bool m_bEnd;
	_float m_fTime;
	_bool m_bCanEnd;


};

END

