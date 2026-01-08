#include "pch.h"
#include "CTestStage.h"
#include "CProtoMgr.h"
#include "CTestCharacter.h"
#include "CWhiteMan.h"

#include "CPlayer.h"
#include "CFirstCamera.h"
#include "CBullet.h"

#include "CTerrain.h"
#include "CTerrainTex.h"
#include "CPoolMgr.h"
#include "CBeamMon.h"
#include "CBeam.h"
#include "CFlyMon.h"

CTestStage::CTestStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: CStage(pGraphicDev), m_pEnvironment_Layer(nullptr), m_pGameLogic_Layer(nullptr)
{
}

CTestStage::~CTestStage()
{
}

HRESULT CTestStage::Ready_Scene()
{
	if (FAILED(Ready_Prototype()))
		return E_FAIL;

	if (FAILED(Ready_Environment_Layer(L"Environment_Layer")))
		return E_FAIL;

	if (FAILED(Ready_GameLogic_Layer(L"GameLogic_Layer")))
		return E_FAIL;

	return S_OK;
}

_int CTestStage::Update_Scene(const _float& fTimeDelta)
{
	int iExit = CStage::Update_Scene(fTimeDelta);
	bool bEmpty = m_pGameLogic_Layer->IsEmptyByOBJID(OBJ_MONSTER);
	if (bEmpty)
	{
		CWhiteMan* man = CPoolMgr::GetInstance()->Get_Object<CWhiteMan>();
		if (man)
		{
			man->SetPos({ 0, 0, 110.f });
			m_pGameLogic_Layer->Add_GameObject(man); 
		}
	}
	return iExit;
}

void CTestStage::LateUpdate_Scene(const _float& fTimeDelta)
{
	CStage::LateUpdate_Scene(fTimeDelta);
}

void CTestStage::Render_Scene()
{
}

HRESULT CTestStage::Ready_Environment_Layer(const _tchar* pLayerTag)
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

	//Terrain Pool 생성 
	if (CPoolMgr::GetInstance()->HasPool<CTerrain>() == false)
	{
		if (FAILED(CPoolMgr::GetInstance()->CreatePool<CTerrain>(m_pGraphicDev)))
		{
			MSG_BOX("Terrain Pool Create Failed");
			return E_FAIL;
		}
	}
	pGameObject = CPoolMgr::GetInstance()->Get_Object<CTerrain>();
	
	if (nullptr == pGameObject) return E_FAIL;
	if (FAILED(m_pEnvironment_Layer->Add_GameObject(pGameObject))) return E_FAIL;

	m_mapLayer.insert({ pLayerTag , m_pEnvironment_Layer });

    return S_OK;
}

HRESULT CTestStage::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
	m_pGameLogic_Layer = CLayer::Create();
	if (nullptr == m_pGameLogic_Layer)
		return E_FAIL;

	//Player
	CGameObject* pGameObject = nullptr;

	pGameObject = CPlayer::Create(m_pGraphicDev);
	pGameObject->SetPos({ 0.f, 6.f, 0.f });
	if (FAILED(m_pGameLogic_Layer->Add_GameObject(pGameObject)))
		return E_FAIL;

	//Bullet Pool 
	if (CPoolMgr::GetInstance()->HasPool<CBullet>() == false)
	{
		if (FAILED(CPoolMgr::GetInstance()->CreatePool<CBullet>(m_pGraphicDev)))
		{
			MSG_BOX("Bullet Pool Create Failed");
			return E_FAIL;
		}
	}

	//WhiteMan
	if (CPoolMgr::GetInstance()->HasPool<CWhiteMan>() == false)
	{
		if (FAILED(CPoolMgr::GetInstance()->CreatePool<CWhiteMan>(m_pGraphicDev)))
		{
			MSG_BOX("WhiteMan Pool Create Failed");
			return E_FAIL;
		}
	}
	pGameObject = CPoolMgr::GetInstance()->Get_Object<CWhiteMan>();
	pGameObject->SetPos({ 0.f, 0.f, 110.f });
	if (nullptr == pGameObject) return E_FAIL;
	if (FAILED(m_pGameLogic_Layer->Add_GameObject(pGameObject))) return E_FAIL;

	//Beam Mon
	if (CPoolMgr::GetInstance()->HasPool<CBeamMon>() == false)
	{
		if (FAILED(CPoolMgr::GetInstance()->CreatePool<CBeamMon>(m_pGraphicDev)))
		{
			MSG_BOX("BeamMon Pool Create Failed");
			return E_FAIL;
		}
	}
	pGameObject = CPoolMgr::GetInstance()->Get_Object<CBeamMon>();
	pGameObject->SetPos({ 0, 18.f, 60.f });

	if (nullptr == pGameObject) return E_FAIL;
	if (FAILED(m_pGameLogic_Layer->Add_GameObject(pGameObject))) return E_FAIL;


	//Fly Mon
	if (CPoolMgr::GetInstance()->HasPool<CFlyMon>() == false)
	{
		if (FAILED(CPoolMgr::GetInstance()->CreatePool<CFlyMon>(m_pGraphicDev)))
		{
			MSG_BOX("FlyMon Pool Create Failed");
			return E_FAIL;
		}
	}
	pGameObject = CPoolMgr::GetInstance()->Get_Object<CFlyMon>();
	pGameObject->SetPos({ 70.f, 10.f, 90.f });

	if (nullptr == pGameObject) return E_FAIL;
	if (FAILED(m_pGameLogic_Layer->Add_GameObject(pGameObject))) return E_FAIL;


	m_mapLayer.insert({ pLayerTag , m_pGameLogic_Layer });

	return S_OK;

}

HRESULT CTestStage::Ready_Prototype()
{
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RcTexUp", Engine::CRcTexUp::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RcTex", Engine::CRcTex::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Transform", Engine::CTransform::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Collision", Engine::CCollision::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_StateComponent", Engine::CStateComponent::Create(m_pGraphicDev))))
		return E_FAIL;

#pragma region SAMPLE
	// SAMPLE
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TerrainTex", Engine::CTerrainTex::Create(m_pGraphicDev, VTXCNTX, VTXCNTZ, VTXITV))))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_CubeTex", Engine::CCubeTex::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TerrainTexture", Engine::CTerrainTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Terrain/Terrain0.dds", 1))))
		return E_FAIL;

	//if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TerrainTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/Terrain/Grass_%d.tga", 2))))
	//	return E_FAIL;
	//
	//if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_SkyTexture", Engine::CTexture::Create(m_pGraphicDev, TEX_CUBE, L"../Bin/Resource/Texture/SkyBox/burger%d.dds", 4))))
	//	return E_FAIL;
#pragma endregion
	vector<TextureSource> vTextureSource =
	{
		{ 0, L"../Bin/Resource/Texture/Player/pika.dds" }
	};

	CTexture* pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, vTextureSource);
		if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TestTexture", pCom_Texture)))
			return E_FAIL;

	//White Man Texture 
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CWhiteMan::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_WhiteManTexture", pCom_Texture)))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_WhiteManAnimation", Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CWhiteMan::GetAnimSources()))))
		return E_FAIL;

	//Beam Mon Texture 
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CBeamMon::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BeamMonTexture", pCom_Texture)))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BeamMonAnimation", Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CBeamMon::GetAnimSources()))))
		return E_FAIL;

	//Beam Texture
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CBeam::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BeamTexture", pCom_Texture)))
		return E_FAIL;

	//FlyMon Texture
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CFlyMon::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_FlyMonTexture", pCom_Texture)))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_FlyMonAnimation", Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CFlyMon::GetAnimSources()))))
		return E_FAIL;

	//Bullet Texture
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CBullet::GetTextureSource());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_BulletTexture", pCom_Texture)))
		return E_FAIL;



#pragma region Left

	vector<TextureSource> vLeftTextureSource =
	{
		{ 0, L"../Bin/Resource/Texture/Player/Left_Hand_Idle.dds" },
		{ 1, L"../Bin/Resource/Texture/Player/Left_Hand_Reload_P.dds" },
		{ 2, L"../Bin/Resource/Texture/Player/Left_Hand_Reload_S.dds" }

	};

	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, vLeftTextureSource);
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_LeftTexture", pCom_Texture)))
		return E_FAIL;

	//상태값, 마지막 행 번호,  마지막 열 번호, 프레임이 끝나는 열 번호, 루프 유무, 플레이 속도 = 0.12f	
	vector<AnimationSource> vLeftAnimSource =
	{
		{ 0,1,3,3, true, 0.11f},
		{ 1,0,3,3, false, 0.11f},
		{ 2,0,3,3, false, 0.11f},
	};

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_LeftAnimation", 
		Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, vLeftAnimSource))))
		return E_FAIL;
#pragma endregion


#pragma region PLAYER_RIGHT_HAND

	vector<TextureSource> vRightTextureSource =
	{
		{ 0, L"../Bin/Resource/Texture/Player/Right_Hand_Idle_P.dds" },
		{ 1, L"../Bin/Resource/Texture/Player/Right_Hand_Shot_P.dds" },
		{ 2, L"../Bin/Resource/Texture/Player/Right_Hand_Reload_P.dds" }

	};

	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, vRightTextureSource);
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RightTexture", pCom_Texture)))
		return E_FAIL;

	//상태값, 마지막 행 번호,  마지막 열 번호, 프레임이 끝나는 열 번호, 루프 유무, 플레이 속도 = 0.12f	
	vector<AnimationSource> vRightAnimSource =
	{
		{ 0,0,3,3, true, 0.11f},
		{ 1,0,5,5, false, 0.02f},
		{ 2,1,6,6, false, 0.02f},
	};

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RightAnimation", 
		Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, vRightAnimSource))))
		return E_FAIL;

#pragma endregion


#pragma region Middle_Part

	vector<TextureSource> vMiddleTextureSource =
	{
		{ 0, L"../Bin/Resource/Texture/Player/Middle_Kick.dds" },
		{ 1, L"../Bin/Resource/Texture/Player/Middle_Soda.dds" },

	};

	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, vMiddleTextureSource);
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_MiddleTexture", pCom_Texture)))
		return E_FAIL;

	//상태값, 마지막 행 번호,  마지막 열 번호, 프레임이 끝나는 열 번호, 루프 유무, 플레이 속도 = 0.12f	
	vector<AnimationSource> vMiddleAnimSource =
	{
		{ 0,0,3,3, false, 0.08f},
		{ 1,0,6,6, false, 0.05f}
	};

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_MiddleAnimation", 
		Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, vMiddleAnimSource))))
		return E_FAIL;


#pragma endregion

	return S_OK;
}

CTestStage* CTestStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTestStage* pTestStage = new CTestStage(pGraphicDev);

	if (FAILED(pTestStage->Ready_Scene()))
	{
		Safe_Release(pTestStage);
		MSG_BOX("Test Stage Create Failed");
		return nullptr;
	}

	return pTestStage;
}

void CTestStage::Free()
{
	CScene::Free();
}
