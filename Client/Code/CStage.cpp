#include "pch.h"
#include "CStage.h"
#include "CBackGround.h"

CStage::CStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev), m_pBackGround(nullptr)
{
}

CStage::~CStage()
{
}

_int CStage::Update_Scene(const _float& fTimeDelta)
{
	_int iExit = Engine::CScene::Update_Scene(fTimeDelta);
	return iExit;
}

void CStage::LateUpdate_Scene(const _float& fTimeDelta)
{
	Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

void CStage::Render_Scene()
{
}

void CStage::Free()
{
	CScene::Free();
}
