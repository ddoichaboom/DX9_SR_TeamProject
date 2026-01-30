#include "pch.h"
#include "CEnding.h"
#include "CVideoMgr.h"
#include "CSoundMgr.h"
#include "CDInputMgr.h"

CEnding::CEnding(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev)
{
}

CEnding::~CEnding()
{
}

HRESULT CEnding::Ready_Scene()
{

	if (FAILED(CVideoMgr::GetInstance()->ReadyVideo(g_hWnd, m_endingVideoName.c_str())))
	{
		CVideoMgr::GetInstance()->SetPlayFlag(false);
		return E_FAIL;
	}
	else
	{
		CVideoMgr::GetInstance()->Play();
		CSoundMgr::GetInstance()->PlayBGM(m_endingSoundName.c_str(),1.8f);
	}
	return S_OK;
}

_int CEnding::Update_Scene(const _float& fTimeDelta)
{
	if (CVideoMgr::GetInstance()->IsFinished() ||CDInputMgr::GetInstance()->Key_Down(DIK_P))
	{
		CVideoMgr::GetInstance()->SetPlayFlag(false);
		CVideoMgr::GetInstance()->Cleanup();
		CSoundMgr::GetInstance()->StopAll();
		return RET_DEAD;
	}
	return RET_NONE;
}


CEnding* CEnding::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEnding* pEnding = new CEnding(pGraphicDev);

	if (FAILED(pEnding->Ready_Scene()))
	{
		Safe_Release(pEnding);
		MSG_BOX("Ending Create Failed");
		return nullptr;
	}

	return pEnding;
}


void CEnding::Free()
{
	//CVideoMgr::GetInstance()->Cleanup();
	//CSoundMgr::GetInstance()->StopAll();
	CScene::Free();
}
