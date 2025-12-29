# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 프로젝트 개요

SR3_Project는 DirectX 9 기반의 3D 게임 엔진과 맵 에디터 프로젝트입니다.

**주요 프로젝트:**
- **Engine**: 공통 게임 엔진 (렌더링, 컴포넌트, 리소스 관리)
- **Client**: 게임 클라이언트 (플레이어 실행 파일)
- **Editor**: 맵 에디터 (ImGui 기반 레벨 디자인 툴)

**현재 상태**: Phase 6 완료 (맵 저장/로드), Phase 6.5 진행 예정 (ImGuizmo), Phase 7 예정 (텍스처 시스템)

## 빌드 및 실행

### 빌드 방법
```bash
# Visual Studio 2019/2022에서 솔루션 열기
SR3_Project.sln

# Editor 프로젝트를 시작 프로젝트로 설정
# 솔루션 탐색기 > Editor 우클릭 > 시작 프로젝트로 설정

# 빌드 및 실행
F5 (디버그 모드)
Ctrl+F5 (디버그 없이 실행)
```

### 실행 파일 경로
- Editor: `Editor/Bin/Editor.exe`
- Client: `Client/Bin/Client.exe`

## 아키텍처 개요

### 프로젝트 구조

```
SR3_Project/
├── Engine/              # 공통 게임 엔진
│   ├── Header/          # 엔진 헤더 파일
│   ├── Code/            # 엔진 구현 파일
│   └── Bin/             # 엔진 빌드 출력 (Engine.lib)
│
├── Reference/           # 공유 헤더 및 라이브러리
│   ├── Header/          # 공통 헤더 (Engine_Define.h, json.hpp 등)
│   └── Library/         # 엔진 라이브러리
│
├── Editor/              # 맵 에디터
│   ├── Header/          # 모든 .h 파일 위치
│   ├── Code/            # 모든 .cpp 파일 위치
│   ├── ImGui/           # ImGui 라이브러리
│   ├── Include/         # 프로젝트 설정 파일 (pch.h, framework.h)
│   └── Bin/             # 에디터 빌드 출력
│
├── Client/              # 게임 클라이언트
│   ├── Header/
│   ├── Code/
│   └── Bin/
│
├── Resource/            # 게임 리소스
│   ├── Texture/         # 텍스처 파일
│   └── Mesh/            # 메쉬 파일 (있는 경우)
│
├── Map/                 # 저장된 맵 파일 (.json)
│
└── 명세서/              # 프로젝트 문서 (개발 계획, 진행 상황 등)
```

### 중요: 파일 시스템 vs Visual Studio 필터

**실제 파일 위치 (파일 시스템):**
- 모든 `.h` 파일: `Editor/Header/` 디렉토리에 직접 저장
- 모든 `.cpp` 파일: `Editor/Code/` 디렉토리에 직접 저장

**Visual Studio 필터 (솔루션 탐색기 가상 분류):**
```
Editor (프로젝트)
├── 00. EditorApp
├── 01. UI (MenuBar, ToolBar, Hierarchy, Inspector)
├── 03. Scene (EditorScene, EditorCamera, Grid)
├── 04. Object (EditorObject, EditorTile, EditorCube)
├── 05. Picking (MousePicker, SelectionMgr)
└── 06. Resource (FileIO, json.hpp)
```

필터는 단지 코드 조직을 위한 것이며, 실제 파일 경로와 무관합니다.

### 엔진 아키텍처

**Engine 프로젝트는 컴포넌트 기반 게임 엔진:**

```cpp
// 핵심 베이스 클래스
CBase               // 참조 카운팅 베이스 클래스 (AddRef, Release)
CGameObject         // 모든 게임 오브젝트의 베이스
CComponent          // 컴포넌트 베이스 (Transform, VIBuffer 등)
CScene              // 씬 관리

// 싱글톤 매니저
CGraphicDev         // DirectX 9 디바이스 관리
CRenderer           // 렌더링 파이프라인
CProtoMgr           // 프로토타입 패턴 (오브젝트 복제)
CManagement         // 씬 관리
CDInputMgr          // 입력 관리
```

**매크로:**
- `DECLARE_SINGLETON(ClassName)` - 싱글톤 선언 (헤더)
- `IMPLEMENT_SINGLETON(ClassName)` - 싱글톤 구현 (cpp)
- `Safe_Release(ptr)` - 안전한 Release 호출
- `MSG_BOX(msg)` - 에러 메시지 박스

### Editor 주요 시스템

#### 1. Scene 시스템 (Phase 2 완료)
```cpp
CEditorScene        // 메인 씬 (오브젝트 관리, 입력 처리)
CEditorCamera       // FREE/FPS 카메라 (WASD 이동, 마우스 회전)
CGrid               // XZ 평면 그리드
```

#### 2. UI 시스템 (Phase 3 완료)
```cpp
CMainMenuBar        // File, Edit, View, Help 메뉴
CToolBar            // 에디터 모드 선택 (Select, Place Tile, Place Cube)
CHierarchy          // 씬 오브젝트 트리 뷰
CInspector          // 선택된 오브젝트 속성 편집
```

#### 3. Object 시스템 (Phase 4 완료)
```cpp
CEditorObject       // 베이스 클래스 (Transform, Texture 관리)
  ├─ CEditorTile    // RcTex 기반 타일 (XZ 평면)
  └─ CEditorCube    // CubeTex 기반 큐브
```

#### 4. Ray Picking 시스템 (Phase 5 완료)
```cpp
CMousePicker        // Screen to World Ray 변환
CSelectionMgr       // 로컬 스페이스 Ray-AABB 충돌 검사
                    // 겹치는 오브젝트 순환 선택 (Unity 스타일)
```

**핵심 최적화: 로컬 스페이스 Ray-AABB**
```cpp
// World 공간이 아닌 Local 공간에서 충돌 검사
// 1. 월드 행렬의 역행렬 계산
// 2. Ray를 로컬 공간으로 변환
// 3. 고정 AABB (-1, -1, -1) ~ (1, 1, 1) 사용
// 4. Slab Method로 충돌 검사
// 장점: 정점 변환 불필요, 회전/크기 자동 반영
```

#### 5. File I/O 시스템 (Phase 6 완료)
```cpp
CFileIO             // JSON 기반 맵 저장/로드
                    // nlohmann/json 라이브러리 사용
```

**JSON 파일 포맷:**
```json
{
  "version": 1,
  "objectCount": 5,
  "objects": [
    {
      "type": "Tile",
      "position": [5.0, 0.0, 5.0],
      "rotation": [-90.0, 0.0, 0.0],
      "scale": [1.0, 1.0, 1.0],
      "name": "Tile_0"
    }
  ]
}
```

**JSON 포맷의 장점:**
- 사람이 읽을 수 있음 (디버깅 쉬움)
- 메모장으로 직접 수정 가능
- Phase 7에서 "texture" 필드 추가 용이

## 개발 워크플로우

### Phase 진행 상황

| Phase | 목표 | 완료율 | 상태 |
|-------|------|--------|------|
| Phase 1 | 기반 시스템 (Engine + ImGui) | 100% | ✅ 완료 |
| Phase 2 | Scene & Camera | 100% | ✅ 완료 |
| Phase 3 | UI 시스템 | 100% | ✅ 완료 |
| Phase 4 | 오브젝트 배치 | 100% | ✅ 완료 |
| Phase 5 | Picking & 선택 시스템 | 100% | ✅ 완료 |
| Phase 6 | 맵 저장/로드 (JSON) | 100% | ✅ 완료 |
| **Phase 6.5** | **ImGuizmo 통합** | **0%** | **⏳ 다음 단계** |
| Phase 7 | 텍스처 시스템 | 0% | ⏳ 진행 예정 |

**전체 진행률: 약 86%** (Phase 6.5 완료 시 90%)

### Phase 6.5 계획 (다음 단계)

**목표**: Transform 직접 조작 시스템 (Unity/Unreal 수준의 UX)

**작업 목록:**
1. ImGuizmo 라이브러리 추가 (10분)
2. CEditorScene에 Gizmo 렌더링 추가 (30분)
3. Unity/Unreal 입력 패턴 구현 (20분)
   - **핵심**: 우클릭으로 카메라/편집 모드 전환
   - 우클릭 홀드 → 카메라 모드 (WASD 이동, Gizmo 키 무시)
   - 우클릭 안 누름 → 편집 모드 (W/E/R로 Gizmo 전환)
4. 렌더링 파이프라인 통합 (15분)
5. 테스트 및 검증 (15분)

**예상 시간**: 1-2시간

**키 충돌 해결**:
- 기존 문제: W/E 키가 카메라 이동(WASD)과 Gizmo 전환(W/E/R)에 동시 사용
- 해결책: Unity/Unreal 방식 채택 (우클릭 = 카메라, 평소 = 편집)
- 결과: 직관적이고 충돌 없는 입력 시스템

**참고 문서**: `명세서/Editor_Phase6.5_ImGuizmo_구현가이드.md`

### Phase 7 계획 (Phase 6.5 이후)

**목표**: Inspector에서 텍스처 선택 및 적용

**작업 목록:**
1. CTextureManager 구현 (싱글톤)
2. CEditorApp::Load_Textures() 추가
3. CInspector 텍스처 선택 UI 추가
4. CEditorObject 텍스처 적용 구현
5. CFileIO에 텍스처 키 저장/로드 추가

**참고 문서**: `명세서/Editor_Phase7_텍스처시스템_구현가이드.md`

### 코드 작성 시 주의사항

#### 1. 파일 포함 및 헤더
```cpp
// .cpp 파일 첫 줄은 항상 pch.h
#include "pch.h"

// pch.h에는 다음이 포함됨:
// - Windows API
// - DirectX 9 (d3d9.h, d3dx9.h)
// - STL (vector, list, map, string 등)
// - Engine_Define.h (엔진 타입 정의)
```

#### 2. 메모리 관리
```cpp
// 참조 카운팅 사용
CEditorObject* pObj = CEditorTile::Create(...);  // RefCount = 1
pScene->Add_Object(pObj);                        // RefCount = 2
Safe_Release(pObj);                              // RefCount = 1

// 삭제 시
Safe_Release(pObj);  // RefCount = 0 → 자동 delete
```

#### 3. 싱글톤 사용
```cpp
// 헤더 파일
class CFileIO : public CBase
{
    DECLARE_SINGLETON(CFileIO)
    // ...
};

// cpp 파일
IMPLEMENT_SINGLETON(CFileIO)

// 사용
CFileIO::GetInstance()->Save_MapData(...);

// 정리 (CEditorApp::Free()에서)
CFileIO::DestroyInstance();
```

#### 4. JSON 사용 시 주의사항
```cpp
// CFileIO.h 상단에 반드시 추가
#ifdef new
#undef new
#endif
#include "json.hpp"

// 이유: pch.h의 DBG_NEW 매크로가 json.hpp와 충돌
```

#### 5. Windows 파일 다이얼로그
```cpp
// framework.h에 추가 필요
#include <commdlg.h>

// 사용 예시
OPENFILENAME ofn;
ZeroMemory(&ofn, sizeof(ofn));
ofn.lpstrFilter = L"JSON Map Files (*.json)\0*.json\0";
if (GetSaveFileName(&ofn))
{
    // 파일 저장
}
```

## 주요 참고 문서

**프로젝트 명세서 (`명세서/` 폴더):**
- `Editor_진행상황_2025-12-27.md` - 최신 진행 상황 (가장 중요!)
- `Editor_전체_개발계획_v3.md` - 전체 로드맵
- `Editor_Phase6_파일저장로드_구현가이드_JSON.md` - Phase 6 완료 가이드
- `Editor_Phase6.5_ImGuizmo_구현가이드.md` - Phase 6.5 가이드 (다음 단계!)
- `Editor_Phase7_텍스처시스템_구현가이드.md` - Phase 7 가이드

**작업 시작 전:**
1. `Editor_진행상황_2025-12-27.md` 읽기 (현재 상태 파악)
2. 해당 Phase 구현 가이드 읽기
3. 기존 코드 분석 (관련 클래스 Read)

## 코드 스타일

### 네이밍 규칙
```cpp
// 클래스: C 접두사 + PascalCase
class CEditorScene { };

// 멤버 변수: m_ 접두사 + PascalCase
_float m_fSpeed;
CEditorObject* m_pSelectedObject;

// 함수: PascalCase (동사 + 명사)
void Set_Position(_vec3 vPos);
_vec3 Get_Position() const;

// 상수: 모두 대문자 + 언더스코어
const _uint FILE_VERSION = 1;

// 열거형: 모두 대문자 + 언더스코어
enum OBJECT_TYPE
{
    OBJ_TILE,
    OBJ_CUBE,
    OBJ_END
};
```

### 엔진 타입 정의
```cpp
// Engine_Define.h에 정의된 타입들 (D3D 래퍼)
typedef D3DXVECTOR3     _vec3;
typedef D3DXVECTOR4     _vec4;
typedef D3DXMATRIX      _matrix;
typedef unsigned int    _uint;
typedef float           _float;
```

### 코멘트 스타일
```cpp
// 한국어 주석 사용 (프로젝트 전체)
// Phase 6 완료 - 맵 저장/로드

// 섹션 구분
// ========== 맵 저장 (JSON) ==========

// Phase 표시 (구현 예정 기능)
// Phase 7에서 추가 예정:
// jObj["texture"] = textureKey;
```

## 일반적인 작업

### 새 클래스 추가하기

1. **헤더 파일 생성** (`Editor/Header/CNewClass.h`)
```cpp
#pragma once
#include "CBase.h"

class CNewClass : public CBase
{
private:
    explicit CNewClass();
    virtual ~CNewClass();

public:
    HRESULT Ready_NewClass();
    void Update_NewClass(const _float& fTimeDelta);

public:
    static CNewClass* Create();

private:
    virtual void Free() override;
};
```

2. **구현 파일 생성** (`Editor/Code/CNewClass.cpp`)
```cpp
#include "pch.h"
#include "CNewClass.h"

CNewClass::CNewClass()
{
}

CNewClass::~CNewClass()
{
    Free();
}

HRESULT CNewClass::Ready_NewClass()
{
    return S_OK;
}

CNewClass* CNewClass::Create()
{
    CNewClass* pInstance = new CNewClass;
    if (FAILED(pInstance->Ready_NewClass()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CNewClass Create Failed");
        return nullptr;
    }
    return pInstance;
}

void CNewClass::Free()
{
    // 리소스 정리
}
```

3. **Visual Studio 필터에 추가**
   - 솔루션 탐색기에서 적절한 필터에 헤더/소스 파일 추가

### 맵 저장/로드 테스트

```bash
# 1. Editor 실행
F5

# 2. 오브젝트 배치
- ToolBar에서 "Place Tile" 또는 "Place Cube" 선택
- Scene View에서 클릭하여 배치

# 3. 맵 저장
- File > Save Map (Ctrl+S)
- 파일명 입력: "test.json"
- Map/ 폴더에 저장됨

# 4. JSON 파일 확인
- 메모장으로 test.json 열기
- 내용 검증 (position, rotation, scale 확인)

# 5. 맵 로드
- File > New Map (모든 오브젝트 삭제)
- File > Open Map (Ctrl+O)
- test.json 선택
- 모든 오브젝트가 정확히 복원되는지 확인
```

### 디버깅

```cpp
// OutputDebugString 사용 (Visual Studio Output 창에 출력)
char szDebug[256];
sprintf_s(szDebug, "Position: (%f, %f, %f)\n", vPos.x, vPos.y, vPos.z);
OutputDebugStringA(szDebug);

// 메시지 박스
MSG_BOX("This is an error message");

// JSON 파싱 에러 확인
try {
    // JSON 작업
} catch (const json::exception& e) {
    char szError[512];
    sprintf_s(szError, "JSON Error: %s", e.what());
    MessageBoxA(nullptr, szError, "Error", MB_OK);
}
```

## 알려진 이슈 및 제한사항

1. **TerrainTex 리소스 없음**
   - Height.bmp 파일이 없어 CEditorTerrain 미구현
   - 필요 시 128x128 이상의 높이맵 추가

2. **텍스처 시스템 미구현 (Phase 7)**
   - 현재는 기본 텍스처만 표시됨
   - Inspector에서 텍스처 선택 불가

3. **Undo/Redo 미구현**
   - Edit > Undo/Redo 메뉴는 있지만 기능 없음
   - Phase 8 이후 구현 예정

## 추가 정보

**외부 라이브러리:**
- ImGui (v1.89 이상) - UI 시스템
- nlohmann/json (v3.11.3) - JSON 파서 (헤더 온리)
- DirectX 9 SDK (2010)

**참고 자료:**
- ImGui GitHub: https://github.com/ocornut/imgui
- nlohmann/json: https://github.com/nlohmann/json
- DirectX 9 Documentation

**문제 발생 시:**
1. 해당 Phase 구현 가이드 확인
2. `Editor_진행상황_2025-12-27.md` 참조
3. 기존 완료된 Phase 코드 참고
