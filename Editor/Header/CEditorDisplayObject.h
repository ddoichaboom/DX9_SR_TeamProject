#pragma once
#include "CEditorObject.h"


class CEditorDisplayObject : public CEditorObject
{
protected:
    explicit        CEditorDisplayObject(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual         ~CEditorDisplayObject();

public:
    static vector<TextureSource>& GetTextureSources()
    {
        return m_vTextureSource;
    }

public:
    virtual HRESULT         Ready_GameObject() override;
    virtual _int            Update_GameObject(const _float& fTimeDelta) override;
    virtual void            LateUpdate_GameObject(const _float& fTimeDelta) override;
    virtual void            Render_GameObject() override;

protected:
    HRESULT                 Add_Component();
    virtual void		    Free() override;

public:
    void				Set_DisplayObjectType(DISPLAY_OBJECT_TYPE eObjectType, _uint iTextureId = 0);
    DISPLAY_OBJECT_TYPE Get_DisplayObjectType() const { return m_eObjectType; }
              
    void                Set_TextureIdx(_uint iIdx) { m_iTextureID = iIdx; }
    _uint               Get_TextureIdx() const { return m_iTextureID; }

public:
    // 기본 생성 (위치만)
    static CEditorDisplayObject* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

    // 전체 파라미터 지정 - 텍스처 포함 
    static CEditorDisplayObject* Create(LPDIRECT3DDEVICE9 pGraphicDev,
                                        _vec3 vPos, _vec3 vRot, _vec3 vScale,
                                        DISPLAY_OBJECT_TYPE iType, _uint iTextureId = 0);
private:
    DISPLAY_OBJECT_TYPE				m_eObjectType;
    _uint							m_iTextureID;

private:
    static vector<TextureSource>    m_vTextureSource;
};

