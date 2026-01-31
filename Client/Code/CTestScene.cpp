#include "pch.h"
#include "CTestScene.h"
#include "CDInputMgr.h"

CTestScene::CTestScene(LPDIRECT3DDEVICE9 pGraphicDev)
    : CScene(pGraphicDev)
{
}

CTestScene::~CTestScene()
{
}

HRESULT CTestScene::Ready_Scene()
{
    return S_OK;
}

_int CTestScene::Update_Scene(const _float& fTimeDelta)
{
    if (CDInputMgr::GetInstance()->Key_Down(DIK_P))
        return RET_DEAD;


    return RET_NONE;
}

CTestScene* CTestScene::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTestScene* pLogo = new CTestScene(pGraphicDev);

	if (FAILED(pLogo->Ready_Scene()))
	{
		Safe_Release(pLogo);
		MSG_BOX("TestScene Create Failed");
		return nullptr;
	}

	return pLogo;
}

void CTestScene::Free()
{
    CScene::Free();
}
