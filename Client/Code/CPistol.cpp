#include "pch.h"
#include "CPistol.h"

CPistol::CPistol(LPDIRECT3DDEVICE9 pGraphicDev)
	: CWeapon(pGraphicDev)
{
}

CPistol::CPistol(const CWeapon& rhs)
	: CWeapon(rhs)
{
}

CPistol::~CPistol()
{
}

HRESULT CPistol::Ready_GameObject()
{
	m_bSelect = false;
	// 파워 / 정확도 / attack cool time
	m_iPower = 3;
	m_fCoolTime = 0.1f;
	// 최대 불렛
	m_iMaxBullet = 9;
	m_iNowBullet = m_iMaxBullet;
	return S_OK;
}

_int CPistol::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bSelect == false)
		return RET_NONE;

	int iExit = CGameObject::Update_GameObject(fTimeDelta);
	m_fTime += fTimeDelta;

	return iExit;
}

void CPistol::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_bSelect == false)
		return;

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
	if (!Can_Fire())
		return;

	m_fTime = 0.f;
	m_iNowBullet--;

	if (m_iNowBullet <= 0)
	{
		m_iNowBullet = 0;
		m_bIsEmpty = true;
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
