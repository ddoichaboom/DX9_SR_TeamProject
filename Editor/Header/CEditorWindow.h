#pragma once
#include "CEditorObject.h"

class CEditorWindow : public CEditorObject
{
private:
    explicit        CEditorWindow(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual         ~CEditorWindow();

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

public:
    void            Set_TextureIdx(_uint iIdx) { m_iTextureIdx = iIdx; }
    _uint           Get_TextureIdx() const { return m_iTextureIdx; }

private:
    HRESULT         Add_Component();

public:
    // 기본 생성 (위치만)
    static CEditorWindow* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

    // 확장 생성 (위치, 회전, 크기 - 로드 시 사용)
    static CEditorWindow* Create(LPDIRECT3DDEVICE9 pGraphicDev,
                                _vec3 vPos, _vec3 vRot, _vec3 vScale);

private:
    _uint           m_iTextureIdx;  // 텍스처 인덱스 (기본 0)

private:
    static vector<TextureSource> m_vTextureSource;
    virtual void    Free() override;

};

