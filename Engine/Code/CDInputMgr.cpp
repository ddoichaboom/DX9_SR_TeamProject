#include "CDInputMgr.h"

IMPLEMENT_SINGLETON(CDInputMgr)

Engine::CDInputMgr::CDInputMgr(void)
{
	ZeroMemory(m_byKeyState, sizeof(m_byKeyState));
	ZeroMemory(m_byPrevKeyState, sizeof(m_byPrevKeyState));
	ZeroMemory(&m_tPrevMouseState, sizeof(m_tPrevMouseState));
	ZeroMemory(&m_tMouseState, sizeof(m_tMouseState));
}

Engine::CDInputMgr::~CDInputMgr(void)
{
	Free();
}

bool CDInputMgr::Key_Down(_ubyte byKeyID)
{
	if (!(m_byPrevKeyState[byKeyID] & 0x80) && (m_byKeyState[byKeyID] & 0x80))
	{
		return true;
	}
	return false;
}

bool CDInputMgr::Key_Pressing(_ubyte byKeyID)
{
	if (m_byKeyState[byKeyID] & 0x80) return true;
	return false;
}

bool CDInputMgr::Key_Up(_ubyte byKeyID)
{
	if ((m_byPrevKeyState[byKeyID] & 0x80) && !(m_byKeyState[byKeyID] & 0x80))
	{
		return true;
	}
	return false;
}

bool CDInputMgr::Mouse_Down(MOUSEKEYSTATE eMouse)
{
	if (!(m_tPrevMouseState.rgbButtons[eMouse] & 0x80) && (m_tMouseState.rgbButtons[eMouse] & 0x80))
	{
		return true;
	}
	return false;
}

bool CDInputMgr::Mouse_Pressing(MOUSEKEYSTATE eMouse)
{
	if (m_tMouseState.rgbButtons[eMouse] & 0x80)
	{
		return true;
	}
	return false;
}

bool CDInputMgr::Mouse_Up(MOUSEKEYSTATE eMouse)
{
	if ((m_tPrevMouseState.rgbButtons[eMouse] & 0x80) && !(m_tMouseState.rgbButtons[eMouse] & 0x80))
	{
		return true;
	}
	return false;
}

HRESULT Engine::CDInputMgr::Ready_InputDev(HINSTANCE hInst, HWND hWnd)
{

	// DInput 컴객체를 생성하는 함수
	if (FAILED(DirectInput8Create(hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pInputSDK,
		NULL)))
		return E_FAIL;

	// 키보드 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		return E_FAIL;

	// 생성된 키보드 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// 장치에 대한 독점권을 설정해주는 함수, (클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pKeyBoard->Acquire();


	// 마우스 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;


	// 생성된 마우스 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pMouse->Acquire();


	return S_OK;
}

void Engine::CDInputMgr::Update_InputDev(void)
{
	//이전 프레임의 키보드,마우스값 저장 
	memcpy(m_byPrevKeyState, m_byKeyState, sizeof(m_byPrevKeyState));
	memcpy(&m_tPrevMouseState, &m_tMouseState, sizeof(DIMOUSESTATE));

	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);
}

void Engine::CDInputMgr::Free(void)
{
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}

