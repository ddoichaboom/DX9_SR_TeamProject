#pragma once
#include "CEditorScene.h"

class CEditorCamera;
class CToolBar;
class CHierarchy;
class CBlood;
class CParticleEmitter;
class CEditorFloor;

class CEffectScene :
    public CEditorScene
{
private:
	explicit CEffectScene(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEffectScene();

public:
	virtual HRESULT				Ready_Scene() override;
	virtual _int				Update_Scene(const _float& fTimeDelta) override;
	virtual void				LateUpdate_Scene(const _float& fTimeDelta) override;
	virtual void				Render_Scene() override;

public:
	CBlood*						GetEmitter();
	CEditorCamera*				Get_Camera() { return m_pCamera; }
	LPDIRECT3DDEVICE9			Get_GraphicDev() { return m_pGraphicDev; }

public:
	void						Set_ToolBar(CToolBar* pToolBar) { m_pToolBar = pToolBar; }
	void						Set_Hierarchy(CHierarchy* pHierarchy) { m_pHierarchy = pHierarchy; }

public:
	static		CEffectScene*	Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual		void			Free() override;

protected:
	CBlood*						m_pBlood;
	CEditorFloor*				m_pFloor;

};

