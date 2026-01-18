#pragma once
#include "CTexture.h"

BEGIN(Engine)
class ENGINE_DLL CScrollTexture : public CTexture
{
protected:
	explicit				CScrollTexture();
	explicit				CScrollTexture(LPDIRECT3DDEVICE9 pGraphicDev,_float _speed, _vec2 _dir);
	explicit				CScrollTexture(const CScrollTexture& rhs);
	virtual					~CScrollTexture();

public:
	void					Update_Texture(const _float& fTimeDelta);
	void					Late_Update_Texture(); 
public:
	static CScrollTexture*	Create(LPDIRECT3DDEVICE9 pGraphicDev, vector<TextureSource>& _vData, _float _speed, _vec2 _dir = _vec2(1,1));
	static CScrollTexture*	Create(LPDIRECT3DDEVICE9 pGraphicDev, TextureSource _vData, _float _speed, _vec2 _dir = _vec2(1, 1));
	virtual CComponent*		Clone();	

protected:
	virtual void			Free();

protected:
	_vec2 m_vDir;
	_float m_fSpeed;

	_vec2 m_vUVPos = {};

};

END