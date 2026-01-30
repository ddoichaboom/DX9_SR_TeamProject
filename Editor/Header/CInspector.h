#pragma once
#include "CBase.h"


class CEditorCamera;
class CEditorScene;
class CEditorObject;
class CEditorWall;
class CEditorSpawnPoint;
class CEditorFloor;
class CEditorCeiling;
class CEditorSlopeFloor;
class CEditorMapCollider;
class CEditorTriggerBox;
class CEditorDoor;
class CEditorInteractObject;
class CEditorDisplayObject;

class CInspector : public CBase
{
private:
    explicit CInspector();
    virtual ~CInspector();

public:
    HRESULT Ready_Inspector(CEditorCamera* pCamera, CEditorScene* pScene);
    void    Update_Inspector();
    void    Render_Inspector();

private:
    void    Render_CameraProperties();
    void    Render_FloorTextureUI(CEditorFloor* pFloor);
    void    Render_CeilingTextureUI(CEditorCeiling* pCeiling);
    void    Render_WallTextureUI(CEditorWall* pWall);
    void    Render_MapColliderProperties(CEditorMapCollider* pCollider);
    void    Render_TriggerBoxProperties(CEditorTriggerBox* pTrigger);
    void    Render_DoorProperties(CEditorDoor* pDoor);
    void    Render_InteractObjectProperties(CEditorInteractObject* pInteract);
    void    Render_DisplayObjectProperties(CEditorDisplayObject* pDisplay);

private:
    void    Render_ObjectProperties();
    void    Render_WallProperties(CEditorWall* pWall);
    void    Render_MonsterSpawnPointProperties(CEditorSpawnPoint* pSpawn);

public:
    static CInspector* Create(CEditorCamera* pCamera, CEditorScene* pScene);

    void    Set_SelectedType(int iType) { m_iSelectedType = iType; }

private:
    CEditorCamera* m_pCamera;

    // 선택된 오브젝트 타입 (임시)
    int             m_iSelectedType;

    CEditorScene*   m_pScene;
    
private:
    virtual void Free() override;

};

