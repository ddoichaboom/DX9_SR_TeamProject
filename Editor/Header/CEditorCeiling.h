#pragma once
#include "CEditorObject.h"

namespace Engine
{
    class CTexture;
}

class CEditorCeiling : public CEditorObject
{
private:
    explicit        CEditorCeiling(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual         ~CEditorCeiling();

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
    void            Set_TextureIdx(_int iIdx);
    _int            Get_TextureIdx() const { return m_iTextureIdx; }

    void            Set_CeilingType(_uint iType);
    _uint           Get_CeilingType() const { return m_iCeilingType; }

protected:
    CTexture*       m_pTextureCom;
    _int            m_iTextureIdx;      // 0 ~ 7 (아틀라스 인덱스)
    _uint           m_iCeilingType;       // enum 값 

public:
    // 기본 생성 (위치만)
    static CEditorCeiling* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

    // 전체 파라미터 지정 - 텍스처 제외
    static CEditorCeiling* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot, _vec3 vScale);

    // 전체 파라미터 지정 텍스처 포함 
    static CEditorCeiling* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot, _vec3 vScale, _uint iType, _int iIdx, _int iRoomIndex);

private:
    static vector<TextureSource> m_vTextureSource;

protected:
    virtual void    Free() override;
};

