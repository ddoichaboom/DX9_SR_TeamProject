#include "pch.h"
#include "CLogo.h"
#include "CVideoMgr.h"
#include "CSoundMgr.h"
#include "CDInputMgr.h"

CLogo::CLogo(LPDIRECT3DDEVICE9 pGraphicDev) 
	: CScene(pGraphicDev)
{
}

CLogo::~CLogo()
{
}

HRESULT CLogo::Ready_Scene()
{
	if (FAILED(CVideoMgr::GetInstance()->ReadyVideo(g_hWnd, m_logoVideoName.c_str())))
	{
		CVideoMgr::GetInstance()->SetPlayFlag(false);
		return E_FAIL;
	}
	else
	{
		CVideoMgr::GetInstance()->Play();
		CSoundMgr::GetInstance()->PlayBGM(m_logoSoundName.c_str());
	}
	return S_OK;
}

_int CLogo::Update_Scene(const _float& fTimeDelta)
{
	if (CDInputMgr::GetInstance()->Key_Down(DIK_P) || CVideoMgr::GetInstance()->IsFinished())
	{
		CVideoMgr::GetInstance()->SetPlayFlag(false);
		CVideoMgr::GetInstance()->Cleanup();
		CSoundMgr::GetInstance()->StopAll();
		return RET_DEAD;
	}
	return RET_NONE;
}

CLogo* CLogo::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLogo* pLogo = new CLogo(pGraphicDev);

	if (FAILED(pLogo->Ready_Scene()))
	{
		Safe_Release(pLogo);
		MSG_BOX("LOGO Create Failed");
		return nullptr;
	}

	return pLogo;
}


void CLogo::Free()
{
	//CVideoMgr::GetInstance()->Cleanup();
	//CSoundMgr::GetInstance()->StopAll();
	CScene::Free();
}
