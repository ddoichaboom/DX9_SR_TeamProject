#pragma once
#include "CBase.h"

class CMainMenuBar : public CBase
{
private:
	explicit CMainMenuBar();
	virtual ~CMainMenuBar();


public:
	HRESULT Ready_MenuBar();
	void    Update_MenuBar();
	void    Render_MenuBar();

private:
	// 파일 메뉴 관련
	void    Render_FileMenu();
	void    Render_EditMenu();
	void    Render_ViewMenu();
	void    Render_HelpMenu();

private:
	// UI 상태
	bool    m_bShowAbout;

public:
	static CMainMenuBar* Create();

private:
	virtual void Free() override;
};

