#include "pch.h"
#include "CWeapon.h"

CWeapon::CWeapon(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
    , m_bIsEmpty(false), m_bSelect(false)
    , m_iNowBullet(0), m_iMaxBullet(0), m_iPower(0)
    , m_fCoolTime(0.f), m_fRange(0.f), m_fTime(0.f)
{

}

CWeapon::CWeapon(const CWeapon& rhs)
    : CGameObject(rhs)
    , m_bIsEmpty(false), m_bSelect(false)
    , m_iNowBullet(0), m_iMaxBullet(0), m_iPower(0)
    , m_fCoolTime(0.f), m_fRange(0.f), m_fTime(0.f)
{
}

CWeapon::~CWeapon()
{
}

HRESULT CWeapon::Ready_GameObject()
{
    return S_OK;
}

_int CWeapon::Update_GameObject(const _float& fTimeDelta)
{
    int iExit = CGameObject::Update_GameObject(fTimeDelta);

    return iExit;
}

void CWeapon::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CWeapon::Render_GameObject()
{
}

void CWeapon::Activate()
{
    CGameObject::Activate();
}

void CWeapon::Deactivate()
{
    CGameObject::Deactivate();
}

HRESULT CWeapon::Add_Component()
{
    return S_OK;
}

void CWeapon::Free()
{
    CGameObject::Free();
}
