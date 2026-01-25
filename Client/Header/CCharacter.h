#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CStateComponent;
	class CCollision;
	class CCollider;
}

class CCharacter :
    public CGameObject
{
protected:
	explicit			CCharacter(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit			CCharacter(LPDIRECT3DDEVICE9 pGraphicDev, _float fHP);
	explicit			CCharacter(const CCharacter& rhs);
	virtual				~CCharacter();

public:
	HRESULT				Ready_GameObject() override;
	_int				Update_GameObject(const _float& fTimeDelta) override;
	void				LateUpdate_GameObject(const _float& fTimeDelta) override;
	void				Render_GameObject() PURE;

	//첫 콜라이더만 반환(콜라이더가 한 개인 객체면 이걸 사용)
	virtual CCollider*	GetCollider();
	//모든 콜라이더 반환
	virtual bool		GetAllCollider(vector<CCollider*>& _OutColliders);

	_float		GetHP() override { return m_fHP; }
	_float		GetMaxHP() override { return m_fMaxHP; }

protected:
	HRESULT				Add_Component() override;
	//void				Set_OnTerrain();

	virtual void		Move_ByCollision(COL_DIR& dir, _vec3 _diff);
protected:
	void				Free() override;
	virtual void		ChangeState(_uint nextStateID) {};

public:
	_float				GetAttackDamage() override;
	void				Activate() override;
	void				Deactivate() override;
	void				SetPos(_vec3 _pos) override;
	void				Rotate(ROTATION _Axis, _float _degree)  override;

	void				StopUpdate() { m_bStopUpdate = true; }
	void				StartUpdate() { m_bStopUpdate = false; }
protected:
	CRcTex*		m_pBufferCom;
	CTransform* m_pTransformCom;
	CTexture*	m_pTextureCom;
	CStateComponent* m_pStateCom;
	CCollision* m_pCollisionCom;

protected:
	_float m_fTime;
	_float m_fMaxHP;
	_float m_fHP;
	_float m_fAttackDamage;

	bool m_bStopUpdate;

};

