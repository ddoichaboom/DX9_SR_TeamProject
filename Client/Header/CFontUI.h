#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "CDoorLeft.h"

class CBaseUI;

class CFontUI : public CBase
{
protected:
	explicit	CFontUI();	
	explicit	CFontUI(FONT_TYPE eType, const _vec3& vPos, const _vec3& vScale);
	explicit	CFontUI(const CFontUI& rhs);
	virtual		~CFontUI();

public:
	static		CFontUI*	Create();
	static		CFontUI*	Create(FONT_TYPE eType, const _vec3& vPos, const _vec3& vScale);

	void		Set_Projection(bool bProj) { m_bProject = bProj; }
	
protected:
	virtual		void		Free();


public :
	virtual		HRESULT		Ready_Font();	
	virtual		_int		Update_GameObject(const _float& fTimeDelta);
	virtual		void		LateUpdate_GameObject(const _float& fTimeDelta);

public :
	void		Set_Position(const _vec3& vPos) { m_vPosition = vPos; }

	void		Set_Scale(const _vec3& vScale)	{ m_vScale = vScale; }
	void		Set_Text(const wstring& wText) { m_tData.pString = wText; }
	void		Set_Parent(CBaseUI* pParent);

	void		Set_Color(D3DXCOLOR eColor) { m_tData.Color = eColor; }

	

protected:
	FontData	m_tData;
	_vec3		m_vPosition;
	_vec3		m_vScale;
	_bool		m_bProject;

	FONT_TYPE	m_eFontType;

	CBaseUI*	m_pParentUI;
};

