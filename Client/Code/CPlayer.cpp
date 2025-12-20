#include "pch.h"
#include "CPlayer.h"
#include "CProtoMgr.h"
#include "CRenderer.h"


CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	:CCharacter(pGraphicDev)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	:CCharacter(rhs)
{
}

CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;

	return S_OK;
}

_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CCharacter::Update_GameObject(fTimeDelta);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
	return iExit;

}

void CPlayer::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CCharacter::LateUpdate_GameObject(fTimeDelta);
}

void CPlayer::Render_GameObject()
{
	// ..
}

HRESULT CPlayer::Add_Component()
{
	if (FAILED(CCharacter::Add_Component())) return E_FAIL;
	return S_OK;
}

void CPlayer::Key_Input(const _float& fTimeDelta)
{
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayer* pPlayer = new CPlayer(pGraphicDev);

	if (FAILED(pPlayer->Ready_GameObject()))
	{
		Safe_Release(pPlayer);
		MSG_BOX("pPlayer Create Failed");
		return nullptr;
	}

	return pPlayer;
}

void CPlayer::Free()
{
	CCharacter::Free();
}