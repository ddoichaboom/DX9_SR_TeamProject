#include "pch.h"
#include "CTestStage.h"
#include "CProtoMgr.h"
#include "CTestCharacter.h"
#include "CWhiteMan.h"


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

	_matrix View, Proj;
	_vec3 eye = { 0,20,-30.f };
	_vec3 at = { 0,0,1 };
	_vec3 up = { 0,1,0 };

	_float fFov = D3DXToRadian(60.f);
	_float fAspect = (_float)WINCX / WINCY;
	_float fNear = 0.1f;
	_float fFar = 1000.f;


	D3DXMatrixLookAtLH(&View, &eye, &at, &up);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &View);

	D3DXMatrixPerspectiveFovLH(&Proj, fFov, fAspect, fNear, fFar);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &Proj);

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
    return S_OK;
}

HRESULT CTestStage::Ready_GameLogic_Layer(const _tchar* pLayerTag)
{
	CLayer* pLayer = CLayer::Create();
	if (nullptr == pLayer)
		return E_FAIL;

	CGameObject* pGameObject = nullptr;
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

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Calculator", Engine::CCalculator::Create(m_pGraphicDev))))
		return E_FAIL;


	vector<TextureSource> vWhiteManSource =
	{
		{ 0, L"../Bin/Resource/Texture/Test/white_Idle_1024.png" }
		,{ 2, L"../Bin/Resource/Texture/Test/white_Aiming_1024.png" }
		,{ 3, L"../Bin/Resource/Texture/Test/white_AttackIdle_1024.png" }
		,{ 4, L"../Bin/Resource/Texture/Test/white_Attack_1024.png" }
		,{ 5, L"../Bin/Resource/Texture/Test/white_Attack2_1024.png" }
		,{ 6, L"../Bin/Resource/Texture/Test/white_Walk_1024.png" }
		,{ 7, L"../Bin/Resource/Texture/Test/white_Hit_1024.png" }
		,{ 8, L"../Bin/Resource/Texture/Test/white_DeadBack_512.png" }
	};

	vector<TextureSource> vTextureSource =
	{
		{ 0, L"../Bin/Resource/Texture/Player/Pika.png" }
	};

	CTexture* pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, vTextureSource);
		if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TestTexture", pCom_Texture)))
			return E_FAIL;

	pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, vWhiteManSource);
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_WhiteManTexture", pCom_Texture)))
		return E_FAIL;

	vector<AnimationSource> vAnimSource =
	{
		{ 0,1,5,5, true, 0.13f}		//IDLE
		,{ 2,1,4,3, false, 0.11f}	//Aiming
		,{ 3,1,3,2, true, 0.11f}	//AttackStart
		,{ 4,1,5,5, true, 0.11f}	//Attack1
		,{ 5,1,4,3, true, 0.09f}	//Attack2
		,{ 6,1,6,5, true, 0.11f}	//Walk
		,{ 7,1,3,2, true, 0.11f}	//Hit
		,{ 8,6,3,2, true, 0.11f}	//DeadBack
	};

	//if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TestAnimation", Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, vAnimSource))))
	//	return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_WhiteManAnimation", Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, vAnimSource))))
		return E_FAIL;

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
