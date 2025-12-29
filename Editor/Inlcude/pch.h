// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H

// Windows 헤더
#include "framework.h"

// DirectX 9
#include <d3d9.h>
#include <d3dx9.h>

#include <process.h>


// STL
#include <vector>
#include <list>
#include <map>
#include <string>
#include <algorithm>
#include <fstream>

#define IMGUI_DEFINE_MATH_OPERATORS

// ImGui 헤더
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif

#endif

// CBase 포함 (모든 클래스가 상속)
#include "CBase.h"


// Engine 기본 타입만 (자주 변경 안 됨)
#include "Engine_Define.h"

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace std;
using namespace Engine;  // WINCX, WINCY 등 Engine 매크로 사용

#endif //PCH_H
