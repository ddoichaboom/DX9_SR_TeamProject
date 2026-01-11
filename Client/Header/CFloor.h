#pragma once
#include "CTerrain.h"

class CFloor : public CTerrain
{
private:
    explicit CFloor(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CFloor(const CFloor& rhs);
    virtual ~CFloor();

public:
    static vector<TextureSource>& GetTextureSources()
    {
        return m_vTextureSource;
    }
    static vector<AnimationSource>& GetAnimSources()
    {
        return m_vAnimSource;
    }

public:
    virtual HRESULT     Ready_GameObject() override;
    virtual _int        Update_GameObject(const _float& fTimeDelta) override;
    virtual void        LateUpdate_GameObject(const _float& fTimeDelta) override;
    virtual void        Render_GameObject() override;
    void                Set_FloorType(_uint eFloorType);

private:
    HRESULT             Add_Component();
public:
    // 기본 생성 
    static CFloor* Create(LPDIRECT3DDEVICE9 pGraphicDev);

    static CFloor* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

    // 확장 생성 (위치, 회전, 스케일)
    static CFloor* Create(LPDIRECT3DDEVICE9 pGraphicDev,
                            _vec3 vPos, _vec3 vRot, _vec3 vScale);

private:
    virtual void Free() override;

private:
    static vector<TextureSource>    m_vTextureSource;
    static vector<AnimationSource>  m_vAnimSource;
};

