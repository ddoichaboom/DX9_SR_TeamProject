#ifndef Engine_Struct_h__
#define Engine_Struct_h__

#include "Engine_Typedef.h"

namespace Engine
{
	typedef struct tagVertexColor
	{
		_vec3		vPosition;			
		_ulong		dwColor;
	
	}VTXCOL;

	const _ulong	FVF_COL = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX0;

	typedef struct tagVertexTexture
	{
		_vec3		vPosition;
		_vec3		vNormal;
		_vec2		vTexUV;

	}VTXTEX;

	const _ulong	FVF_TEX = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

	typedef struct tagVertexCubeTexture
	{
		_vec3		vPosition;
		_vec3		vTexUV;

	}VTXCUBE;

	const _ulong	FVF_CUBE = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0); // 텍스처의 UV 좌표 값을 FLOAT형 3개로 표현하겠다는 매크로(괄호안의 숫자 0의 의미는 본래 버텍스에 텍스쳐 UV값이 여러개가 올 수 있는데 그중 0번째 값을 지정하겠다는 의미)

	typedef struct tagIndex16
	{
		_ushort  _0;
		_ushort  _1;
		_ushort  _2;

	}INDEX16;

	typedef struct tagIndex32
	{
		_ulong	_0;
		_ulong	_1;
		_ulong	_2;

	}INDEX32;	

	//텍스쳐 생성에 필요한 정보
	typedef struct tagTextureSource
	{
		_uint state;
		const _tchar* path;
	} TextureSource;

	//텍스쳐의 정보
	typedef struct tagTextureDesc
	{
		IDirect3DBaseTexture9* pTexture;
		_vec2 vOriginSize;
	} TextureDesc;


	typedef struct tagAnimationSource
	{
		_uint  uState;
		_float fMaxRow;
		_float fMaxCol;
		_float fEndFrameCol; // 마지막 프레임의 열 번호 (이미지 배열이 꽉 차있지않은 경우를 고려함) 
		_bool  bLoop;
		_float fPlayTime = 0.12f;
		_float fEndRatio = 0.f;
		_bool  bPriority = false; //애니메이션 우선순위. 현재 애니메이션 무시하고 바로 출력할지
	} AnimationSource;

	typedef struct tagAnimationDesc
	{
		TextureDesc* pTextureDesc;
		_float fPlayTime;
		_bool bLoop;
		_vec2 vMaxIdx;
		_float fEndFrameCol;
		_vec2 vUVoffset;
		_float fTotalFrame;
		_float fEndRatio = 0.f;
		_bool bPriority = false;
		//애니메이션에서 텍스쳐마다 크기가 다른 경우, 0번 텍스쳐를 기준으로 scale 비율을 조정함 
		_vec2 vTextureScaleOffset = { 1.f, 1.f };
	} AnimationDesc;

	//애니메이션 Queue에 삽입될 구조체
	typedef struct tagAnimTask
	{
		SUBSTATE subState;
		AnimationDesc* animDesc;
	}AnimTask;

	//충돌 
	class CGameObject;
	typedef struct tagCollisionInfo
	{
		CGameObject* pTarget;
		_vec3 vDiff;
		_float fDamage;
	} CollisionInfo;
}


#endif // Engine_Struct_h__
