#include "pch.h"
#include "CEditorScene.h"
#include "CDInputMgr.h"

#include "CEditorCamera.h"
#include "CGrid.h"
#include "CEditorObject.h"
#include "CEditorTile.h"
#include "CEditorCube.h"
#include "CToolBar.h"
#include "CMousePicker.h"
#include "CSelectionMgr.h"
#include "CHierarchy.h"

CEditorScene::CEditorScene(LPDIRECT3DDEVICE9 pGraphicDev)
    : CScene(pGraphicDev)
    , m_pGraphicDev(pGraphicDev)
    , m_pCamera(nullptr)
    , m_pGrid(nullptr)
    , m_pToolBar(nullptr)
    , m_pMousePicker(nullptr)
    , m_pSelectionMgr(nullptr)
    , m_pHierarchy(nullptr)
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

    m_pCamera->Set_Position(_vec3(0.f, 10.f, -10.f));
    m_pCamera->Set_LookAt(_vec3(0.f, 0.f, 0.f));

    // 그리드 개수, 그리드 간 간격 지정 
    m_pGrid = CGrid::Create(m_pGraphicDev, 100, 100, 2.f);
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

    return 0;
}

void CEditorScene::LateUpdate_Scene(const _float& fTimeDelta)
{
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

                // 위치 오프셋 (겹치지 않게)
                vPos.x += 1.0f;

                CEditorObject* pNewObj = nullptr;

                // 타입별 복제
                if (dynamic_cast<CEditorTile*>(pSelectedObj))
                {
                    pNewObj = CEditorTile::Create(m_pGraphicDev, vPos, vRot, vScale);
                }
                else if (dynamic_cast<CEditorCube*>(pSelectedObj))
                {
                    pNewObj = CEditorCube::Create(m_pGraphicDev, vPos, vRot, vScale);
                }

                if (pNewObj)
                {
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

        if (eMode == MODE_PLACE_TILE || eMode == MODE_PLACE_CUBE)
        {

            // Ray - Plane Intersection ( Y = 0 평면 )
            _vec3 vPos = Pick_OnPlane(vRayPos, vRayDir, 0.f);

            // 배치 
            if (eMode == MODE_PLACE_TILE)
                Place_Tile(vPos);
            else if (eMode == MODE_PLACE_CUBE)
                Place_Cube(vPos);
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

void CEditorScene::Place_Tile(const _vec3& vPos)
{
    CEditorTile* pTile = CEditorTile::Create(m_pGraphicDev, vPos);

    if (pTile)
    {
        Add_Object(pTile);
        Safe_Release(pTile);  // Add_Object에서 AddRef했으므로 Release
    }
}

void CEditorScene::Place_Cube(const _vec3& vPos)
{
    CEditorCube* pCube = CEditorCube::Create(m_pGraphicDev, vPos);

    if (pCube)
    {
        // 큐브는 Y 위치를 0.5f로 조정 (바닥에서 절반 높이)
        _vec3 vAdjustedPos = vPos;
        vAdjustedPos.y = vPos.y + 1.0f;
        pCube->Set_Position(vAdjustedPos);

        Add_Object(pCube);
        Safe_Release(pCube);
    }
}

// =======================================================

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