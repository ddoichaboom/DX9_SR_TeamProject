#pragma once
#include "CBase.h"

//pch.h의 DBG_NEW 매크로 비활성화(json.hpp 충돌 방지)
#ifdef new
#undef new
#endif

#include "json.hpp"
#include "Engine_Define.h"


class CEditorScene;

// JSON 라이브러리 별칭 
using json = nlohmann::json;

// 오브젝트 타입 열거형
enum OBJECT_TYPE
{
	OBJ_FLOOR,       // 바닥 (Tile 대체)
	OBJ_CEILING,     // 천장 
	OBJ_CUBE,        // 오브젝트
	OBJ_WALL,        // 벽 
	OBJ_SPAWNPOINT,  // 스폰 지점 
	OBJ_END
};

class CFileIO : public CBase
{
	DECLARE_SINGLETON(CFileIO)

private:
	explicit CFileIO();
	virtual ~CFileIO();

public:
	// 맵 저장 (JSON)
	HRESULT Save_MapData(const wstring& wstrPath, CEditorScene* pScene);

	// 맵 로드 (JSON)
	HRESULT Load_MapData(const wstring& wstrPath,
						CEditorScene* pScene,
						LPDIRECT3DDEVICE9 pGraphicDev);

private:
	// wstring -> string 변환
	string WStringToString(const wstring& wstr);

	// string -> wstring 변환
	wstring StringToWString(const string& str);

private:
	static const _uint FILE_VERSION = 2;

private:
	virtual	void	Free() override;
};
