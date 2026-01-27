#include "pch.h"
#include "CSniperStage.h"
#include "CProtoMgr.h"
#include "CMapLoader.h"
#include "CPoolMgr.h"
#include "CManagement.h"
#include "CDInputMgr.h"

#include "CFirstCamera.h"
#include "CCursor.h"

CSniperStage::CSniperStage(LPDIRECT3DDEVICE9 pGraphicDev) : CStage(pGraphicDev)
{
}

CSniperStage::~CSniperStage()
{
}

HRESULT CSniperStage::Ready_Scene()
{
	if (FAILED(Ready_Environment_Layer(L"Environment_Layer"))) return E_FAIL;
	if (FAILED(Ready_GameLogic_Layer(L"GameLogic_Layer"))) return E_FAIL;
	return S_OK;
}

_int CSniperStage::Update_Scene(const _float& fTimeDelta)
{
	int iExit = CStage::Update_Scene(fTimeDelta);
	return iExit;
}

void CSniperStage::LateUpdate_Scene(const _float& fTimeDelta)
{
	CStage::LateUpdate_Scene(fTimeDelta);
}

void CSniperStage::Render_Scene()
{
}

HRESULT CSniperStage::Ready_Environment_Layer(const _tchar* pLayerTag)
{
	return S_OK;
}

HRESULT CSniperStage::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)	return E_FAIL;


	m_mapLayer.insert({ pLayerTag, pLayer });
	m_pGameLogic_Layer = pLayer;
	return S_OK;

}

HRESULT CSniperStage::Ready_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CSniperStage::Remove_PrevObjectPool()
{
	return E_NOTIMPL;
}

HRESULT CSniperStage::Ready_ObjectPool_Character()
{
	return E_NOTIMPL;
}

HRESULT CSniperStage::Ready_ObjectPool_Terrain()
{
	return E_NOTIMPL;
}

HRESULT CSniperStage::Ready_ObjectPool_UI()
{
	return E_NOTIMPL;
}

HRESULT CSniperStage::Ready_ObjectPool_Effect()
{
	return E_NOTIMPL;
}

HRESULT CSniperStage::Ready_CharacterTextureProto()
{
	return E_NOTIMPL;
}

HRESULT CSniperStage::Ready_TerrainTextureProto()
{
	return E_NOTIMPL;
}

HRESULT CSniperStage::Ready_UITextureProto()
{
	return E_NOTIMPL;
}

HRESULT CSniperStage::Ready_EffectTextureProto()
{
	return E_NOTIMPL;
}

void CSniperStage::Check_Collision()
{
}

CSniperStage* CSniperStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSniperStage* pSniperStage = new CSniperStage(pGraphicDev);

	if (FAILED(pSniperStage->Ready_Scene()))
	{
		Safe_Release(pSniperStage);
		MSG_BOX("Sniper Stage Create Failed");
		return nullptr;
	}

	return pSniperStage;
}

void CSniperStage::Free()
{
	CScene::Free();
}