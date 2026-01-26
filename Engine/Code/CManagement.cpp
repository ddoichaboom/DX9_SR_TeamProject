#include "CManagement.h"
#include "CRenderer.h"
#include "CProtoMgr.h"
#include "CFontMgr.h"

IMPLEMENT_SINGLETON(CManagement)

CManagement::CManagement() : m_pScene(nullptr), m_eCurrSceneType(SCENE_NONE), m_iFloorNumber(1)
{
}

CManagement::~CManagement()
{
    Free();
}

CComponent* CManagement::Get_Component(COMPONENTID eID, const _tchar* pLayerTag, OBJ_ID _objID, const _tchar* pComponentTag)
{
    if (nullptr == m_pScene)
        return nullptr;

    return m_pScene->Get_Component(eID, pLayerTag, _objID, pComponentTag);
}

CLayer* CManagement::Get_Layer(const _tchar* pLayerTag)
{
    if (nullptr == m_pScene) return nullptr;
    return m_pScene->Get_Layer(pLayerTag);
}

HRESULT CManagement::Set_Scene(CScene* pScene)
{
    if (nullptr == pScene)
        return  E_FAIL;

    Safe_Release(m_pScene);

    m_pScene = pScene;

    return S_OK;
}

_int CManagement::Update_Scene(const _float& fTimeDelta)
{
    if (nullptr == m_pScene)
        return -1;

    Update_CountTime(fTimeDelta);

    return m_pScene->Update_Scene(fTimeDelta);
}

void CManagement::LateUpdate_Scene(const _float& fTimeDelta)
{
    m_pScene->LateUpdate_Scene(fTimeDelta);
}

void CManagement::Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CRenderer::GetInstance()->Render_GameObject(pGraphicDev);
    CFontMgr::GetInstance()->Render_FontGroup();
    // debug용 렌더
    m_pScene->Render_Scene();
}

void CManagement::Free()
{
    Safe_Release(m_pScene);
}

wstring CManagement::Convert_PlayTime()
{    
    // MillSecond 단위 변환
    _int iTotalTime = static_cast<int>(m_fTime * 1000.f);

    _int iMinTime = (iTotalTime / 60000);
    _int iSecTime = (iTotalTime % 60000) / 1000;
    _int iMillSecTime = (iTotalTime % 1000) / 10;

    _tchar buffer[64];

    swprintf_s(buffer, L"%02d:%02d:%02d sec", iMinTime, iSecTime, iMillSecTime);

    return wstring(buffer);
}

wstring CManagement::Convert_StageInfo()
{
    wstring wText = L"FLOOR 1";

    if(m_iFloorNumber > 1)
        wText = L"FLOOR 2";
   
    return wText;
}

void CManagement::Update_CountTime(const _float& fTimeDelta)
{
    if (m_bCountTime)
    {
        m_fTime += fTimeDelta;
    }
}
