#include "CAnimation.h"
#include "CTexture.h"
#include "CStateComponent.h"


CAnimation::CAnimation()
	:m_iCurState(-1), m_vFrameIdx{0,0}, m_bPlaying(false), 
	m_bEnd(false), m_fTime(0.f), m_bCanEnd(true), m_CurAnimTask({SUB_MAX,NULL})
{
	D3DXMatrixIdentity(&m_UVMatrix);
}

CAnimation::CAnimation(LPDIRECT3DDEVICE9 pGraphicDev)
	:CComponent(pGraphicDev), m_iCurState(-1), m_vFrameIdx{ 0,0 },
	m_bPlaying(false), m_bEnd(false),m_fTime(0.f), m_bCanEnd(true), m_CurAnimTask({ SUB_MAX,NULL })
{
	D3DXMatrixIdentity(&m_UVMatrix);
}

CAnimation::CAnimation(const CAnimation& rhs)
	:CComponent(rhs),m_iCurState(-1), m_vFrameIdx{ 0,0 },
	m_bPlaying(false), m_bEnd(false), m_fTime(0.f), m_bCanEnd(true), m_CurAnimTask({ SUB_MAX,NULL })
{
	//TODO : 얕은 복사 주의
	m_mapAnimation.insert(rhs.m_mapAnimation.begin(), rhs.m_mapAnimation.end());
}

CAnimation::~CAnimation()
{
}

void CAnimation::Free()
{
	CComponent::Free();
	if (!m_bClone)
	{
		for (auto& mp : m_mapAnimation)
		{
			Safe_Delete(mp.second);
		}
	}
	m_mapAnimation.clear();
}

AnimationDesc* CAnimation::MakeAnimationDesc(CTexture* _pTextureComp, AnimationSource& AnimSource)
{
	AnimationDesc* animDesc = new AnimationDesc;
	if (!animDesc) return nullptr;

	//상태에 대응하는 텍스쳐를 얻어옴
	animDesc->pTextureDesc = _pTextureComp->GetTextureDesc(AnimSource.uState);
	if (!animDesc->pTextureDesc)
	{
		Safe_Delete(animDesc);
		return nullptr;
	}

	animDesc->fPlayTime = AnimSource.fPlayTime;
	animDesc->bLoop = AnimSource.bLoop;
	//순서 주의 (열, 행) 순 -> x,y 에 대응하기 위함 
	animDesc->vMaxIdx = { AnimSource.fMaxCol, AnimSource.fMaxRow }; // 마지막 행,열 인덱스 번호
	animDesc->fEndFrameCol = AnimSource.fEndFrameCol; //마지막 프레임의 열 번호

	IDirect3DBaseTexture9* BaseTexture = animDesc->pTextureDesc->pTexture;
	IDirect3DTexture9* texture = NULL;

	//COM객체를 형변환 하는 방법
	if (FAILED(BaseTexture->QueryInterface(IID_IDirect3DTexture9, (void**)(&texture))))
	{
		Safe_Delete(animDesc);
		MSG_BOX("[CAnimation] Invalid Texture");
		return nullptr;
	}

	//텍스쳐 원본 크기 
	const _vec2& originSize = animDesc->pTextureDesc->vOriginSize;
	//프레임 한 컷 당 너비, 높이 
	_float cutWitdh = originSize.x / (animDesc->vMaxIdx.x + 1);
	_float cutHeight = originSize.y / (animDesc->vMaxIdx.y + 1);

	//desc의 width,height 정보는 GPU가 밉맵에 맞게 생성한 텍스쳐의 크기 (원본가 같거나 다를수도 있음)
	//생성된 텍스쳐에 맞게 한 프레임당 uv 비율 구하기 
	D3DSURFACE_DESC desc;
	texture->GetLevelDesc(0, &desc);

	animDesc->vUVoffset = { cutWitdh / desc.Width, cutHeight / desc.Height };
	//전체 프레임 크기. 이것을 기준으로 현재의 Ratio를 계산함 
	animDesc->fTotalFrame = (animDesc->fEndFrameCol+1)* (animDesc->vMaxIdx.y+1);
	//애니메이션이 종료 가능한 Ratio 비율 
	animDesc->fEndRatio = AnimSource.fEndRatio;
	animDesc->bPriority = AnimSource.bPriority;

	return animDesc;
}


HRESULT CAnimation::Ready_Animation(CTexture* _pTextureComp, vector<AnimationSource>& _vAnimSource)
{
	if (_pTextureComp == nullptr) return E_FAIL;

	for (auto& source : _vAnimSource)
	{
		AnimationDesc* desc = MakeAnimationDesc(_pTextureComp, source);
		if(desc) m_mapAnimation.insert({ source.uState, desc });
	}

	return S_OK;
}

HRESULT CAnimation::Ready_Animation(CTexture* _pTextureComp, AnimationSource _AnimSource)
{
	if (_pTextureComp == nullptr) return E_FAIL;

	AnimationDesc* desc = MakeAnimationDesc(_pTextureComp, _AnimSource);
	if (desc) m_mapAnimation.insert({ _AnimSource.uState, desc });
	return S_OK;
}


//순서 주의 x = x 축 y = y축  
_int CAnimation::Update_Component(const _float& fTimeDelta)
{
	if (!m_CurAnimTask.animDesc || !m_bPlaying || m_bEnd) return 0 ;
	auto pCurAnimation = m_CurAnimTask.animDesc;
	if (pCurAnimation->vMaxIdx ==_vec2(0,0)) return 0; // 한 장 

	m_fTime += fTimeDelta;

	if (m_fTime >= pCurAnimation->fPlayTime)
	{
		m_fTime = 0.f;

		//지난 프레임이 마지막 이미지 였다면 
		if (m_vFrameIdx == _vec2(pCurAnimation->fEndFrameCol, pCurAnimation->vMaxIdx.y))
		{
			if (pCurAnimation->bLoop)
			{
				m_vFrameIdx = { 0.f, 0.f };
			}
			else
			{
				m_bEnd = true;
				if (!m_AnimDeq.empty())
				{
					PlayNextAnim();
					return 0 ;
				}
			}
		}
		else
		{
			m_vFrameIdx.x += 1.f;
			if (m_vFrameIdx.x > pCurAnimation->vMaxIdx.x) // 마지막 열 도달 
			{
				m_vFrameIdx.x = 0.f;
				m_vFrameIdx.y += 1.f;
				//마지막 행 검사는 위의 End인덱스 체크에서 수행됨 
			}
			
			_float curRatio = ((m_vFrameIdx.x+1) *(m_vFrameIdx.y+1)) / pCurAnimation->fTotalFrame;
			if (curRatio >= pCurAnimation->fEndRatio) m_bCanEnd = true;
			else m_bCanEnd = false;
		}

	}
	//한 프레임의 uv크기로 축소 + 해당 프레임의 uv위치로 이동한다
	// == 한 프레임의텍스쳐만 보여짐
	D3DXMatrixIdentity(&m_UVMatrix);
	m_UVMatrix._11 = pCurAnimation->vUVoffset.x;
	m_UVMatrix._22 = pCurAnimation->vUVoffset.y;
	m_UVMatrix._31 = m_vFrameIdx.x * pCurAnimation->vUVoffset.x;
	m_UVMatrix._32 = m_vFrameIdx.y * pCurAnimation->vUVoffset.y;

	return 0;
}

void CAnimation::Render_Animation()
{
	if (!m_CurAnimTask.animDesc) return;
	//행렬변환 결과 중 앞에서 두 개의 요소만 적용한다 = 2차원 텍스쳐로 쓰겠다
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
	//텍스쳐 UV 공간을 행렬 변환한다 
	m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &m_UVMatrix);
	m_pGraphicDev->SetTexture(0, m_CurAnimTask.animDesc->pTextureDesc->pTexture);
	
}

void CAnimation::LateRender_Animation()
{
	//텍스쳐 공간을 변환하지 않게 함. disable로 돌려줘야함
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
}

CAnimation* CAnimation::Create(LPDIRECT3DDEVICE9 pGraphicDev, CTexture* _pTextureComp, vector<AnimationSource>& _vAnimSource)
{
	CAnimation* pAnimation = new CAnimation(pGraphicDev);

	if (FAILED(pAnimation->Ready_Animation(_pTextureComp, _vAnimSource)))
	{
		Safe_Release(pAnimation);
		MSG_BOX("Animation Create Failed");
		return nullptr;
	}

	return pAnimation;
}

CAnimation* CAnimation::Create(LPDIRECT3DDEVICE9 pGraphicDev, CTexture* _pTextureComp, AnimationSource _AnimSource)
{
	CAnimation* pAnimation = new CAnimation(pGraphicDev);

	if (FAILED(pAnimation->Ready_Animation(_pTextureComp, _AnimSource)))
	{
		Safe_Release(pAnimation);
		MSG_BOX("Animation Create Failed");
		return nullptr;
	}

	return pAnimation;
}

CComponent* CAnimation::Clone()
{
	return new CAnimation(*this);
}


void CAnimation::Update_State(const _uint _state)
{
	if (m_iCurState == _state) return;
	AnimationDesc* nextAnim = m_mapAnimation[_state];
	if (nextAnim == nullptr) return;
	//상태를 바꿀 수 없는 상황이라면 종료 (우선순위가 높거나, 현 애니메이션이 종료 가능이여야 진행) 
	if (nextAnim->bPriority == false && CanEnd() == false) return; 

	//현 State의 End 에 대응하는 애니메이션 
	_uint endID = CStateComponent::MakeStateID(m_iCurState, SUB_END);
	AnimationDesc* curAnimEnd = m_mapAnimation[endID];
	
	//새 State의 Begin, 기본 상태(None)에 대응하는 애니메이션 
	_uint nextAnimBeginID = CStateComponent::MakeStateID(_state, SUB_BEGIN);
	AnimationDesc* nextAnimBegin = m_mapAnimation[nextAnimBeginID];

	ResetDeque();
	if (nextAnim->bPriority) //우선순위가 설정된 애니메이션이 들어오면 기존 애니메이션은 아예 비움 
	{
		if (nextAnimBegin) m_AnimDeq.push_back({ SUB_BEGIN, nextAnimBegin });
		if (nextAnim) m_AnimDeq.push_back({ SUB_NONE, nextAnim });
	}
	else // 그 외의 경우 기존 애니메이션의 End용 애니메이션을 넣은 뒤 다음 애니메이션 Begin,Update 넣음 
	{
		if(curAnimEnd) m_AnimDeq.push_back({ SUB_END, curAnimEnd });
		if (nextAnimBegin) m_AnimDeq.push_back({ SUB_BEGIN, nextAnimBegin });
		if (nextAnim) m_AnimDeq.push_back({ SUB_NONE, nextAnim });
	}

	//End 애니메이션은 기존의 나머지 처리일뿐 state가 갱신됨. State를 변경해주기 
	m_iCurState = _state;
	PlayNextAnim();
}

//현재 애니메이션 상태를 유지하면서 한번만 출력될 애니메이션
//기존의 상태를 유지함
void CAnimation::PlayOnce(const _uint _state)
{
	auto iter = m_mapAnimation.find(_state);
	if (iter == m_mapAnimation.end() || iter->second == m_CurAnimTask.animDesc) return;

	//현재 애니메이션 다시 넣기. 상태 갱신 하지않음 
	m_AnimDeq.push_front(m_CurAnimTask);
	AnimTask task= {SUB_NONE, iter->second };
	Change_Animation(task);
}

void CAnimation::PlayNextAnim()
{
	if (m_AnimDeq.empty()) return;

	AnimTask task = m_AnimDeq.front(); m_AnimDeq.pop_front();
	Change_Animation(task);
}


void CAnimation::ResetDeque()
{
	m_AnimDeq = deque<AnimTask>();
}

void CAnimation::Change_Animation(AnimTask& animTask)
{
	m_CurAnimTask = animTask;
	PlayFromStart();
	D3DXMatrixScaling(&m_UVMatrix, m_CurAnimTask.animDesc->vUVoffset.x, m_CurAnimTask.animDesc->vUVoffset.y, 1.0f);

}

void CAnimation::Change_Animation(_uint _state)
{
	auto desc = m_mapAnimation[_state];
	if (desc)
	{
		ResetDeque();
		m_iCurState = _state;
		m_CurAnimTask = { SUB_NONE, desc };
		PlayFromStart();
		D3DXMatrixScaling(&m_UVMatrix, m_CurAnimTask.animDesc->vUVoffset.x, m_CurAnimTask.animDesc->vUVoffset.y, 1.0f);
	}
}

void CAnimation::PlayFromStart()
{
	m_vFrameIdx = { 0,0 };
	m_bPlaying = true;
	m_bEnd = false;
	m_bCanEnd = false;
	m_fTime = 0.f;
}
void CAnimation::Play()
{
	m_bPlaying = true;
	m_bEnd = false;
	m_fTime = 0.f;
}

void CAnimation::Pause()
{
	m_bPlaying = false;
}

void CAnimation::Stop()
{
	m_bPlaying = false;
	m_vFrameIdx = { 0,0 };
}


