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

CTestStage::CTestStage(LPDIRECT3DDEVICE9 pGraphicDev) : CStage(pGraphicDev)
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
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	_matrix View, Proj;
	_vec3 vEye = { 0,0,0.f };
	_vec3 vAt = { 0,0.f,1.f };
	_vec3 vUp = { 0,1,0 };

	_float fFov = D3DXToRadian(60.f);
	_float fAspect = (_float)WINCX / WINCY;
	_float fNear = 0.1f;
	_float fFar = 1000.f;

	CGameObject* pGameObject = nullptr;
	pGameObject = CFirstCamera::Create(m_pGraphicDev, &vEye, &vAt, &vUp);
	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"Camera", pGameObject)))
		return E_FAIL;


	pGameObject = CTerrain::Create(m_pGraphicDev);

	if (nullptr == pGameObject)
		return E_FAIL;

	if (FAILED(pLayer->Add_GameObject(L"Terrain", pGameObject)))
		return E_FAIL;

	m_mapLayer.insert({ pLayerTag , pLayer });

    return S_OK;
}

HRESULT CTestStage::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

	pGameObject = CPlayer::Create(m_pGraphicDev);

	if (FAILED(pLayer->Add_GameObject(L"Player", pGameObject)))
		return E_FAIL;

	pGameObject = CTestCharacter::Create(m_pGraphicDev);

	if (FAILED(pLayer->Add_GameObject(L"TestCharacter", pGameObject)))
		return E_FAIL;

	pGameObject = CWhiteMan::Create(m_pGraphicDev);

	if (FAILED(pLayer->Add_GameObject(L"WhiteMan", pGameObject)))
		return E_FAIL;


	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;

}

HRESULT CTestStage::Ready_Prototype()
{
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

	//TODO : 아래와 같이 벡터 직접 넣는대신 정적 멤버 함수로 대체하기 
	//White Man Texture 
	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, CWhiteMan::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_WhiteManTexture", pCom_Texture)))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_WhiteManAnimation", Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, CWhiteMan::GetAnimSources()))))
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
	CDataMgr<CWhiteMan>::GetInstance()->DestroyInstance();
	CDataMgr<CPlayer>::GetInstance()->DestroyInstance();
	CScene::Free();
}
