#pragma once
#include "CEditorObject.h"

class CEditorInteractObject : public CEditorObject
{
protected:
    explicit        CEditorInteractObject(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual         ~CEditorInteractObject();

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
    void            SetBillBoard();

public:
    void            Set_ItemType(OBJ_ITEM_TYPE eObjItemType);
    OBJ_ITEM_TYPE   Get_ItemType() { return m_eObjectItemType; }

public:
    // 기본 생성 (위치만)
    static CEditorInteractObject* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

    // 전체 파라미터 지정 - 텍스처 포함 
    static CEditorInteractObject* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, OBJ_ITEM_TYPE iType);

private:
    OBJ_ITEM_TYPE       m_eObjectItemType;

protected:
    virtual void    Free() override;

private:
    static vector<TextureSource> m_vTextureSource;
};

