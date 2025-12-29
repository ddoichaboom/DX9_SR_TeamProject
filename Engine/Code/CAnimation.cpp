#include "CAnimation.h"
#include "CTexture.h"


CAnimation::CAnimation()
	: m_pCurAnimation(nullptr), m_iCurState(-1), m_vFrameIdx{0,0}, m_bPlaying(false), 
	m_bEnd(false), m_fTime(0.f), m_bCanEnd(true)
{
	D3DXMatrixIdentity(&m_UVMatrix);
}

CAnimation::CAnimation(LPDIRECT3DDEVICE9 pGraphicDev)
	:CComponent(pGraphicDev), m_pCurAnimation(nullptr), m_iCurState(-1),
	m_vFrameIdx{ 0,0 }, m_bPlaying(false), m_bEnd(false),m_fTime(0.f), m_bCanEnd(true)
{
	D3DXMatrixIdentity(&m_UVMatrix);
}

CAnimation::CAnimation(const CAnimation& rhs)
	:CComponent(rhs), m_pCurAnimation(rhs.m_pCurAnimation), m_iCurState(-1),
	m_vFrameIdx{ 0,0 }, m_bPlaying(false), m_bEnd(false), m_fTime(0.f), m_bCanEnd(true)
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
	animDesc->fTotalFrame = animDesc->fEndFrameCol * animDesc->vMaxIdx.y;
	//애니메이션이 종료 가능한 Ratio 비율 
	animDesc->fEndRatio = AnimSource.fEndRatio;

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
	if (!m_pCurAnimation || !m_bPlaying || m_bEnd) return 0 ;
	if (m_pCurAnimation->vMaxIdx ==_vec2(0,0)) return 0; // 한 장 

	m_fTime += fTimeDelta;

	if (m_fTime >= m_pCurAnimation->fPlayTime)
	{
		m_fTime = 0.f;

		//지난 프레임이 마지막 이미지 였다면 
		if (m_vFrameIdx == _vec2(m_pCurAnimation->fEndFrameCol, m_pCurAnimation->vMaxIdx.y))
		{
			if (m_pCurAnimation->bLoop)
			{
				m_vFrameIdx = { 0.f, 0.f };
			}
			else
			{
				m_bEnd = true;
			}
		}
		else
		{
			m_vFrameIdx.x += 1.f;
			if (m_vFrameIdx.x > m_pCurAnimation->vMaxIdx.x) // 마지막 열 도달 
			{
				m_vFrameIdx.x = 0.f;
				m_vFrameIdx.y += 1.f;
				//마지막 행 검사는 위의 End인덱스 체크에서 수행됨 
			}
			
			_float curRatio = (m_vFrameIdx.x * m_vFrameIdx.y) / m_pCurAnimation->fTotalFrame;
			if (curRatio >= m_pCurAnimation->fEndRatio) m_bCanEnd = true;
			else m_bCanEnd = false;
		}

	}
	//한 프레임의 uv크기로 축소 + 해당 프레임의 uv위치로 이동한다
	// == 한 프레임의텍스쳐만 보여짐
	D3DXMatrixIdentity(&m_UVMatrix);
	m_UVMatrix._11 = m_pCurAnimation->vUVoffset.x;
	m_UVMatrix._22 = m_pCurAnimation->vUVoffset.y;
	m_UVMatrix._31 = m_vFrameIdx.x * m_pCurAnimation->vUVoffset.x;
	m_UVMatrix._32 = m_vFrameIdx.y * m_pCurAnimation->vUVoffset.y;

	return 0;
}

void CAnimation::Render_Animation()
{
	if (!m_pCurAnimation) return;
	//행렬변환 결과 중 앞에서 두 개의 요소만 적용한다 = 2차원 텍스쳐로 쓰겠다
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
	//텍스쳐 UV 공간을 행렬 변환한다 
	m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &m_UVMatrix);
	m_pGraphicDev->SetTexture(0, m_pCurAnimation->pTextureDesc->pTexture);
	
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

//animDesc 의 ratio는 디폴트가 0
//ratio를 따로 설정하지않으면 항상 바로 전환되고, 
//설정을 했다면 설정한 ratio이상 진행되어야 전환됨 
//Hit같이 일정 애니메이션 이상 플레이 되어야 넘어가는 것은 ratio 설정하기 
void CAnimation::Update_State(_uint State)
{
	if(CanEnd()) Change_Animation(State);
}

void CAnimation::Change_Animation(const _uint _state)
{
	if (m_iCurState == _state) return;
	if (m_mapAnimation.find(_state) == m_mapAnimation.end()) return;
	m_iCurState = _state;
	m_pCurAnimation = m_mapAnimation[m_iCurState];
	PlayFromStart();
	D3DXMatrixScaling(&m_UVMatrix, m_pCurAnimation->vUVoffset.x, m_pCurAnimation->vUVoffset.y, 1.0f);

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


