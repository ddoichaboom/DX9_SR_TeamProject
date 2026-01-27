#include "pch.h"
#include "CRoadStage.h"
#include "CProtoMgr.h"
#include "CMapLoader.h"
#include "CPoolMgr.h"
#include "CManagement.h"
#include "CDInputMgr.h"
#include "CSoundMgr.h"
#include "CUIManager.h"

// 게임 오브젝트 
#include "CFirstCamera.h"
#include "CCursor.h"

CRoadStage::CRoadStage(LPDIRECT3DDEVICE9 pGraphicDev) : CStage(pGraphicDev)
{
}

CRoadStage::~CRoadStage()
{
}

HRESULT CRoadStage::Ready_Scene()
{
	//다른 맵과 연결하면서 로딩스레드 추가하기 전까지는 필요한 함수 직접 호출하기
	//Ready_Prototype .. 등 
	CUIManager::GetInstance()->Clear_UIGroup();

	if (FAILED(Ready_Environment_Layer(L"Environment_Layer"))) return E_FAIL;
	if (FAILED(Ready_GameLogic_Layer(L"GameLogic_Layer"))) return E_FAIL;

	return S_OK;
}

_int CRoadStage::Update_Scene(const _float& fTimeDelta)
{
	int iExit = CStage::Update_Scene(fTimeDelta);
	return iExit;
}

void CRoadStage::LateUpdate_Scene(const _float& fTimeDelta)
{
	CStage::LateUpdate_Scene(fTimeDelta);
}

void CRoadStage::Render_Scene()
{
}

HRESULT CRoadStage::Ready_Environment_Layer(const _tchar* pLayerTag)
{
	return S_OK;
}

HRESULT CRoadStage::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)	return E_FAIL;


	m_mapLayer.insert({ pLayerTag, pLayer });
	m_pGameLogic_Layer = pLayer;

	return S_OK;
}

HRESULT CRoadStage::Ready_Prototype()
{
	return S_OK;
}

HRESULT CRoadStage::Remove_PrevObjectPool()
{

	return S_OK;
}

HRESULT CRoadStage::Ready_ObjectPool_Character()
{
	return S_OK;
}

HRESULT CRoadStage::Ready_ObjectPool_Terrain()
{
	return S_OK;
}

HRESULT CRoadStage::Ready_ObjectPool_UI()
{
	return S_OK;
}

HRESULT CRoadStage::Ready_ObjectPool_Effect()
{
	return S_OK;
}

HRESULT CRoadStage::Ready_CharacterTextureProto()
{
	return S_OK;
}

HRESULT CRoadStage::Ready_TerrainTextureProto()
{
	return S_OK;
}

HRESULT CRoadStage::Ready_UITextureProto()
{
	return S_OK;
}

HRESULT CRoadStage::Ready_EffectTextureProto()
{
	return S_OK;
}

void CRoadStage::Check_Collision()
{
}

void CRoadStage::OnEvent(EVENT_TYPE _type, EventData* _pData)
{
	if (_type == EVENT_ENDING)
	{
		//m_bStageEnd = true;
	}
}

CRoadStage* CRoadStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRoadStage* pRoadStage = new CRoadStage(pGraphicDev);

	if (FAILED(pRoadStage->Ready_Scene()))
	{
		Safe_Release(pRoadStage);
		MSG_BOX("Road Stage Create Failed");
		return nullptr;
	}

	return pRoadStage;
}

void CRoadStage::Free()
{
	CScene::Free();
}