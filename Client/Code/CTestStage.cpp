#include "pch.h"
#include "CTestStage.h"
#include "CProtoMgr.h"
#include "CTestCharacter.h"

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
	_vec3 eye = { 0,0,-2.f };
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

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;

}

HRESULT CTestStage::Ready_Prototype()
{
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_RcTex", Engine::CRcTex::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_Transform", Engine::CTransform::Create(m_pGraphicDev))))
		return E_FAIL;

	//상태값 = 이 컴포넌트를 소유하는 오브젝트의 상태 (Idle, Attack...) 
	//상태값, 경로 
	vector<TextureSource> vTextureSource =
	{
		{ 0, L"../Bin/Resource/Texture/Test/White_Idle.png" },
		{ 1, L"../Bin/Resource/Texture/Test/White_Copy.png" },
		{ 2, L"../Bin/Resource/Texture/Test/White_Copy_DDS.dds" }
	};
	CTexture* pCom_Texture = Engine::CTexture::Create(m_pGraphicDev, vTextureSource);
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TestTexture", pCom_Texture)))
		return E_FAIL;

	//상태값, 마지막 행 번호,  마지막 열 번호, 프레임이 끝나는 열 번호, 루프 유무, 플레이 속도 = 0.12f
	//AnimationSource source = { 0,1,5,5, true, 0.11f};
	vector<AnimationSource> vAnimSource =
	{
		{ 0,1,5,5, true, 0.11f},
		{ 1,1,5,5, true, 0.11f},
		{ 2,1,5,5, true, 0.11f},
	};

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TestAnimation", Engine::CAnimation::Create(m_pGraphicDev, pCom_Texture, vAnimSource))))
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
