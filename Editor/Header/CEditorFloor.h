#pragma once
#include "CEditorObject.h"

namespace Engine
{
    class CTexture;
}

class CEditorFloor : public CEditorObject
{
protected:
    explicit        CEditorFloor(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual         ~CEditorFloor();

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

    virtual void    Set_FloorType(_uint iType);
    _uint           Get_FloorType() const { return m_iFloorType; }

protected:
    CTexture*       m_pTextureCom;      // 텍스처 컴포넌트 (새로 추가)
    _int            m_iTextureIdx;      // 0 ~ 7 (아틀라스 인덱스)
    _uint           m_iFloorType;       // enum 값 

public:
    // 기본 생성 (위치만)
    static CEditorFloor* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

    // 전체 파라미터 지정 - 텍스처 제외
    static CEditorFloor* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot, _vec3 vScale);
    
    // 전체 파라미터 지정 - 텍스처 포함 
    static CEditorFloor* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot, _vec3 vScale, _uint iType, _int iIdx, _int iRoomIndex);

protected:
    virtual void    Free() override;

private:
    static vector<TextureSource> m_vTextureSource;
};

