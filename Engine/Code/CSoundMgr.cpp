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

	FMOD_System_GetMasterChannelGroup(m_pSystem, &master);

	FMOD_System_CreateChannelGroup(m_pSystem, "SFX", &gSFXGroup);
	FMOD_System_CreateChannelGroup(m_pSystem, "BGM", &gBGMGroup);
	FMOD_System_CreateChannelGroup(m_pSystem, "MONSTER", &gMonsterGroup);
	FMOD_System_CreateChannelGroup(m_pSystem, "PLAYER", &gPlayerGroup);

	FMOD_ChannelGroup_AddGroup(master, gSFXGroup, false, nullptr);
	FMOD_ChannelGroup_AddGroup(master, gBGMGroup, false, nullptr);
	FMOD_ChannelGroup_AddGroup(master, gMonsterGroup, false, nullptr);
	FMOD_ChannelGroup_AddGroup(master, gPlayerGroup, false, nullptr);
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

	FMOD_System_PlaySound(m_pSystem, iter->second, gSFXGroup, FALSE, nullptr);
	FMOD_ChannelGroup_SetVolume(gSFXGroup, fVolume);
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::PlayBGM(const TCHAR* pSoundKey, float fVolume)
{
	wstring key(pSoundKey);
	auto iter = m_mapSound.find(key);

	if (iter == m_mapSound.end())
		return;

	FMOD_Sound_SetMode(iter->second, FMOD_LOOP_NORMAL);
	FMOD_System_PlaySound(m_pSystem, iter->second, gBGMGroup, FALSE, nullptr);
	FMOD_ChannelGroup_SetVolume(gBGMGroup, fVolume);
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::PlayMonsterSound(const TCHAR* pSoundKey, float fVolume)
{
	wstring key(pSoundKey);
	auto iter = m_mapSound.find(key);

	if (iter == m_mapSound.end())
		return;

	FMOD_System_PlaySound(m_pSystem, iter->second, gMonsterGroup, FALSE, nullptr);
	FMOD_ChannelGroup_SetVolume(gMonsterGroup, fVolume);
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::PlayPlayerSound(const TCHAR* pSoundKey, float fVolume)
{
	wstring key(pSoundKey);
	auto iter = m_mapSound.find(key);

	if (iter == m_mapSound.end())
		return;

	FMOD_System_PlaySound(m_pSystem, iter->second, gPlayerGroup, FALSE, nullptr);
	FMOD_ChannelGroup_SetVolume(gPlayerGroup, fVolume);
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::StopSound(CHANNELID eID)
{
	FMOD_BOOL bIsPlay = false;
	FMOD_Channel_Stop(m_pChannelArr[eID]);
}

void CSoundMgr::StopAll()
{
	for (int i = 0; i < SOUND_END; ++i)
		FMOD_Channel_Stop(m_pChannelArr[i]);
}

void CSoundMgr::SetChannelVolume(CHANNELID eID, float fVolume)
{
	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);
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
	for (auto& Mypair : m_mapSound)
	{
		FMOD_Sound_Release(Mypair.second);
	}
	m_mapSound.clear();

	FMOD_System_Close(m_pSystem);
	FMOD_System_Release(m_pSystem);
}
