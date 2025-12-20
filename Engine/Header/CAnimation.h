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

	//리소스 확인용 Test 함수 
public:
	void ChangeNextAnimation();
	_uint currentState = 0;

public:
	void Change_Animation(const _uint _state);
	bool IsPlaying() { return m_bPlaying; }
	bool IsEnd() { return m_bEnd; }
	void Play();
	void PlayFromStart();
	void Pause();
	void Stop();

private:
	map<_uint, AnimationDesc*> m_mapAnimation;
	AnimationDesc* m_pCurAnimation;
	_matrix m_UVMatrix;

	//현재 애니메이션 텍스쳐의 특정 프레임의 인덱스 
	_vec2 m_vFrameIdx;
	_bool m_bPlaying;
	_bool m_bEnd;
	_float m_fTime;

};

END

