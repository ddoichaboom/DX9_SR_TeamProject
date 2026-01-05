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
	// 맵 로드 (JSON)
	HRESULT Load_MapData(const wstring& wstrPath,
							Engine::CLayer* pLayer,
							LPDIRECT3DDEVICE9 pGraphicDev);

	_vec3 Get_PlayerSpawnPos() const { return m_vPlayerSpawnPos; }

	const map<string, vector<_vec3>>& Get_MonsterSpawns() const
	{
		return m_mapMonsterSpawnPos;
	}
	vector<_vec3> Get_MonsterSpawnPos(const string& monsterKey) const
	{
		auto iter = m_mapMonsterSpawnPos.find(monsterKey);
		if (iter != m_mapMonsterSpawnPos.end())
			return iter->second;
		return vector<_vec3>();  // 빈 벡터 반환
	}

private:
	// JSON에서 GameObject 생성
	CGameObject* Create_GameObject_FromJSON(const json& jObj,
											LPDIRECT3DDEVICE9 pGraphicDev);

	// wstring -> string 변환
	string WStringToString(const wstring& wstr);

	// string -> wstring 변환
	wstring StringToWString(const string& str);

private:
	static const _uint FILE_VERSION = 2;  // Editor v2와 호환

private:
	_vec3						m_vPlayerSpawnPos;
	map<string, vector<_vec3>> m_mapMonsterSpawnPos;

	virtual void Free() override;
};

