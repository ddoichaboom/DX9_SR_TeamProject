#pragma once
#include "CEditorObject.h"

namespace Engine
{
	class CTexture;
}

class CEditorDoor : public CEditorObject
{
protected:
    explicit        CEditorDoor(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual         ~CEditorDoor();

public:
    static vector<TextureSource>& GetTextureSources()
    {
        return m_vTextureSource;
    }

public:
    virtual HRESULT Ready_GameObject() override;
    virtual _int    Update_GameObject(const _float& fTimeDelta) override;
    virtual void    LateUpdate_GameObject(const _float& fTimeDelta) override;
    virtual void    Render_GameObject() override;

private:
    HRESULT         Add_Component();

public:
    void            Set_DoorType(DOOR_TYPE eDoorType);
    DOOR_TYPE       Get_DoorType() const { return m_eDoorType; }

    void            Set_DoorID(_uint iDoorID) { m_iDoorID = iDoorID; }
    _uint           Get_DoorID() const { return m_iDoorID; }

public:
    // 기본 생성 (XY 평면, 기본 크기)
    static CEditorDoor* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

    // 전체 파라미터 지정 (맵 로드용)
    static CEditorDoor* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot, _vec3 vScale);

    static CEditorDoor* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot, _vec3 vScale, DOOR_TYPE eDoorType, _int iDoorID);

private:
    Engine::CTexture*               m_pTextureCom;
    static vector<TextureSource>    m_vTextureSource;

protected:
    virtual void    Free() override;

private:
    DOOR_TYPE       m_eDoorType;
    _int            m_iDoorID;
};

