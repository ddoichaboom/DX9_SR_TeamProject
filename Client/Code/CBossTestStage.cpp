#include "pch.h"
#include "CBossTestStage.h"
#include "CProtoMgr.h"
#include "CTestCharacter.h"
#include "CWhiteMan.h"

#include "CPlayer.h"
#include "CFirstCamera.h"

#include "CPoolMgr.h"
#include "CBeamMon.h"
#include "CBeam.h"
#include "CFlyMon.h"

#include "CBoss.h"
#include "CBossBullet.h"
#include "CRocket.h"

//Player
#include "CLeftPart.h"
#include "CRightPart.h"
#include "CMiddlePart.h"
#include "CPistol.h"


CBossTestStage::CBossTestStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: CStage(pGraphicDev), m_pEnvironment_Layer(nullptr), m_pGameLogic_Layer(nullptr)
{
}

CBossTestStage::~CBossTestStage()
{
}

HRESULT CBossTestStage::Ready_Scene()
{
	if (FAILED(Ready_Prototype()))
		return E_FAIL;

	if (FAILED(Ready_ObjectPool()))
		return E_FAIL;

	if (FAILED(Ready_Environment_Layer(L"Environment_Layer")))
		return E_FAIL;

	if (FAILED(Ready_GameLogic_Layer(L"GameLogic_Layer")))
		return E_FAIL;

	return S_OK;
}

_int CBossTestStage::Update_Scene(const _float& fTimeDelta)
{
	int iExit = CStage::Update_Scene(fTimeDelta);
	return iExit;
}

void CBossTestStage::LateUpdate_Scene(const _float& fTimeDelta)
{
	CStage::LateUpdate_Scene(fTimeDelta);
}

void CBossTestStage::Render_Scene()
{
}


HRESULT CBossTestStage::Ready_ObjectPool()
{
	
	if (!CPoolMgr::GetInstance()->HasPool<CBossBullet>())
	{
		if (FAILED(CPoolMgr::GetInstance()->CreatePool<CBossBullet>(m_pGraphicDev)))
		{
			MSG_BOX("Bullet Pool Create Failed");
			return E_FAIL;
		}
	}

	if (!CPoolMgr::GetInstance()->HasPool<CRocket>())
	{
		if (FAILED(CPoolMgr::GetInstance()->CreatePool<CRocket>(m_pGraphicDev)))
		{
			MSG_BOX("Rocket Pool Create Failed");
			return E_FAIL;
		}
	}

	if (!CPoolMgr::GetInstance()->HasPool<CFlyMon>())
	{
		if (FAILED(CPoolMgr::GetInstance()->CreatePool<CFlyMon>(m_pGraphicDev)))
		{
			MSG_BOX("FlyMon Pool Create Failed");
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CBossTestStage::Ready_Environment_Layer(const _tchar* pLayerTag)
{
	m_pEnvironment_Layer = CLayer::Create();
	if (nullptr == m_pEnvironment_Layer)
		return E_FAIL;

	_vec3 vEye = { 0,0,0.f };
	_vec3 vAt = { 0,0.f,1.f };
	_vec3 vUp = { 0,1,0 };

	//Camera
	CGameObject* pGameObject = nullptr;
	pGameObject = CFirstCamera::Create(m_pGraphicDev, &vEye, &vAt, &vUp);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(m_pEnvironment_Layer->Add_GameObject(pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ pLayerTag , m_pEnvironment_Layer });



    return S_OK;
}

HRESULT CBossTestStage::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

#pragma region Player
	CPlayer* pPlayer = nullptr;
	_vec3 pPlayerSpawnPos = { 0,0,0 };

	pGameObject = pPlayer = CPlayer::Create(m_pGraphicDev, pPlayerSpawnPos);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(pGameObject)))
		return E_FAIL;

	pGameObject = CMiddlePart::Create(m_pGraphicDev);
	pPlayer->Set_MiddlePart(dynamic_cast<CMiddlePart*>(pGameObject));
	if (FAILED(pLayer->Add_GameObject(pGameObject)))
		return E_FAIL;

	pGameObject = CRightPart::Create(m_pGraphicDev);
	pPlayer->Set_RightPart(dynamic_cast<CRightPart*>(pGameObject));
	if (FAILED(pLayer->Add_GameObject(pGameObject)))
		return E_FAIL;

	pGameObject = CLeftPart::Create(m_pGraphicDev);
	pPlayer->Set_LeftPart(dynamic_cast<CLeftPart*>(pGameObject));
	if (FAILED(pLayer->Add_GameObject(pGameObject)))
		return E_FAIL;

	pGameObject = CPistol::Create(m_pGraphicDev);
	pPlayer->Add_Weapon((byte)1, dynamic_cast<CPistol*>(pGameObject));
	if (FAILED(pLayer->Add_GameObject(pGameObject)))
		return E_FAIL;

#pragma endregion

	pGameObject = CPoolMgr::GetInstance()->Get_Object<CFlyMon>();
	pGameObject->SetPos({ 70.f, 10.f, 90.f });

	if (nullptr == pGameObject) return E_FAIL;
	if (FAILED(pLayer->Add_GameObject(pGameObject))) return E_FAIL;

	pGameObject = CBoss::Create(m_pGraphicDev);

	if (nullptr == pGameObject) return E_FAIL;
	if (FAILED(pLayer->Add_GameObject(pGameObject))) return E_FAIL;

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;

}

HRESULT CBossTestStage::Ready_Prototype()
{
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RcTexUp", CRcTexUp::Create(m_pGraphicDev))))
		return E_FAIL;

	CTexture* pCom_Texture = nullptr;
	////FlyMon Texture
	//pCom_Texture = CTexture::Create(m_pGraphicDev, CFlyMon::GetTextureSources());
	//if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_FlyMonTexture", pCom_Texture)))
	//	return E_FAIL;

	//if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_FlyMonAnimation", CAnimation::Create(m_pGraphicDev, pCom_Texture, CFlyMon::GetAnimSources()))))
	//	return E_FAIL;
	//
	////Beam Texture
	//pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CBeam::GetTextureSources());
	//if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BeamTexture", pCom_Texture)))
	//	return E_FAIL;

	//BOSS Texture
	pCom_Texture = CTexture::Create(m_pGraphicDev, CBoss::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BossTexture", pCom_Texture)))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BossAnimation", CAnimation::Create(m_pGraphicDev, pCom_Texture, CBoss::GetAnimSources()))))
		return E_FAIL;

	//Boss Bullet Texture
	pCom_Texture = CTexture::Create(m_pGraphicDev, CBossBullet::GetTextureSource());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BulletTexture", pCom_Texture)))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BossBulletAnimation", CAnimation::Create(m_pGraphicDev, pCom_Texture, CBossBullet::GetAnimSource()))))
		return E_FAIL;

	//Boss Rocket Texture
	pCom_Texture = CTexture::Create(m_pGraphicDev, CRocket::GetTextureSource());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RocketTexture", pCom_Texture)))
		return E_FAIL;

	return S_OK;
}

CBossTestStage* CBossTestStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBossTestStage* pBossTestStage = new CBossTestStage(pGraphicDev);

	if (FAILED(pBossTestStage->Ready_Scene()))
	{
		Safe_Release(pBossTestStage);
		MSG_BOX("Boss Test Stage Create Failed");
		return nullptr;
	}

	return pBossTestStage;
}

void CBossTestStage::Free()
{
	CScene::Free();
}
