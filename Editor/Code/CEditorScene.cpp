#include "pch.h"
#include "CEditorScene.h"
#include "CDInputMgr.h"

#include "CEditorCamera.h"
#include "CGrid.h"
#include "CEditorObject.h"
#include "CEditorFloor.h"
#include "CEditorSlopeFloor.h"
#include "CEditorVendingMachine.h"
#include "CEditorCeiling.h"
#include "CEditorWall.h"
#include "CEditorSpawnPoint.h"
#include "CToolBar.h"
#include "CMousePicker.h"
#include "CSelectionMgr.h"
#include "CHierarchy.h"
#include "CEditorDynamicFloor.h"
#include "CEditorDynamicWall.h"
#include "CEditorMapCollider.h"
#include "CEditorTriggerBox.h"
#include "CEditorDoor.h"

CEditorScene::CEditorScene(LPDIRECT3DDEVICE9 pGraphicDev)
    : CScene(pGraphicDev)
    , m_pGraphicDev(pGraphicDev)
    , m_pCamera(nullptr)
    , m_pGrid(nullptr)
    , m_pToolBar(nullptr)
    , m_pMousePicker(nullptr)
    , m_pSelectionMgr(nullptr)
    , m_pHierarchy(nullptr)
    , m_eDupplicateDir(POSITIVE_X)
    , m_vDupplicateDir(1.f, 0.f, 0.f)
{
    m_pGraphicDev->AddRef();
}

CEditorScene::~CEditorScene()
{
}

HRESULT CEditorScene::Ready_Scene()
{
    m_pCamera = CEditorCamera::Create(m_pGraphicDev);
    if (nullptr == m_pCamera)
    {
        MSG_BOX("EditorCamera Create Failed");
        return E_FAIL;
    }

    m_pCamera->Set_Position(_vec3(0.f, 20.f, 0.f));
    m_pCamera->Set_LookAt(_vec3(0.f, 0.f, 1.f));

    // 그리드 개수, 그리드 간 간격 지정 
    m_pGrid = CGrid::Create(m_pGraphicDev,
                            300,    // X축 개수 
                            300,    // Z축 개수
                            16.f);  // 그리드 한 칸당 크기 ( 되도록이면 타일과 동일한 사이즈로 설정 )
    if (nullptr == m_pGrid)
    {
        MSG_BOX("Grid Create Failed");
        return E_FAIL;
    }

    m_pGrid->Set_Visible(true);

    // MousePicker 생성
    m_pMousePicker = CMousePicker::Create();
    if (nullptr == m_pMousePicker)
    {
        MSG_BOX("MouserPicker Create Failed");
        return E_FAIL;
    }

    // SelectionMgr 생성
    m_pSelectionMgr = CSelectionMgr::Create();
    if (nullptr == m_pSelectionMgr)
    {
        MSG_BOX("SelectionMgr Create Failed");
        return E_FAIL;
    }

    return S_OK;
}

_int CEditorScene::Update_Scene(const _float& fTimeDelta)
{
    if (m_pCamera)
        m_pCamera->Update_GameObject(fTimeDelta);

    Handle_Input();

    for (auto& pObj : m_ObjectList)
    {
        pObj->Update_GameObject(fTimeDelta);
    }

    Handle_Arrow();
    return 0;
}

void CEditorScene::LateUpdate_Scene(const _float& fTimeDelta)
{
    for (auto& pObj : m_ObjectList)
    {
        pObj->LateUpdate_GameObject(fTimeDelta);
    }
}

void CEditorScene::Render_Scene()
{
    if (nullptr == m_pCamera)
        return;

    // 1. View/Proj Matrix 
    _matrix matView, matProj;
    m_pCamera->Get_ViewMatrix(&matView);
    m_pCamera->Get_ProjMatrix(&matProj);

    m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);
    m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);

    // 2. 그리드 그리기
    if (m_pGrid)
        m_pGrid->Render_GameObject();
}

void CEditorScene::Add_Object(CEditorObject* pObject)
{
    if (nullptr == pObject)
        return;

    m_ObjectList.push_back(pObject);
    pObject->AddRef();
}

void CEditorScene::Remove_Object(CEditorObject* pObject)
{
    auto iter = find(m_ObjectList.begin(), m_ObjectList.end(), pObject);

    if (iter != m_ObjectList.end())
    {
        Safe_Release(*iter);
        m_ObjectList.erase(iter);
    }
}

void CEditorScene::Clear_AllObjects()
{
    // 선택 해제
    Set_SelectedObject(nullptr);

    // 모든 오브젝트 Release
    for (auto& pObj : m_ObjectList)
    {
        Safe_Release(pObj);
    }

    m_ObjectList.clear();

    // Hierarchy 동기화
    if (m_pHierarchy)
    {
        m_pHierarchy->Sync_Selection(nullptr);
    }
}

void CEditorScene::Set_SelectedObject(CEditorObject* pObj)
{
    if (m_pSelectionMgr)
        m_pSelectionMgr->Set_Selection(pObj);

    if (m_pHierarchy)
        m_pHierarchy->Sync_Selection(pObj);
}

CEditorObject* CEditorScene::Get_SelectedObject() const
{
    if (m_pSelectionMgr)
        return m_pSelectionMgr->Get_Selection();

    return nullptr;
}

list<CEditorObject*>& CEditorScene::Get_SelectedObjects()
{
    if (m_pSelectionMgr)
        return m_pSelectionMgr->Get_AllSelections();
}

void CEditorScene::Add_SelectedObject(CEditorObject* pObj)
{
    if (!pObj || !m_pSelectionMgr)
        return;

    // CSelctionMgr에 추가
    m_pSelectionMgr->Add_Selection(pObj);

    // Hierarchy 동기화
    if (m_pHierarchy)
    {
        CEditorObject* pPrimary = m_pSelectionMgr->Get_Selection();
        m_pHierarchy->Sync_Selection(pPrimary);
    }
}

void CEditorScene::Remove_SelectedObject(CEditorObject* pObj)
{
    if (!pObj || !m_pSelectionMgr)
        return;

    // CSelectionMgr 에서 제거
    m_pSelectionMgr->Remove_Selection(pObj);

    // Primary 선택 객체가 변경되었을 수 있으므로 동기화
    if (m_pHierarchy)
    {
        CEditorObject* pPrimary = m_pSelectionMgr->Get_Selection();
        m_pHierarchy->Sync_Selection(pPrimary);
    }
}

void CEditorScene::Clear_SelectedObjects()
{
    if (!m_pSelectionMgr)
        return;

    // 모든 선택 해제
    m_pSelectionMgr->Clear_Selection();

    // Hierarchy 동기화
    if (m_pHierarchy)
        m_pHierarchy->Sync_Selection(nullptr);
}



void CEditorScene::Handle_Input()
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
        return;     // ImGui 윈도우 클릭 시 무시 

    Handle_Duplicate();

    Handle_Delete();

    Handle_Left_Click();
}

void CEditorScene::Handle_Duplicate()
{
    // Ctrl + D  : 객체 복제
    if (Engine::CDInputMgr::GetInstance()->Key_Down(DIK_D) &&
        Engine::CDInputMgr::GetInstance()->Key_Pressing(DIK_LCONTROL))
    {
        list<CEditorObject*>& SelectedList = m_pSelectionMgr->Get_AllSelections();

        if (!SelectedList.empty())
        {
            list<CEditorObject*> newObjects;

            for (auto& pSelectedObj : SelectedList)
            {
                if (!pSelectedObj)
                    continue;

                // 현재 Transform 값 가져오기
                _vec3 vPos = pSelectedObj->Get_Position();
                _vec3 vRot = pSelectedObj->Get_Rotation();
                _vec3 vScale = pSelectedObj->Get_Scale();

                CEditorObject* pNewObj = nullptr;
                _uint iType(0);
                _int iIdx(0);
                _int iRoomIndex(0);

                // 타입별 복제
                if (CEditorDynamicFloor* pDynamicFloor = static_cast<CEditorDynamicFloor*>(pSelectedObj))
                {
                    iType = pDynamicFloor->Get_FloorType();
                    iRoomIndex = pDynamicFloor->Get_RoomIndex();
                    
                    vPos += m_vDupplicateDir * 16.f;
                    pNewObj = CEditorDynamicFloor::Create(m_pGraphicDev, vPos, vRot, vScale, iType);
                }
                else if (CEditorSlopeFloor* pSlopeFloor = static_cast<CEditorSlopeFloor*>(pSelectedObj))
                {
                    iType = pSlopeFloor->Get_FloorType();
                    _float fSlopeAngle = pSlopeFloor->Get_SlopeAngle();
                    SLOPE_DIR eSlopeDir = pSlopeFloor->Get_SlopeDirection();
                    iRoomIndex = pSlopeFloor->Get_RoomIndex();

                    vPos = pSlopeFloor->Get_OppositeEndPosition(m_vDupplicateDir);

                    pNewObj = CEditorSlopeFloor::Create(m_pGraphicDev, vPos, vScale,
                                                        iType, fSlopeAngle, eSlopeDir);
                }
                else if (CEditorFloor* pFloor = static_cast<CEditorFloor*>(pSelectedObj))
                {
                    iType       =  pFloor->Get_FloorType();
                    iIdx        = pFloor->Get_TextureIdx();
                    iRoomIndex  = pFloor->Get_RoomIndex();

                    vPos += m_vDupplicateDir * 16.f;
                    pNewObj = CEditorFloor::Create(m_pGraphicDev, vPos, vRot, vScale, iType, iIdx);
                }
                
                else if (CEditorCeiling* pCeiling = static_cast<CEditorCeiling*>(pSelectedObj))
                {
                    iType   = pCeiling->Get_CeilingType();
                    iIdx    = pCeiling->Get_TextureIdx();
                    iRoomIndex = pCeiling->Get_RoomIndex();

                    vPos += m_vDupplicateDir * 16.f;
                    pNewObj = CEditorCeiling::Create(m_pGraphicDev, vPos, vRot, vScale, iType, iIdx);
                }
                else if (CEditorVendingMachine* pMachine =  static_cast<CEditorVendingMachine*>(pSelectedObj))
                {
                    iRoomIndex = pMachine->Get_RoomIndex();

                    vPos += m_vDupplicateDir * 16.f;
                    pNewObj = CEditorVendingMachine::Create(m_pGraphicDev, vPos, vRot, vScale);
                    pNewObj->Set_RoomIndex(iRoomIndex);
                }
                else if (CEditorDynamicWall* pDynamicWall = static_cast<CEditorDynamicWall*>(pSelectedObj))
                {
                    WALL_DIR eDir = pDynamicWall->Get_WallDirection();

                    vPos += m_vDupplicateDir * 32.f;

                    iType = pDynamicWall->Get_WallType();
                    iIdx = pDynamicWall->Get_TextureIdx();
                    iRoomIndex = pDynamicWall->Get_RoomIndex();

                    pNewObj = CEditorDynamicWall::Create(m_pGraphicDev, vPos, vRot, vScale, eDir, iType, iIdx);
                }
                else if (CEditorWall* pWall = static_cast<CEditorWall*>(pSelectedObj))
                {
                    WALL_DIR eDir = pWall->Get_WallDirection();

                    vPos += m_vDupplicateDir * 32.f;

                    iType = pWall->Get_WallType();
                    iIdx = pWall->Get_TextureIdx();
                    iRoomIndex = pWall->Get_RoomIndex();

                    pNewObj = CEditorWall::Create(m_pGraphicDev, vPos, vRot, vScale, eDir, iType, iIdx);
                }
                else if (CEditorSpawnPoint* pSpawn = static_cast<CEditorSpawnPoint*>(pSelectedObj))
                {
                    SPAWN_TYPE eType = pSpawn->Get_SpawnType();
                    iRoomIndex = pSpawn->Get_RoomIndex();

                    // Player 타입은 복제 대신 위치 이동 옵션 제공
                    if (eType == SPAWN_PLAYER)
                    {
                        wchar_t szMsg[1024];
                        swprintf_s(szMsg,
                            L"플레이어 스폰 위치 복사 불가능.\n\n"
                            L"새로운 위치로 플레이어 스폰 위치 이동하시겠습니까?\n\n"
                            L"새로운 위치: (%.1f, %.1f, %.1f)\n\n"
                            L"YES: 옮기기\n"
                            L"NO: 취소",
                            vPos.x, vPos.y, vPos.z);

                        int iResult = MessageBoxW(nullptr, szMsg,
                            L"플레이어 스폰 오브젝트 복사 시도?",
                            MB_YESNO | MB_ICONQUESTION);

                        if (iResult == IDYES)
                        {
                            // 기존 Player 위치 이동
                            pSpawn->Set_Position(vPos);
                        }
                        continue;  // 복제는 하지 않음
                    }

                    // Monster 타입만 복제 허용
                    const string& strMonsterKey = pSpawn->Get_MonsterKey();
                    pNewObj = CEditorSpawnPoint::Create(m_pGraphicDev, vPos, vRot, vScale,
                        eType, strMonsterKey);
                }
                else if (CEditorMapCollider* pMapCollider = static_cast<CEditorMapCollider*>(pSelectedObj))
                {
                    vPos += m_vDupplicateDir * 16.f;
                    _vec3 vColliderScale = pMapCollider->Get_ColliderScale();
                    pNewObj = CEditorMapCollider::Create(m_pGraphicDev, vPos, vColliderScale);
                }
                else if (CEditorDoor* pDoor = static_cast<CEditorDoor*>(pSelectedObj))
                {
                    vPos += m_vDupplicateDir * 16.f;
                    DOOR_TYPE iDoorType = pDoor->Get_DoorType();
                    _int iDoorID = pDoor->Get_DoorID();
                    iRoomIndex = pDoor->Get_RoomIndex();
                    pNewObj = CEditorDoor::Create(m_pGraphicDev, vPos, vRot, vScale, iDoorType, iDoorID);
                }
                else if (CEditorTriggerBox* pTriggerBox = static_cast<CEditorTriggerBox*>(pSelectedObj))
                {
                    vPos += m_vDupplicateDir * 16.f;
                    _vec3 vColliderScale = pTriggerBox->Get_ColliderScale();
                    TRIGGER_TYPE eType = pTriggerBox->Get_TriggerType();
                    pNewObj = CEditorTriggerBox::Create(m_pGraphicDev, vPos, vColliderScale, eType);
                    }


                if (pNewObj)
                {
                    pNewObj->Set_RoomIndex(iRoomIndex); // 방 정보 설정 
                    m_ObjectList.push_back(pNewObj);
                    newObjects.push_back(pNewObj);
                }
            }

            // 복제된 객체들 선택 설정
            if (!newObjects.empty())
            {
                Clear_SelectedObjects();

                for (auto& pNewObj : newObjects)
                {
                    Add_SelectedObject(pNewObj);
                }
            }
        }
    }
}

void CEditorScene::Handle_Delete()
{
    // Delete 키 : 선택된 객체 삭제
    if (Engine::CDInputMgr::GetInstance()->Key_Down(DIK_DELETE))
    {
        if (!m_pSelectionMgr)
            return;

        list<CEditorObject*>& SelectedObjList = m_pSelectionMgr->Get_AllSelections();


        if (!SelectedObjList.empty())
        {
            list<CEditorObject*> ObjectsToDelete;
            for (auto& pObj : SelectedObjList)
            {
                ObjectsToDelete.push_back(pObj);
            }

            // 선택 해제
            Clear_SelectedObjects();

            // 객체 삭제
            for (auto& pObj : ObjectsToDelete)
            {
                if (!pObj)
                    continue;

                auto iter = find(m_ObjectList.begin(), m_ObjectList.end(), pObj);
                if (iter != m_ObjectList.end())
                {
                    Safe_Release(*iter);
                    m_ObjectList.erase(iter);
                }
            }
        }
    }
}

void CEditorScene::Handle_Left_Click()
{
    if (Engine::CDInputMgr::GetInstance()->Mouse_Down(DIM_LB))
    {
        if (!m_pToolBar)
            return;

        EDITOR_MODE eMode = m_pToolBar->Get_EditorMode();

        // Ray 계산 (공통)
        POINT ptMouse;
        GetCursorPos(&ptMouse);
        ScreenToClient(g_hWnd, &ptMouse);

        _matrix matView, matProj;
        m_pCamera->Get_ViewMatrix(&matView);
        m_pCamera->Get_ProjMatrix(&matProj);

        m_pMousePicker->Update(matView, matProj, ptMouse);

        _vec3 vRayPos = m_pMousePicker->Get_RayPos();
        _vec3 vRayDir = m_pMousePicker->Get_RayDir();

        if (eMode == MODE_PLACE_FLOOR || eMode == MODE_PLACE_DYNAMIC_FLOOR || 
            eMode == MODE_PLACE_CEILING || eMode == MODE_PLACE_VENDINGMACHINE || 
            eMode == MODE_PLACE_WALL || eMode == MODE_PLACE_SPAWN_PLAYER || 
            eMode == MODE_PLACE_SPAWN_MONSTER || eMode == MODE_PLACE_SLOPE_FLOOR ||
            eMode == MODE_PLACE_MAPCOLLIDER || eMode == MODE_PLACE_TRIGGERBOX ||
            eMode == MODE_PLACE_SPAWN_BOSSMONSTER || eMode == MODE_PLACE_DYNAMIC_WALL ||
            eMode == MODE_PLACE_DOOR)
        {

            // Ray - Plane Intersection (Y = 0 평면)
            _vec3 vPos = Pick_OnPlane(vRayPos, vRayDir, 0.f);

            // 오브젝트 배치
            if (eMode == MODE_PLACE_FLOOR)
                Place_Floor(vPos);
            else if (eMode == MODE_PLACE_SLOPE_FLOOR)
                Place_Slope_Floor(vPos);
            else if (eMode == MODE_PLACE_DYNAMIC_FLOOR)
                Place_Dynamic_Floor(vPos);
            else if (eMode == MODE_PLACE_CEILING)
                Place_Ceiling(vPos);
            else if (eMode == MODE_PLACE_VENDINGMACHINE)
                Place_VendingMachine(vPos);
            else if (eMode == MODE_PLACE_WALL)
                Place_Wall(vPos);
            else if (eMode == MODE_PLACE_DYNAMIC_WALL)
                Place_Dynamic_Wall(vPos);
            else if (eMode == MODE_PLACE_SPAWN_PLAYER)
                Place_SpawnPlayer(vPos);
            else if (eMode == MODE_PLACE_SPAWN_MONSTER)
                Place_SpawnMonster(vPos);
            else if (eMode == MODE_PLACE_MAPCOLLIDER)
                Place_MapCollider(vPos);
            else if (eMode == MODE_PLACE_TRIGGERBOX)
                Place_TriggerBox(vPos);
            else if (eMode == MODE_PLACE_SPAWN_BOSSMONSTER)
                Place_SpawnBossMonster(vPos);
            else if (eMode == MODE_PLACE_DOOR)
                Place_Door(vPos);
        }
        else if (eMode == MODE_SELECT)
        {
            static POINT ptPrevMouse = { 0, 0 };

            CEditorObject* pPickedObject = m_pSelectionMgr->Pick_Object(vRayPos, vRayDir, m_ObjectList);

            _bool bCtrlPressed = Engine::CDInputMgr::GetInstance()->Key_Pressing(DIK_LCONTROL);
            _bool bShiftPressed = Engine::CDInputMgr::GetInstance()->Key_Pressing(DIK_LSHIFT);

            if (pPickedObject)
            {
                if (bCtrlPressed && bShiftPressed)
                {
                    // Ctrl + Shift + 클릭 : 선택 해제
                    Remove_SelectedObject(pPickedObject);
                }
                else if (bCtrlPressed)
                {
                    // Ctrl + 클릭 : 다중 선택 토글
                    if (m_pSelectionMgr->Is_Selected(pPickedObject))
                    {
                        // 이미 선택됨 -> 선택 해제
                        Remove_SelectedObject(pPickedObject);
                    }
                    else
                    {
                        // 선택 안됨 -> 선택 추가
                        Add_SelectedObject(pPickedObject);
                    }
                }
                else
                {
                    // 일반 클릭 : 단일 선택 ( 기존 선택 모두 해제 )
                    Set_SelectedObject(pPickedObject);
                }
            }
            else
            {
                // 빈 공간 클릭 
                if (!bCtrlPressed)
                {
                    // Ctrl 안 눌렀을 때만 모든 선택 해제 
                    Clear_SelectedObjects();
                }
            }

            ptPrevMouse = ptMouse;
        }
    }
}

void CEditorScene::Handle_Arrow()
{
    if (CDInputMgr::GetInstance()->Key_Down(DIK_UP))
        m_eDupplicateDir = POSITIVE_X;
    else if (CDInputMgr::GetInstance()->Key_Down(DIK_DOWN))
        m_eDupplicateDir = NEGATIVE_X;
    else if (CDInputMgr::GetInstance()->Key_Down(DIK_RIGHT))
        m_eDupplicateDir = POSITIVE_Z;
    else if (CDInputMgr::GetInstance()->Key_Down(DIK_LEFT))
        m_eDupplicateDir = NEGATIVE_Z;

    switch (m_eDupplicateDir)
    {
    case POSITIVE_X:
        m_vDupplicateDir = { 1.f, 0.f, 0.f };
        break;
    case NEGATIVE_X:
        m_vDupplicateDir = { -1.f, 0.f, 0.f };
        break;
    case POSITIVE_Z:
        m_vDupplicateDir = { 0.f, 0.f, 1.f };
        break;
    case NEGATIVE_Z:
        m_vDupplicateDir = { 0.f, 0.f, -1.f };
        break;
    default:
        m_vDupplicateDir = { 1.f, 0.f, 0.f };
        break;
    }
}

_vec3 CEditorScene::Pick_OnPlane(const _vec3& vRayPos, const _vec3& vRayDir, _float fPlaneY)
{
    // Ray-Plane Intersection 계산
    // Ray: P = RayPos + t * RayDir
    // Plane: Y = fPlaneY
    // 교차점: RayPos.y + t * RayDir.y = fPlaneY
    // 해: t = (fPlaneY - RayPos.y) / RayDir.y

    if (abs(vRayDir.y) < 0.0001f)  // Ray가 평면과 평행
    {
        // 기본값 반환
        return _vec3(0.f, fPlaneY, 0.f);
    }

    _float t = (fPlaneY - vRayPos.y) / vRayDir.y;

    // t가 음수면 Ray가 평면 뒤쪽을 향함
    if (t < 0.f)
    {
        return _vec3(0.f, fPlaneY, 0.f);
    }

    // 교차점 계산
    _vec3 vIntersection = vRayPos + vRayDir * t;

    return vIntersection;
}

void CEditorScene::Place_Floor(const _vec3& vPos)
{
    CEditorFloor* pFloor = CEditorFloor::Create(m_pGraphicDev, vPos);

    if (pFloor)
    {
        Add_Object(pFloor);
        Safe_Release(pFloor);
    }
}

void CEditorScene::Place_Dynamic_Floor(const _vec3& vPos)
{
    CEditorDynamicFloor* pDynamicFloor = CEditorDynamicFloor::Create(m_pGraphicDev, vPos);

    if (pDynamicFloor)
    {
        Add_Object(pDynamicFloor);
        Safe_Release(pDynamicFloor);
    }
}

void CEditorScene::Place_Slope_Floor(_vec3 vPos)
{
    // 스냅 처리 ( 그리드 크기 : 16 )
    vPos.x = floorf(vPos.x / 16.f) * 16.f + 8.f;
    vPos.z = floorf(vPos.z / 16.f) * 16.f + 8.f;
    vPos.y = 0.f;

    CEditorSlopeFloor* pSlopeFloor = CEditorSlopeFloor::Create(m_pGraphicDev, vPos);

    if (pSlopeFloor)
    {
        Add_Object(pSlopeFloor);
        Safe_Release(pSlopeFloor);
    }
}

void CEditorScene::Place_Ceiling(const _vec3& vPos)
{
    CEditorCeiling* pCeiling = CEditorCeiling::Create(m_pGraphicDev, vPos);

    if (pCeiling)
    {
        // Y 위치 조정 (천장은 바닥보다 위)
        _vec3 vAdjustedPos = vPos;
        vAdjustedPos.y = vPos.y + 32.0f;  
        pCeiling->Set_Position(vAdjustedPos);

        Add_Object(pCeiling);
        Safe_Release(pCeiling);
    }
}

void CEditorScene::Place_Wall(const _vec3& vPos)
{
    // 기본 XY 평면 벽 배치
    CEditorWall* pWall = CEditorWall::Create(m_pGraphicDev, vPos, WALL_XY_FRONT);

    if (pWall)
    {
        // Y 위치 조정 (벽 중심이 바닥보다 위)
        _vec3 vAdjustedPos = vPos;
        vAdjustedPos.y = vPos.y + 16.0f;  
        pWall->Set_Position(vAdjustedPos);

        Add_Object(pWall);
        Safe_Release(pWall);
    }
}

void CEditorScene::Place_Dynamic_Wall(const _vec3& vPos)
{
    // 기본 XY 평면 벽 배치
    CEditorDynamicWall* pDynamicWall = CEditorDynamicWall::Create(m_pGraphicDev, vPos, WALL_XY_FRONT);

    if (pDynamicWall)
    {
        // Y 위치 조정 (벽 중심이 바닥보다 위)
        _vec3 vAdjustedPos = vPos;
        vAdjustedPos.y = vPos.y + 16.0f;
        pDynamicWall->Set_Position(vAdjustedPos);

        Add_Object(pDynamicWall);
        Safe_Release(pDynamicWall);
    }
}

void CEditorScene::Place_Door(const _vec3& vPos)
{
    // 그리드 스냅 (16 단위)
    _vec3 vSnappedPos;
    vSnappedPos.x = floorf(vPos.x / 16.f) * 16.f + 8.f;
    vSnappedPos.y = vPos.y + 16.0f;  // 바닥에서 문 중심까지의 높이
    vSnappedPos.z = floorf(vPos.z / 16.f) * 16.f + 8.f;

    CEditorDoor* pDoor = CEditorDoor::Create(m_pGraphicDev, vSnappedPos);

    if (pDoor)
    {
        // 기본 이름 설정
        static _int s_iDoorIdx = 0;
        wchar_t wszName[64];
        swprintf_s(wszName, L"Door_%d", s_iDoorIdx++);
        pDoor->Set_Name(wszName);

        Add_Object(pDoor);
        Safe_Release(pDoor);
    }
}

void CEditorScene::Place_SpawnPlayer(const _vec3& vPos)
{
    CEditorSpawnPoint* pExistingPlayer = nullptr;
    for (auto& pObj : m_ObjectList)
    {
        CEditorSpawnPoint* pSpawn = static_cast<CEditorSpawnPoint*>(pObj);
        if (pSpawn && pSpawn->Get_SpawnType() == SPAWN_PLAYER)
        {
            pExistingPlayer = pSpawn;
            break;
        }
    }

    if (pExistingPlayer)
    {
        int iResult = MessageBoxW(nullptr,
            L"이미 플레이어 스폰지점이 존재합니다.\n"
            L"새로운 스폰 지점으로 대체합니까?\n",
            L"플레이어 스폰지점 존재",
            MB_YESNO | MB_ICONWARNING);

        if (iResult == IDYES)
        {
            m_ObjectList.remove(pExistingPlayer);
            Safe_Release(pExistingPlayer);
        }
        else
            return;
    }

    
    CEditorSpawnPoint* pSpawn = CEditorSpawnPoint::Create(m_pGraphicDev, vPos, SPAWN_PLAYER);

    if (pSpawn)
    {
        _vec3 vAdjustedPos = vPos;
        vAdjustedPos.y = vPos.y + 20.f;  
        pSpawn->Set_Position(vAdjustedPos);

        Add_Object(pSpawn);
        Safe_Release(pSpawn);
    }
}

void CEditorScene::Place_SpawnMonster(const _vec3& vPos)
{
    CEditorSpawnPoint* pSpawn = CEditorSpawnPoint::Create(m_pGraphicDev, vPos, SPAWN_MONSTER);

    if (pSpawn)
    {
        // Y 위치 조정
        _vec3 vAdjustedPos = vPos;
        vAdjustedPos.y = vPos.y + 0.5f;
        pSpawn->Set_Position(vAdjustedPos);

        // 기본 몬스터 키 설정 (Inspector에서 변경 가능하도록 향후 확장)
        pSpawn->Set_MonsterKey("DefaultMonster");

        Add_Object(pSpawn);
        Safe_Release(pSpawn);
    }
}

void CEditorScene::Place_SpawnBossMonster(const _vec3& vPos)
{
    CEditorSpawnPoint* pSpawn = CEditorSpawnPoint::Create(m_pGraphicDev, vPos, SPAWN_BOSSMONSTER);

    if (pSpawn)
    {
        _vec3 vAdjustedPos = vPos;
        vAdjustedPos.y = vPos.y + 100.f;
        pSpawn->Set_Position(vAdjustedPos);

        Add_Object(pSpawn);
        Safe_Release(pSpawn);
    }
}

void CEditorScene::Place_MapCollider(const _vec3& vPos)
{
    // 그리드 스냅 (16 단위)
    _vec3 vSnappedPos;
    vSnappedPos.x = floorf(vPos.x / 16.f) * 16.f + 8.f;
    vSnappedPos.y = 8.f;  // 기본 높이 (바닥에서 살짝 위)
    vSnappedPos.z = floorf(vPos.z / 16.f) * 16.f + 8.f;

    CEditorMapCollider* pMapCollider = CEditorMapCollider::Create(m_pGraphicDev, vSnappedPos);

    if (pMapCollider)
    {
        // 기본 이름 설정
        static _int s_iMapColliderIdx = 0;
        wchar_t wszName[64];
        swprintf_s(wszName, L"MapCollider_%d", s_iMapColliderIdx++);
        pMapCollider->Set_Name(wszName);

        Add_Object(pMapCollider);
        Safe_Release(pMapCollider);
    }
}

void CEditorScene::Place_TriggerBox(const _vec3& vPos)
{
    // 그리드 스냅 (16 단위)
    _vec3 vSnappedPos;
    vSnappedPos.x = floorf(vPos.x / 16.f) * 16.f + 8.f;
    vSnappedPos.y = 8.f;  // 기본 높이
    vSnappedPos.z = floorf(vPos.z / 16.f) * 16.f + 8.f;

    CEditorTriggerBox* pTriggerBox = CEditorTriggerBox::Create(m_pGraphicDev, vSnappedPos);

    if (pTriggerBox)
    {
        // 기본 이름 설정
        static _int s_iTriggerBoxIdx = 0;
        wchar_t wszName[64];
        swprintf_s(wszName, L"TriggerBox_%d", s_iTriggerBoxIdx++);
        pTriggerBox->Set_Name(wszName);

        Add_Object(pTriggerBox);
        Safe_Release(pTriggerBox);
    }
}

void CEditorScene::Place_VendingMachine(const _vec3& vPos)
{
    CEditorVendingMachine* pMachine = CEditorVendingMachine::Create(m_pGraphicDev, vPos);

    if (pMachine)
    {
        _vec3 vAdjustedPos = vPos;
        vAdjustedPos.y = vPos.y + 12.0f;    // 설정된 Scale만큼
        pMachine->Set_Position(vAdjustedPos);

        Add_Object(pMachine);
        Safe_Release(pMachine);
    }
}


CEditorScene* CEditorScene::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CEditorScene* pInstance = new CEditorScene(pGraphicDev);

    if (FAILED(pInstance->Ready_Scene()))
    {
        Safe_Release(pInstance);
        MSG_BOX("EditorScene Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CEditorScene::Free()
{
    Clear_AllObjects();

    Safe_Release(m_pSelectionMgr);
    Safe_Release(m_pMousePicker);
    Safe_Release(m_pGrid);
    Safe_Release(m_pCamera);
    Safe_Release(m_pGraphicDev);

    CScene::Free();
}