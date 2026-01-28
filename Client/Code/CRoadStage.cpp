#include "pch.h"
#include "CRoadStage.h"
#include "CProtoMgr.h"
#include "CMapLoader.h"
#include "CPoolMgr.h"
#include "CManagement.h"
#include "CDInputMgr.h"
#include "CSoundMgr.h"
#include "CUIManager.h"

// 필수 게임 오브젝트 
#include "CRoadCamera.h"
#include "CCursor.h"
#include "CRoadPlayer.h"

//스카이 박스
#include "CSkyBox.h"

//미니건 관련
#include "CMinigun.h"
#include "CPannel.h"
#include "CChain.h"

// 게임 로직 오브젝트
#include "CFlyMon.h"
#include "CBullet.h"

// 이펙트 로직 오브젝트
#include "CFlare.h"
#include "CExplosion.h"
#include "CBodyEmit.h"
#include "CHitUI.h"

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

	if (FAILED(Ready_CharacterTextureProto())) return E_FAIL;
	if (FAILED(Ready_ObjectPool_Character())) return E_FAIL;


	if (FAILED(Ready_TerrainTextureProto())) return E_FAIL;
	if (FAILED(Ready_ObjectPool_Terrain())) return E_FAIL;

	if (FAILED(Ready_UITextureProto())) return E_FAIL;
	if (FAILED(Ready_ObjectPool_UI())) return E_FAIL;

	if (FAILED(Ready_EffectTextureProto())) return E_FAIL;
	if (FAILED(Ready_ObjectPool_Effect())) return E_FAIL;

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
	CLayer* pLayer = CLayer::Create();
	CGameObject* pGameObject = nullptr;
	if (nullptr == pLayer)
		return E_FAIL;


	m_mapLayer.insert({ pLayerTag, pLayer });
	m_pEnvironment_Layer = pLayer;
	return S_OK;
}

HRESULT CRoadStage::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)	return E_FAIL;

	CGameObject* pGameObject = nullptr;

	// 카메라
	_vec3 vPlayerPos = { 0.f, 0.f, 0.f };
	_vec3 vEye = vPlayerPos;
	_vec3 vAt = { vPlayerPos.x, vPlayerPos.y, vPlayerPos.z +10.f };
	_vec3 vUp = { 0.f, 1.f, 0.f };
	pGameObject = CRoadCamera::Create(m_pGraphicDev, &vEye, &vAt, &vUp);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(pGameObject)))
		return E_FAIL;

	// 스카이 박스
	pGameObject = CSkyBox::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(pGameObject)))
		return E_FAIL;

	// 커서
	pGameObject = CCursor::Create(m_pGraphicDev);
	if (pGameObject == nullptr)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(pGameObject)))
		return E_FAIL;

	// 플레이어
	pGameObject = CRoadPlayer::Create(m_pGraphicDev);
	if (pGameObject == nullptr)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(pGameObject)))
		return E_FAIL;


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
	if (!Engine::CPoolMgr::GetInstance()->HasPool<CBullet>())
	{
		if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CBullet>(m_pGraphicDev)))
		{
			MSG_BOX("Bullet Pool Create Failed");
			return E_FAIL;
		}
	}


	if (!Engine::CPoolMgr::GetInstance()->HasPool<CFlyMon>())
	{
		if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CFlyMon>(m_pGraphicDev)))
		{
			MSG_BOX("FlyMon Pool Create Failed");
			return E_FAIL;
		}
	}


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
	if (!Engine::CPoolMgr::GetInstance()->HasPool<CFlare>())
	{
		if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CFlare>(m_pGraphicDev)))
		{
			MSG_BOX("Effect Flare Pool Create Failed");
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

	if (!Engine::CPoolMgr::GetInstance()->HasPool<CBodyEmit>())
	{
		if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CBodyEmit>(m_pGraphicDev)))
		{
			MSG_BOX("Effect BodyEmit Pool Create Failed");
			return E_FAIL;
		}
	}

	if (!Engine::CPoolMgr::GetInstance()->HasPool<CHitUI>())
	{
		if (FAILED(Engine::CPoolMgr::GetInstance()->CreatePool<CHitUI>(m_pGraphicDev)))
		{
			MSG_BOX("Effect HitUI Pool Create Failed");
			return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CRoadStage::Ready_CharacterTextureProto()
{
	CTexture* pCom_Texture = nullptr;

	//Minigun
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CMinigun::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_MinigunTexture", pCom_Texture)))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_MinigunAnimation", Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CMinigun::GetAnimSources()))))
		return E_FAIL;

	//Minigun Pannel
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CPannel::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_PannelTexture", pCom_Texture)))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_PannelAnimation", Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CPannel::GetAnimSources()))))
		return E_FAIL;

	//Minigun Chain
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CChain::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_ChainTexture", pCom_Texture)))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_ChainAnimation", Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CChain::GetAnimSources()))))
		return E_FAIL;

	// Monster
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CFlyMon::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_FlyMonTexture", pCom_Texture)))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_FlyMonAnimation", Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CFlyMon::GetAnimSources()))))
		return E_FAIL;

	//Bullet Texture
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CBullet::GetTextureSource());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BulletTexture", pCom_Texture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRoadStage::Ready_TerrainTextureProto()
{
	// ㅣ필수 삭제 ㅣ
	CCubeTexture* pCom_Cube_Texture = nullptr;

	// Obstacle(VendingMachine) Proto 
	pCom_Cube_Texture = Engine::CCubeTexture::Create(m_pGraphicDev, CSkyBox::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_SkyTexture", pCom_Cube_Texture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRoadStage::Ready_UITextureProto()
{
	// ㅣ필수ㅣ TEST 할때 최초 세팅을 위해 생성 나중에 지워야함 
	CUIManager::GetInstance()->Ready_GameObject(m_pGraphicDev);
	return S_OK;
}

HRESULT CRoadStage::Ready_EffectTextureProto()
{
	// ㅣ필수ㅣTEST 할때 최초 세팅을 위해 생성 나중에 지워야함 
	CTexture* pCom_Texture = nullptr;

	//Flare
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CFlare::GetTextureSource());
	if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_Flare_Texture", pCom_Texture)))
	{
		MSG_BOX("Proto Flare Ready Failed");
		return E_FAIL;
	}

	//Explosion
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CExplosion::GetTextureSource());
	if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_EXP_Texture", pCom_Texture)))
	{
		MSG_BOX("Proto Explosion Ready Failed");
		return E_FAIL;
	}

	//BodyEmit
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CBodyEmit::GetTextureSources());
	if (FAILED(Engine::CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Effect_BodyEmit_Texture", pCom_Texture)))
	{
		MSG_BOX("Proto BodyEmit Ready Failed");
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