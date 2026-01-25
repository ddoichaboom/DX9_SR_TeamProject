#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CCubeTex;
	class CTransform;
	class CCubeTexture;
	class CCollision;
	class CCollider;
}

class CSoda;

class CVendingMachine : public CGameObject
{
protected:
	explicit	CVendingMachine(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit	CVendingMachine(const CVendingMachine& rhs);
	virtual		~CVendingMachine();

public:
	static vector<TextureSource>& GetTextureSources()
	{
		return m_vTextureSource;
	}

public:
	virtual		HRESULT				Ready_GameObject() override;
	virtual		_int				Update_GameObject(const _float& fTimeDelta) override;
	virtual		void				LateUpdate_GameObject(const _float& fTimeDelta) override;
	virtual		void				Render_GameObject() override;
				void				OnCollision(CollisionInfo info);

				void				Dispense();		// Soda 배출		
				void				CreateSoda();
protected:
	virtual		HRESULT				Add_Component() override;
	virtual		void				Free() override;

public:
	void							SetPos(_vec3 _pos);
	void							SetAngle(_vec3 _rot);
	void							SetScale(_vec3 _scale);
	void							Set_ColliderScale(_vec3 _scale);
	virtual		void				Activate() override;
	virtual		void				Deactivate() override;

public:
	static CVendingMachine* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	Engine::CCubeTex*		m_pBufferCom;
	Engine::CTransform*		m_pTransformCom;
	Engine::CCubeTexture*	m_pTextureCom;
	Engine::CCollision*		m_pCollisionCom;
	Engine::CCollider*		m_pCollider;
	const _tchar*			m_szColliderName = L"VendingMachine";

	CSoda*					m_pSoda;
	_vec3					m_vColliderScale;		// 콜라이더 스케일
	_vec3					m_vDispensPos;			// 소다 배출 위치

	_bool					m_bDispens;
	_float					m_fTime;
	_uint					m_iCount;

private:
	static vector<TextureSource>    m_vTextureSource;

public :
	static wstring	szSodaMakeSFX;
};

