#include "pch.h"
#include "CPlayerPart.h"
#include "CAnimation.h"

CPlayerPart::CPlayerPart(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCharacter(pGraphicDev), m_pAnimationCom(nullptr), m_eWeaponState(0)
{
}

CPlayerPart::CPlayerPart(const CPlayerPart& rhs)
	: CCharacter(rhs), m_pAnimationCom(nullptr), m_eWeaponState(0)
{
}

CPlayerPart::~CPlayerPart()
{

}

void CPlayerPart::Set_WeaponState(_uint eState)
{
	m_eWeaponState = eState;
}

bool CPlayerPart::IsAnimationEnd()
{
	if(nullptr == m_pAnimationCom)
		return false;

	return m_pAnimationCom->IsEnd();
}
