#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcTexSide;
	class CTransform;
	class CTexture;
}

class CDoorRight : public CGameObject
{
private:
	explicit CDoorRight(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CDoorRight(const CDoorRight& rhs);
	virtual ~CDoorRight();

public:
	static vector<TextureSource>& GetTextureSources()
	{
		return m_vTextureSource;
	}

public:
	virtual HRESULT     Ready_GameObject() override;
	virtual _int        Update_GameObject(const _float& fTimeDelta) override;
	virtual void        LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual void        Render_GameObject() override;

	void				Set_DoorType(DOOR_TYPE eType);
	virtual void		Deactivate() override;
public:
	// 문 열림/닫힘 제어 함수 
	void				Open();
	void				Close();
	_bool				Is_Open() const { return m_bIsOpen; }
	_bool				Is_Animating() const { return m_bIsAnimating; }

	// Transform 설정
	void				Set_Position(const _vec3& vPos);
	void				Set_Rotation(const _vec3& vRot);
	void				Set_Scale(const _vec3& vScale);

public:
	static CDoorRight* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CDoorRight* Create(LPDIRECT3DDEVICE9 pGraphicDev,
								const _vec3& vPos,
								const _vec3& vRot = { 0.f, 0.f, 0.f },
								const _vec3& vScale = { 8.f, 16.f, 1.f });		// 좌우 반전

private:
	virtual HRESULT		Add_Component() override;
	virtual void		Free() override;

	void				Update_Animation(const _float& fTimeDelta);

private:
	Engine::CRcTexSide* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;


	// 애니메이션 관련
	_bool       m_bIsOpen;          // 현재 열린 상태인지
	_bool       m_bIsAnimating;     // 애니메이션 중인지
	_float      m_fCurrentAngle;    // 현재 회전 각도 (0 ~ 90도)
	_float      m_fTargetAngle;     // 목표 회전 각도
	_float      m_fAnimSpeed;       // 애니메이션 속도 (도/초)

	// 초기 Transform 저장
	_vec3       m_vInitialPos;
	_vec3       m_vInitialRot;
	_vec3       m_vInitialScale;

	DOOR_TYPE	m_eDoorType;

private:
	static vector<TextureSource> m_vTextureSource;
};

