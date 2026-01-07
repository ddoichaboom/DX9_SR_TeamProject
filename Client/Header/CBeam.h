#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcTexUp;
	class CTransform;
	class CTexture;
	class CCollider;
}

class CBeam :
    public CGameObject
{
protected:
	explicit		CBeam(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual			~CBeam();

public:
	static CBeam* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static vector<TextureSource>& GetTextureSources()
	{
		return m_vTextureSource;
	}

public :
	HRESULT			Ready_GameObject() override;
	_int			Update_GameObject(const _float& fTimeDelta) override;
	void			LateUpdate_GameObject(const _float& fTimeDelta) override;
	void			Render_GameObject() override;

protected:
	HRESULT			Add_Component() override;
	virtual void	Free();

public:
	bool			CheckCollision(CCollider* _pCollider);
	void			SetPos(_vec3 _pos) override;
	void			SetShootDir(_vec3 _dir); 

protected:
	static vector<TextureSource> m_vTextureSource;
	//버텍스가 RcTexUp 타입임. 정의 참고 
	Engine::CRcTexUp* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

	_vec3			m_vPos;

	_matrix			m_matScale;
	_matrix			m_matTrans;
	_matrix			m_matRot;

	_vec3			m_vShootDir;
	_vec3			m_vRotAxis;
	float			m_fTime;

};

