#pragma once
#include "CBase.h"
#include <io.h>
#include "Engine_Define.h"

struct FMOD_SOUND;
struct FMOD_CHANNEL;
struct FMOD_SYSTEM;
struct FMOD_CHANNELGROUP;

BEGIN(Engine)

class ENGINE_DLL CSoundMgr : public CBase
{
	DECLARE_SINGLETON(CSoundMgr)

private:
	explicit CSoundMgr();
	virtual ~CSoundMgr();

public:
	void Ready_Sound();
	void Update_Sound();

public:
	void PlaySoundByID(const TCHAR* pSoundKey, CHANNELID eID, float fVolume);

	void PlaySFXSound(const TCHAR* pSoundKey, float fVolume = 1.0f);
	void PlayBGM(const TCHAR* pSoundKey, float fVolume = 1.0f);
	void PlayMonsterSound(const TCHAR* pSoundKey, float fVolume = 1.0f);
	void PlayPlayerSound(const TCHAR* pSoundKey, float fVolume = 1.0f);

	void StopSound(CHANNELID eID);
	void StopGroupSound(CHANNELID eID);
	void StopAll();
	void SetChannelVolume(CHANNELID eID, float fVolume);

private:
	void LoadSoundFile();

private:
	virtual void Free();

	// 사운드 리소스 정보를 갖는 객체 
	map<wstring, FMOD_SOUND*> m_mapSound;
	FMOD_CHANNEL* m_pChannelArr[SOUND_END];
	FMOD_CHANNELGROUP* m_pChannelGroup[SOUND_END];

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM* m_pSystem;

	//FMOD_CHANNELGROUP* master = nullptr;
	////몬스터 사운드
	//FMOD_CHANNELGROUP* gMonsterGroup = nullptr;
	////플레이어 관련 사운드
	//FMOD_CHANNELGROUP* gPlayerGroup = nullptr;
	////그 외 효과음 
	//FMOD_CHANNELGROUP* gSFXGroup = nullptr;
	////배경음
	//FMOD_CHANNELGROUP* gBGMGroup = nullptr;

};

END