#include "CSoundMgr.h"
#include "fmod.hpp"

#ifdef _DEBUG
#pragma comment(lib, "fmodL_vc.lib")
#else
#pragma comment(lib, "fmod_vc.lib")
#endif

IMPLEMENT_SINGLETON(CSoundMgr)

CSoundMgr::CSoundMgr() : m_pSystem(nullptr)
{
}

CSoundMgr::~CSoundMgr()
{
	Free();
}

void CSoundMgr::Ready_Sound()
{
	FMOD_System_Create(&m_pSystem, FMOD_VERSION);
	FMOD_System_Init(m_pSystem, 64, FMOD_INIT_NORMAL, nullptr);

	FMOD_System_GetMasterChannelGroup(m_pSystem, &m_pChannelGroup[SOUND_MASTER]);

	FMOD_System_CreateChannelGroup(m_pSystem, "SFX", &m_pChannelGroup[SOUND_SFX]);
	FMOD_System_CreateChannelGroup(m_pSystem, "BGM", &m_pChannelGroup[SOUND_BGM]);
	FMOD_System_CreateChannelGroup(m_pSystem, "MONSTER", &m_pChannelGroup[SOUND_MONSTER]);
	FMOD_System_CreateChannelGroup(m_pSystem, "PLAYER", &m_pChannelGroup[SOUND_PLAYER]);

	FMOD_ChannelGroup_AddGroup(m_pChannelGroup[SOUND_MASTER], m_pChannelGroup[SOUND_SFX], false, nullptr);
	FMOD_ChannelGroup_AddGroup(m_pChannelGroup[SOUND_MASTER], m_pChannelGroup[SOUND_BGM], false, nullptr);
	FMOD_ChannelGroup_AddGroup(m_pChannelGroup[SOUND_MASTER], m_pChannelGroup[SOUND_MONSTER], false, nullptr);
	FMOD_ChannelGroup_AddGroup(m_pChannelGroup[SOUND_MASTER], m_pChannelGroup[SOUND_PLAYER], false, nullptr);
	LoadSoundFile();
}

void CSoundMgr::Update_Sound()
{
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::PlaySoundByID(const TCHAR* pSoundKey, CHANNELID eID, float fVolume)
{
	wstring key(pSoundKey);
	auto iter = m_mapSound.find(key);

	if (iter == m_mapSound.end())
		return;

	FMOD_BOOL bPlay = FALSE;

	FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, FALSE, &m_pChannelArr[eID]);
	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::PlaySFXSound(const TCHAR* pSoundKey, float fVolume)
{
	wstring key(pSoundKey);
	auto iter = m_mapSound.find(key);

	if (iter == m_mapSound.end())
		return;

	FMOD_System_PlaySound(m_pSystem, iter->second, m_pChannelGroup[SOUND_SFX], FALSE, nullptr);
	FMOD_ChannelGroup_SetVolume(m_pChannelGroup[SOUND_SFX], fVolume);
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::PlayBGM(const TCHAR* pSoundKey, float fVolume)
{
	wstring key(pSoundKey);
	auto iter = m_mapSound.find(key);

	if (iter == m_mapSound.end())
		return;

	FMOD_Sound_SetMode(iter->second, FMOD_LOOP_NORMAL);
	FMOD_System_PlaySound(m_pSystem, iter->second, m_pChannelGroup[SOUND_BGM], FALSE, &m_pChannelArr[SOUND_BGM]);
	FMOD_ChannelGroup_SetVolume(m_pChannelGroup[SOUND_BGM], fVolume);
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::PlayMonsterSound(const TCHAR* pSoundKey, float fVolume)
{
	wstring key(pSoundKey);
	auto iter = m_mapSound.find(key);

	if (iter == m_mapSound.end())
		return;

	FMOD_System_PlaySound(m_pSystem, iter->second, m_pChannelGroup[SOUND_MONSTER], FALSE, nullptr);
	FMOD_ChannelGroup_SetVolume(m_pChannelGroup[SOUND_MONSTER], fVolume);
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::PlayPlayerSound(const TCHAR* pSoundKey, float fVolume)
{
	wstring key(pSoundKey);
	auto iter = m_mapSound.find(key);

	if (iter == m_mapSound.end())
		return;

	FMOD_System_PlaySound(m_pSystem, iter->second, m_pChannelGroup[SOUND_PLAYER], FALSE, nullptr);
	FMOD_ChannelGroup_SetVolume(m_pChannelGroup[SOUND_PLAYER], fVolume);
	FMOD_System_Update(m_pSystem);
}

//단일 채널 - 내가 따로 보관했다면 사용 가능 (ex) BGM
void CSoundMgr::StopSound(CHANNELID eID)
{
	FMOD_Channel_Stop(m_pChannelArr[eID]);
}

//그룹전체 정지
void CSoundMgr::StopGroupSound(CHANNELID eID)
{
	FMOD_ChannelGroup_Stop(m_pChannelGroup[eID]);
}

void CSoundMgr::StopAll()
{
	for (int i = 0; i < SOUND_END; ++i)
		FMOD_ChannelGroup_Stop(m_pChannelGroup[i]);
}

void CSoundMgr::SetChannelVolume(CHANNELID eID, float fVolume)
{
	FMOD_ChannelGroup_SetVolume(m_pChannelGroup[eID], fVolume);
}

void CSoundMgr::LoadSoundFile()
{
	_finddata_t fd;
	intptr_t handle = _findfirst("../Bin/Resource/Sound/*.*", &fd);

	if (handle == -1)
		return;

	int iResult = 0;

	char szCurPath[128] = "../Bin/Resource/Sound/";
	char szFullPath[256] = "";

	while (iResult != -1)
	{
		if (strcmp(fd.name, ".") == 0 || strcmp(fd.name, "..") == 0)
		{
			iResult = _findnext(handle, &fd);
			continue; // 다음 파일로
		}

		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, fd.name); // "../Sound/파일명"

		FMOD_SOUND* pSound = nullptr;
		FMOD_RESULT eRes;
		//이름에 BGM이 들어가면 BGM으로 인식 
		char* result = strstr(fd.name, "BGM");

		if (result)
		{
			pSound = nullptr;
			eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_CREATESTREAM | FMOD_LOOP_NORMAL | FMOD_2D, 0, &pSound);
		}
		else
		{
			pSound = nullptr;
			eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_DEFAULT, 0, &pSound);
		}

		if (eRes == FMOD_OK)
		{
			// 아스키 → 유니코드 문자열 변환
			wchar_t wKey[128];
			MultiByteToWideChar(CP_ACP, 0, fd.name, -1, wKey, 128);

			wstring soundKey(wKey);
			m_mapSound.emplace(soundKey, pSound);
		}

		iResult = _findnext(handle, &fd);
	}

	FMOD_System_Update(m_pSystem);
	_findclose(handle);
}

void CSoundMgr::Free()
{
	StopAll();
	for (auto& Mypair : m_mapSound)
	{
		FMOD_Sound_Release(Mypair.second);
	}
	m_mapSound.clear();

	for (int i = 0; i < SOUND_END; ++i)
	{
		if (m_pChannelGroup[i]) 
			FMOD_ChannelGroup_Release(m_pChannelGroup[i]);
	}

	FMOD_System_Close(m_pSystem);
	FMOD_System_Release(m_pSystem);
}
