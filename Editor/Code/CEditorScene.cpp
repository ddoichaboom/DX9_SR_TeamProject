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
    , m_bPrevMouseLeft(false)
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
    for_each(m_ObjectList.begin(), m_ObjectList.end(), CDeleteObj());
    m_ObjectList.clear();
}

void CEditorScene::Set_SelectedObject(CEditorObject* pObj)
{
    if (m_pSelectionMgr)
        m_pSelectionMgr->Set_Selection(pObj);

    if (m_pHierarchy)
        m_pHierarchy->Sync_Selection(pObj);
}

CEditorObject* CEditorScene::Get_SelectedObject()
{
    if (m_pSelectionMgr)
        return m_pSelectionMgr->Get_Selection();

    return nullptr;
}

void CEditorScene::Handle_Input()
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
        return;     // ImGui 윈도우 클릭 시 무시 

    // 현재 프레임 마우스 상태
    bool bCurMouseLeft = (Engine::CDInputMgr::GetInstance()->Get_DIMouseState(DIM_LB) & 0x80) != 0;

    if (!m_bPrevMouseLeft && bCurMouseLeft)
    {
        if (!m_pToolBar)
        {
            m_bPrevMouseLeft = bCurMouseLeft;       // 상태 업데이트
            return;
        }

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
            // Ray - AABB Instersection (오브젝트 선택)

//            CEditorObject* pPickedObject = m_pSelectionMgr->Pick_Object_Cycle(
//                vRayPos, vRayDir, m_ObjectList, ptMouse, ptPrevMouse, 5);

            CEditorObject* pPickedObject = m_pSelectionMgr->Pick_Object(
                vRayPos, vRayDir, m_ObjectList);

            if (pPickedObject)
            {
                Set_SelectedObject(pPickedObject);
            }
            else
            {
                Set_SelectedObject(nullptr);        // 선택 해제 
            }

            ptPrevMouse = ptMouse;
        }
    }

    // 다음 프레임을 위해 현재 상태 저장
    m_bPrevMouseLeft = bCurMouseLeft;
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