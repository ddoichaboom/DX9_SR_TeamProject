#include "pch.h"
#include "CSniperStage.h"
#include "CProtoMgr.h"
#include "CMapLoader.h"
#include "CPoolMgr.h"
#include "CManagement.h"
#include "CDInputMgr.h"
#include "CCursor.h"
#include "CUIManager.h"

//Character
#include "CWhiteMan.h"
#include "CSniperPlayer.h"
#include "CSniperWhiteMan.h"
#include "CBullet.h"
#include "CLeftPart.h"
#include "CSRightHand.h"
#include "CBeam.h"

//UI
#include "CSniperUI.h"
#include "CPhoneBG.h"

//Effect
#include "CBlood.h"
#include "CExplosion.h"
#include "CHitUI.h"


CSniperStage::CSniperStage(LPDIRECT3DDEVICE9 pGraphicDev) 
	: CStage(pGraphicDev), m_pPlayer(nullptr)
{
}

CSniperStage::~CSniperStage()
{
}

HRESULT CSniperStage::Ready_Scene()
{
	if (FAILED(Ready_EffectTextureProto())) return E_FAIL;
	if (FAILED(Ready_CharacterTextureProto())) return E_FAIL;
	if (FAILED(Ready_UITextureProto())) return E_FAIL;
	if (FAILED(Ready_ObjectPool_Effect())) return E_FAIL;
	if (FAILED(Ready_ObjectPool_Character())) return E_FAIL;


	if (FAILED(Ready_Environment_Layer(L"Environment_Layer"))) return E_FAIL;
	if (FAILED(Ready_GameLogic_Layer(L"GameLogic_Layer"))) return E_FAIL;

	return S_OK;
}

_int CSniperStage::Update_Scene(const _float& fTimeDelta)
{
	int iExit = CStage::Update_Scene(fTimeDelta);
	CUIManager::GetInstance()->Update_GameObject(fTimeDelta);
	return iExit;
}

void CSniperStage::LateUpdate_Scene(const _float& fTimeDelta)
{
	CStage::LateUpdate_Scene(fTimeDelta);
	CUIManager::GetInstance()->LateUpdate_GameObject(fTimeDelta);
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

	m_pPlayer = CSniperPlayer::Create(m_pGraphicDev, _vec3(0, 0, 0));
	
	if (m_pPlayer == nullptr) return E_FAIL;
	if(FAILED(pLayer->Add_GameObject(m_pPlayer))) return E_FAIL;

	/*CGameObject * whiteMan = CWhiteMan::Create(m_pGraphicDev, _vec3(0, 0, 400));
	if (whiteMan == nullptr) return E_FAIL;
	if (FAILED(pLayer->Add_GameObject(whiteMan))) return E_FAIL;*/

	CGameObject* SniperMan = CSniperWhiteMan::Create(m_pGraphicDev, _vec3(0, 0, 400));
	if (SniperMan == nullptr) return E_FAIL;
	if (FAILED(pLayer->Add_GameObject(SniperMan))) return E_FAIL;

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

	/*if (!Engine::CPoolMgr::GetInstance()->HasPool<CBullet>())
	{
		if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CBullet>(m_pGraphicDev)))
		{
			MSG_BOX("Bullet Pool Create Failed");
			return E_FAIL;
		}
	}*/

	//if (!Engine::CPoolMgr::GetInstance()->HasPool<CWhiteMan>())
	//{
	//	if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CWhiteMan>(m_pGraphicDev)))
	//	{
	//		MSG_BOX("WhiteMan Pool Create Failed");
	//		return E_FAIL;
	//	}
	//}
	return S_OK;
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
	if (!Engine::CPoolMgr::GetInstance()->HasPool<CBlood>())
	{
		if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CBlood>(m_pGraphicDev)))
		{
			MSG_BOX("Effect Blood Pool Create Failed");
			return E_FAIL;
		}
	}

	if (!Engine::CPoolMgr::GetInstance()->HasPool<CExplosion>())
	{
		if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CExplosion>(m_pGraphicDev)))
		{
			MSG_BOX("Effect Explosion Pool Create Failed");
			return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CSniperStage::Ready_CharacterTextureProto()
{
	CTexture* pCom_Texture = nullptr;

	//Beam Texture
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CBeam::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BeamTexture", pCom_Texture)))
		return E_FAIL;

	// WhiteMan
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CWhiteMan::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_WhiteManTexture", pCom_Texture)))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_WhiteManAnimation", Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CWhiteMan::GetAnimSources()))))
		return E_FAIL;

	//Sniper WhiteMan
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CSniperWhiteMan::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_SniperWhiteManTexture", pCom_Texture)))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_SniperManAnimation", Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CSniperWhiteMan::GetAnimSources()))))
		return E_FAIL;

	//Bullet Texture
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CBullet::GetTextureSource());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BulletTexture", pCom_Texture)))
		return E_FAIL;

	// RightHand
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CSRightHand::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_SniperRightTexture", pCom_Texture)))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_SniperRightAnimation", Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CSRightHand::GetAnimSources()))))
		return E_FAIL;

	//Left Hand 는 Main에 위치

	return S_OK;
}

HRESULT CSniperStage::Ready_TerrainTextureProto()
{
	return E_NOTIMPL;
}

HRESULT CSniperStage::Ready_UITextureProto()
{
	CTexture* pCom_Texture = nullptr;
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CSniperUI::GetTextureSource());
	if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_SniperUITexture", pCom_Texture)))
	{
		MSG_BOX("Sniper UI Ready Failed");
		return E_FAIL;
	}

	CUIManager::GetInstance()->Ready_GameObject(m_pGraphicDev);
	return S_OK;
}

HRESULT CSniperStage::Ready_EffectTextureProto()
{
	CTexture* pCom_Texture = nullptr;
	//Blood Texture
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CBlood::GetTextureSources());
	if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_Blood_Texture", pCom_Texture)))
	{
		MSG_BOX("Proto Blood Ready Failed");
		return E_FAIL;
	}
	//Explosion
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CExplosion::GetTextureSource());
	if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_EXP_Texture", pCom_Texture)))
	{
		MSG_BOX("Proto Explosion Ready Failed");
		return E_FAIL;
	}


	//HitUI
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CHitUI::GetTextureSource());
	if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_HitUI_Texture", pCom_Texture)))
	{
		MSG_BOX("Proto HitUI Ready Failed");
		return E_FAIL;
	}
	return S_OK;
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