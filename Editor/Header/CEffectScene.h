#pragma once
#include "CEditorScene.h"

class CEditorCamera;
class CToolBar;
class CHierarchy;
class CEditorFloor;
namespace Engine
{
	class CParticleEmitter;
}
enum EFFECT_TYPE { EF_BLOOD, EF_TRAIL,EF_FLARE,EF_EXP, EF_BEAM_FLARE, EF_BODY, EF_HITUI, 
	EF_BOSS_TRAIL, EF_TAKEDOWN,EF_TOONFLASH,EF_TOONFOG,EF_SODAUI,EF_BOSSHPUI, EF_END };

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
	void						SetEmitter(EFFECT_TYPE _type);
	CParticleEmitter*			GetEmitter();
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
	map<EFFECT_TYPE,CParticleEmitter*>	m_mapParticle;
	CParticleEmitter*			m_pCurParticle;
	CEditorFloor*				m_pFloor;

};

