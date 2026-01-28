#include "pch.h"
#include "CCredit.h"
#include "CVideoMgr.h"
#include "CSoundMgr.h"
#include "CDInputMgr.h"

CCredit::CCredit(LPDIRECT3DDEVICE9 pGraphicDev)
	: CScene(pGraphicDev)
{
}

CCredit::~CCredit()
{
}

HRESULT CCredit::Ready_Scene()
{
	if (FAILED(CVideoMgr::GetInstance()->ReadyVideo(g_hWnd, m_CreditVideoName.c_str())))
	{
		CVideoMgr::GetInstance()->SetPlayFlag(false);
		return E_FAIL;
	}
	else
	{
		CVideoMgr::GetInstance()->Play();
		CSoundMgr::GetInstance()->PlayBGM(m_CreditSoundName.c_str());
	}
	return S_OK;
}

_int CCredit::Update_Scene(const _float& fTimeDelta)
{
	if (CVideoMgr::GetInstance()->IsFinished())
	{
		CVideoMgr::GetInstance()->SetPlayFlag(false);
		CVideoMgr::GetInstance()->Cleanup();
		CSoundMgr::GetInstance()->StopAll();
		//return RET_DEAD;
	}
	return RET_NONE;
}

CCredit* CCredit::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCredit* pLogo = new CCredit(pGraphicDev);

	if (FAILED(pLogo->Ready_Scene()))
	{
		Safe_Release(pLogo);
		MSG_BOX("LOGO Create Failed");
		return nullptr;
	}

	return pLogo;
}


void CCredit::Free()
{
	//CVideoMgr::GetInstance()->Cleanup();
	//CSoundMgr::GetInstance()->StopAll();
	CScene::Free();
}
