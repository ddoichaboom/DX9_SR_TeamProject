#pragma once

#include "Engine_Define.h"
#include "CBase.h"

BEGIN(Engine)

class ENGINE_DLL CDInputMgr : public CBase
{
	DECLARE_SINGLETON(CDInputMgr)

private:
	explicit CDInputMgr(void);
	virtual ~CDInputMgr(void);

public:
	_byte	Get_DIKeyState(_ubyte byKeyID)
	{
		return m_byKeyState[byKeyID];
	}

	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse)
	{
		return m_tMouseState.rgbButtons[eMouse];
	}

	// 현재 마우스의 특정 축 좌표를 반환
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
	{
		return *(((_long*)&m_tMouseState) + eMouseState);
	}

public:
	bool Key_Down(_ubyte byKeyID);
	bool Key_Pressing(_ubyte byKeyID);
	bool Key_Up(_ubyte byKeyID);

	bool Mouse_Down(MOUSEKEYSTATE eMouse);
	bool Mouse_Pressing(MOUSEKEYSTATE eMouse);
	bool Mouse_Up(MOUSEKEYSTATE eMouse);

public:
	HRESULT Ready_InputDev(HINSTANCE hInst, HWND hWnd);
	void	Update_InputDev(void);

public:
	bool	GetDebugState();
	MOVE_DIR	Get_Direction() const { return m_eDirection; }

private :
	_int	GetAxisRaw_Horizontal();
	_int	GetAxisRaw_Vertical();
	MOVE_DIR	Make_Direction();

	
private:
	const _ubyte			m_byDebugKey = DIK_G;
	_bool					m_bDebug = false;
	LPDIRECTINPUT8			m_pInputSDK = nullptr;

private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = nullptr;
	LPDIRECTINPUTDEVICE8	m_pMouse = nullptr;

private:
	_byte					m_byKeyState[256];		// 키보드에 있는 모든 키값을 저장하기 위한 변수
	DIMOUSESTATE			m_tMouseState;

	_byte					m_byPrevKeyState[256];	// 이전 프레임의 키보드 상태값 
	DIMOUSESTATE			m_tPrevMouseState;		// 이전 프레임의 마우스 상태값

	// Axis 추가
	_int					m_iAxisX;
	_int					m_iAxisY;
	MOVE_DIR				m_eDirection;

public:
	virtual void	Free(void);

};
END


