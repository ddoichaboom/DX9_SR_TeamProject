#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include <atomic>

using Task = std::function<void()>;

struct ThreadArg
{
	Task task;
};

class CLoadingEX : public CBase
{
public:
	enum ELoadingLevel
	{
		Lv1_INIT, Lv2_CHAR_RES, Lv3_TERRAIN_RES, Lv4_UI_RES, Lv5_EFFECT_RES, Lv6_MAP_ENV_LOAD, Lv7_MAP_GAME_LOAD, LEVEL_END
	};
public:
	explicit CLoadingEX(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLoadingEX();
	
public:
	HRESULT	Ready_Loading();
	bool	Update_Loading();
	_float	GetPercent() { return m_fPercent; }

public:
	void	AddTask(_int level, Task _task);
	bool	IsEnd() { return m_bEnd; }
public:
	static unsigned int CALLBACK Thread_Func(void* pArg);
	static CLoadingEX* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void		Free();

private:
	vector<std::vector<Task>> m_vecTasks;

	LPDIRECT3DDEVICE9		m_pGraphicDev;
	HANDLE					m_hThread[4];
	_float					m_fPercent;
	bool					m_bEnd = false;

	atomic<int>				m_iCurTaskCount;
	atomic<int>				m_iEndTaskCount;
	ELoadingLevel			m_eCurLevel;
};

