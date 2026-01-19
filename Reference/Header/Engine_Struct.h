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


	typedef struct tagVertexParticle
	{
		_vec3		vPosition;
		_ulong		dwColor;
		_vec2		vTexUV;

	}VTXPTC;
	//순서 고정 
	const _ulong	FVF_PTC = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;

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
		bool	IsAtlas = false; // 여러 이미지가 합쳐진 이미지인가 
		_float fMaxRow = 0.f;
		_float fMaxCol = 0.f;
		_float fEndFrameCol = 0.f;
		_vec2   vPadding = { 0.f, 0.f };
	} TextureSource;

	//텍스쳐의 정보
	typedef struct tagTextureDesc
	{
		IDirect3DBaseTexture9* pTexture;
		_vec2  vOriginSize;
		_vec2  vMaxIdx = { 0,0 };
		_float fEndFrameCol = 0.f;
		_vec2  vUVoffset = { 1.f, 1.f };
		_vec2  vPaddingUV = { 0.f, 0.f };
	} TextureDesc;


	typedef struct tagAnimationSource
	{
		_uint  uState;
		//TODO : 제거. 이전 버전 호환용 
		_float fMaxRow;
		_float fMaxCol;
		_float fEndFrameCol; // 마지막 프레임의 열 번호 (이미지 배열이 꽉 차있지않은 경우를 고려함) 
		//
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
		//TODO : 제거. 이전 버전 호환용 
		_vec2 vMaxIdx;
		_float fEndFrameCol;
		_vec2 vUVoffset;
		//
		_float fTotalFrame;
		_float fEndRatio = 0.f;
		_bool bPriority = false;
		//애니메이션에서 텍스쳐마다 크기가 다른 경우, 0번 텍스쳐를 기준으로 scale 비율을 조정함 
		_float fAspect = 1.f;
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
		COLLIDER_TAG eTag;
		COL_DIR eDir = CDIR_NONE;
	} CollisionInfo;

	// 맵 데이터 구조체
	typedef struct tagObjectData
	{
		// 기본 정보 
		std::string	sType;				// "Floor", "Ceiling" ... 
		std::string	sName;				// 오브젝트 이름 ( 디버깅 용도 )

		// Transform
		_vec3	vPos;
		_vec3	vRot;
		_vec3	vScale;

		// 오브젝트별 속성
		_int	iTextureIdx;			// 정적 텍스처 출력 위한 인덱스 ( 0 ~ 7 / 0 ~ 2 )
		_uint	iFloorType;			// STATIC/DYNAMIC_FLOOR_TYPE enum
		_uint	iCeilingType;		// STATIC/DYNAMIC_CEILING_TYPE enum
		_uint	iWallType;			// STATIC/DYNAMIC_WALL_TYPE enum

		// SpawnPoint 전용
		std::string sSpawnType;           // "Player", "Monster", "BossMonster"
		std::string sMonsterKey;          // "WhiteMan", "BeamMon" 등

		tagObjectData()
			: sType(""), sName(""),
			vPos(0, 0, 0), vRot(0, 0, 0), vScale(1, 1, 1),
			iTextureIdx(0), iFloorType(0), iCeilingType(0), iWallType(0),
			sSpawnType(""), sMonsterKey("")
		{}

	}ObjectData;

	// 방 정보 구조체 
	typedef struct tagRoomData
	{
		_int iRoomIdx;				// 방 번호
		std::vector<ObjectData> vObjects;	// 해당 방의 모든 오브젝트
		_uint iFloorCount;
		_uint iDynamicFloorCount;
		_uint iCeilingCount;
		_uint iDynamicCeilingCount;
		_uint iWallCount;
		_uint iDynamicWallCount;
		_uint iObstacleCount;

		tagRoomData()
			: iRoomIdx(-1), iFloorCount(0), iDynamicFloorCount(0), iCeilingCount(0),
			iDynamicCeilingCount(0), iWallCount(0), iDynamicWallCount(0), iObstacleCount(0)
		{}

	}RoomData;

	// 추가 데이터가 필요한경우 여기에 추가하거나, EventData를 상속받아 만들어서 EventData로 전달하기
	// 그럴경우 , 외부에서 처리하기 위해 전역으로 
	typedef struct tagEventData
	{
		int value = -1;
	}EventData;


	typedef struct tagParticle
	{
		_vec3 vPosition{};
		_vec2  vSize = { 1.0f, 1.0f };
		_vec3 vVelocity{};
		_float fLifeTime = 0.f;
		_float fAnimTime = 0.f;
		_float fAge = 0.f;
		D3DXCOLOR color{};
		_vec2 vStartUV{};
		_vec2 vEndUV{};
		bool bIsAlive = false;
	}Particle;

	struct BoundingBox
	{
		BoundingBox()
		{
			min.x = FLT_MIN;
			min.y = FLT_MIN;
			min.z = FLT_MIN;
			max.x = FLT_MAX;
			max.y = FLT_MAX;
			max.z = FLT_MAX;
		}
		bool IsPointInside(D3DXVECTOR3& p)
		{
			if (p.x >= min.x && p.y >= min.y && p.z >= min.z &&
				p.x <= max.x && p.y <= max.y && p.z <= max.z)
			{
				return true;
			}
			else return  false;
		}

		D3DXVECTOR3 min;
		D3DXVECTOR3 max;
	};

}


#endif // Engine_Struct_h__
