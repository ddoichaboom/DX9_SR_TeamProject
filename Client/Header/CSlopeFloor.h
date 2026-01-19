#pragma once
#include "CFloor.h"
class CSlopeFloor : public CFloor
{
protected:
    explicit CSlopeFloor(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CSlopeFloor(const CSlopeFloor& rhs);
    virtual ~CSlopeFloor();

public:
    static vector<TextureSource>& GetTextureSources()
    {
        return m_vTextureSource;
    }

public:
    //  GameObject 인터페이스 
    virtual HRESULT     Ready_GameObject() override;
    virtual _int        Update_GameObject(const _float& fTimeDelta) override;
    virtual void        LateUpdate_GameObject(const _float& fTimeDelta) override;
    virtual void        Render_GameObject() override;

    //  경사 관련 메서드 
    void                Set_SlopeDirection(SLOPE_DIR eDir);
    SLOPE_DIR           Get_SlopeDirection() const { return m_eSlopeDir; }

    virtual void        Set_FloorType(_uint eFloorType);

protected:
    virtual HRESULT     Add_Component() override;

public:
    // 기본 생성
    static CSlopeFloor* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
    virtual void Free() override;

protected:
    SLOPE_DIR           m_eSlopeDir;        // 경사 방향

private:
    static vector<TextureSource> m_vTextureSource;

};

