#pragma once

#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CTransform :  public CComponent
{
private:
	explicit CTransform();
	explicit CTransform(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTransform(const CTransform& rhs);
	virtual ~CTransform();

public:
	HRESULT				Ready_Transform();
	virtual		_int	Update_Component(const _float& fTimeDelta);
	virtual		void	LateUpdate_Component();

public:
	void		Set_Pos(_float _fX, _float _fY, _float _fZ)
	{
		m_vInfo[INFO_POS] = { _fX, _fY, _fZ };
	}

	void		Set_Pos(_vec3 _pos)
	{
		m_vInfo[INFO_POS] = _pos;
	}


	void		Move_Pos(const _vec3* pDir, const _float& fTimeDelta, const _float& fSpeed)
	{
		m_vInfo[INFO_POS] += *pDir * fTimeDelta * fSpeed;
	}

	void		Rotation(ROTATION eType, const _float& fAngle)
	{
		*(((_float*)&m_vAngle) + eType) += fAngle;
	}

	_matrix* Get_World() { return &m_matWorld; }

	void		Get_Info(INFO eType, _vec3* pInfo)
	{
		memcpy(pInfo, &m_matWorld.m[eType][0], sizeof(_vec3));
	}

	void	Set_World(_matrix* pWorld)
	{
		m_matWorld = *pWorld;
	}


	void			Chase_Target(const _vec3* pTargetPos, const _float& fTimeDelta, const _float& fSpeed);
	_matrix*		Compute_LookAtTarget(const _vec3* pTargetPos);

public:
	_vec3* Get_Info(INFO eType) { return &m_vInfo[eType]; }

	// Angle 
	_vec3		Get_Angle() const { return m_vAngle; }
	void		Set_Angle(_float fX, _float fY, _float fZ)
	{
		m_vAngle = { fX, fY, fZ };
	}

	// Scale
	_vec3		Get_Scale() const { return m_vScale; }
	void		Set_Scale(_float fX, _float fY, _float fZ)
	{
		m_vScale = { fX, fY, fZ };
	}

public:
	_vec3		m_vInfo[INFO_END];
	_vec3		m_vScale;
	_vec3		m_vAngle;

	_matrix		m_matWorld;

public:
	static CTransform* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();

private:
	virtual void	Free();

};

END