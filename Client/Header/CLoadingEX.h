#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "CLoadingBar.h"
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
	void	Update_Loading(const _float& fTimeDelta);
	_float	GetPercent() { return m_fPercent; }

public:
	void	AddTask(_int level, Task _task);
	bool	IsEnd();
public:
	static unsigned int CALLBACK Thread_Func(void* pArg);
	static CLoadingEX* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void		Free();

private:
	enum {Thread_Cnt = 4};
	vector<std::vector<Task>> m_vecTasks;

	LPDIRECT3DDEVICE9		m_pGraphicDev;
	HANDLE					m_hThread[Thread_Cnt];
	
	bool					m_bEnd = false;

	atomic<int>				m_iCurTaskCount;
	atomic<int>				m_iEndTaskCount;
	ELoadingLevel			m_eCurLevel;

	CLoadingBar*			m_pLoadingBar;

	_float					m_fTotalGauge = 0.f;
	_float					m_fCurGauge = 0.f;
	_float					m_fPercent = 0.f;

};

