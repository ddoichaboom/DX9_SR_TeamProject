#include "pch.h"
#include "CPistol.h"

#include "CFlare.h"
#include "CSoundMgr.h"
#include "CPoolMgr.h"
#include "CManagement.h"


wstring CPistol::szPistolReloadSFX	= L"Pistol_Reload_SFX.wav";
wstring CPistol::szPistolShotSFX	= L"Pistol_Shot_SFX.wav";

CPistol::CPistol(LPDIRECT3DDEVICE9 pGraphicDev)
	: CWeapon(pGraphicDev)
{
}

CPistol::CPistol(const CPistol& rhs)
	: CWeapon(rhs)
{
}

CPistol::~CPistol()
{
}

HRESULT CPistol::Ready_GameObject()
{
	m_bSelect = false;
	// 파워 / attack cool time
	m_fPower = 3;
	m_fCoolTime = 0.1f;
	// 최대 불렛
	m_iMaxBullet = 10;
	m_iNowBullet = m_iMaxBullet;

	m_vPos = { WINCX - 200.f, WINCY - 200.f, 0.f };
	m_vPos = { m_vPos.x - WINCX * 0.5f, -m_vPos.y + WINCY * 0.5f, 0.f };
	
	return S_OK;
}

_int CPistol::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CGameObject::Update_GameObject(fTimeDelta);
	m_fTime += fTimeDelta;

	return iExit;
}

void CPistol::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CPistol::Render_GameObject()
{

}

void CPistol::Activate()
{
}

void CPistol::Deactivate()
{
}

CPistol* CPistol::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPistol* pWeapon = new CPistol(pGraphicDev);

	if (FAILED(pWeapon->Ready_GameObject()))
	{
		Safe_Release(pWeapon);
		MSG_BOX("Pistol Create Failed");
		return nullptr;
	}

	return pWeapon;
}

_bool CPistol::Can_Fire()
{
	if (m_bIsEmpty)
		return false;

	if (!m_bShootAble)
		return false;

	if (m_fTime < m_fCoolTime)
		return false;

	return true;
}

void CPistol::Fire()
{	
	CSoundMgr::GetInstance()->PlayWeaponSound(CPistol::szPistolShotSFX.c_str());
	m_fTime = 0.f;
	m_iNowBullet--;

	if (m_iNowBullet <= 0)
	{
		m_iNowBullet = 0;
		m_bIsEmpty = true;
	}

	CFlare* flare = CPoolMgr::GetInstance()->Get_Object<CFlare>();
	if (flare)
	{
		CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer")->Add_GameObject(flare);
		_vec3 FlarePos = m_vPos + m_vFlarePosOffset;
		flare->SetPos(FlarePos);
		flare->Reset();
	}

}

void CPistol::Reload()
{
	m_fTime = 0.f;
	m_iNowBullet = m_iMaxBullet;
	m_bShootAble = true;
	m_bIsEmpty = false;

	
}

HRESULT CPistol::Add_Component()
{
	return S_OK;
}

void CPistol::Free()
{
	CWeapon::Free();
}
