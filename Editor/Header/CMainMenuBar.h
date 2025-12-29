#pragma once
#include "CBase.h"

class CEditorScene;

class CMainMenuBar : public CBase
{
private:
	explicit CMainMenuBar();
	virtual ~CMainMenuBar();


public:
	HRESULT				Ready_MenuBar();
	void				Update_MenuBar();
	void				Render_MenuBar();

	// Scene 연결
	void				Set_Scene(CEditorScene* pScene) { m_pScene = pScene; }

private:
	// 파일 메뉴 관련
	void				Render_FileMenu();
	void				Render_EditMenu();
	void				Render_ViewMenu();
	void				Render_HelpMenu();

private:
	// UI 상태
	bool				m_bShowAbout;
	CEditorScene*		m_pScene;

public:
	static CMainMenuBar* Create();

private:
	virtual void		Free() override;
};

