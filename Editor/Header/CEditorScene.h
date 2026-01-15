#pragma once
#include "CScene.h"

class CEditorCamera;
class CGrid;
class CEditorObject;
class CToolBar;
class CMousePicker;
class CSelectionMgr;
class CHierarchy;

class CEditorScene : public Engine::CScene
{

private:
	explicit CEditorScene(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEditorScene();

public:
	virtual HRESULT				Ready_Scene() override;
	virtual _int				Update_Scene(const _float& fTimeDelta) override;
	virtual void				LateUpdate_Scene(const _float& fTimeDelta) override;
	virtual void				Render_Scene() override;

public:
	CEditorCamera*				Get_Camera() { return m_pCamera; }
	LPDIRECT3DDEVICE9			Get_GraphicDev() { return m_pGraphicDev; }
	CGrid*						Get_Grid() { return m_pGrid; }

public:
	void						Add_Object(CEditorObject* pObject);
	void						Remove_Object(CEditorObject* pObject);
	void						Clear_AllObjects();
	list<CEditorObject*>&		Get_ObjectList() { return m_ObjectList; }

	void						Set_SelectedObject(CEditorObject* pObj);
	CEditorObject*				Get_SelectedObject() const;

	void						Add_SelectedObject(CEditorObject* pObj);
	void						Remove_SelectedObject(CEditorObject* pObj);
	void						Clear_SelectedObjects();

	// 배치 로직
	_vec3                       Pick_OnPlane(const _vec3& vRayPos, const _vec3& vRayDir,
												_float fPlaneY = 0.f);
	void                        Place_Floor(const _vec3& vPos);
	void						Place_Dynamic_Floor(const _vec3& vPos);
	void                        Place_Cube(const _vec3& vPos);
	void                        Place_Ceiling(const _vec3& vPos);
	void                        Place_Wall(const _vec3& vPos);
	void                        Place_SpawnPlayer(const _vec3& vPos);
	void                        Place_SpawnMonster(const _vec3& vPos);

public:
	void						Set_ToolBar(CToolBar* pToolBar) { m_pToolBar = pToolBar; }
	void						Set_Hierarchy(CHierarchy* pHierarchy) { m_pHierarchy = pHierarchy; }

private:
	void						Handle_Input();		// 입력 처리
	void						Handle_Duplicate();
	void						Handle_Delete();
	void						Handle_Left_Click();
	void						Handle_Arrow();

private:
	LPDIRECT3DDEVICE9			m_pGraphicDev;

	CEditorCamera*				m_pCamera;
	CGrid*						m_pGrid;
	list<CEditorObject*>		m_ObjectList;
	CToolBar*					m_pToolBar;
	CMousePicker*				m_pMousePicker;
	CSelectionMgr*				m_pSelectionMgr;
	CHierarchy*					m_pHierarchy;


public:
	static		CEditorScene* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual		void			Free() override;
};

