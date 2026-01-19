#pragma once
#include "CBase.h"

#ifdef new
#undef new
#endif

#include "json.hpp"
#include "Engine_Define.h"

namespace Engine
{
	class CLayer;
}

using json = nlohmann::json;

class CMapLoader : public CBase
{
	DECLARE_SINGLETON(CMapLoader)

private:
	explicit CMapLoader();
	virtual ~CMapLoader();

public:
	static vector<wstring>& Get_MapFiles()
	{
		return m_vecMapFiles;
	}

public:
	// 모든 맵 데이터 사전 파싱 및 캐싱
	HRESULT Preload_AllMapData(const wstring& wstrPath);

	// 특정 방만 로드 (레이어별 분기 처리)
	HRESULT Load_Room(const wstring& wstrPath,
						_int iRoomIndex,
						CLayer* pLayer,
						LPDIRECT3DDEVICE9 pGraphicDev,
						const wstring& pLayerTag);

	// 특정 방 언로드
	HRESULT Unload_Room(const wstring& wstrPath,
						_int iRoomIndex,
						CLayer* pLayer);

	// 최대 오브젝트 수 계산 
	_uint Get_MaxObjectCount(const wstring& wstrPath,
							const string& objectType);

private:
	// ObjectData  생성
	ObjectData Parse_ObjectData_FromJSON(const json& jObj);

	// 풀에서 오브젝트 획득 및 설정
	CGameObject* Get_GameObject_FromPool(const ObjectData& objData,
										LPDIRECT3DDEVICE9 pGraphicDev);

	// wstring -> string 변환
	string WStringToString(const wstring& wstr);

	// string -> wstring 변환
	wstring StringToWString(const string& str);

private:
	static const _uint FILE_VERSION = 4;  

	// 동적 방 로딩 시스템 
	// Key 1 : 맵 파일 이름 
	// Key 2 : 방 번호 (0, 1, 2 ... )
	// Value : 해당 방의 오브젝트 데이터
	map<string, map<int, RoomData>>		m_mapAllRooms;
	static vector<wstring>				m_vecMapFiles;

private:
	virtual void Free() override;
};

