#pragma once
#include "CEditorFloor.h"
class CEditorSlopeFloor : public CEditorFloor
{
protected:
    explicit        CEditorSlopeFloor(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual         ~CEditorSlopeFloor();

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
    // ========== 경사 관련 메서드 ==========
    void            Set_SlopeAngle(_float fAngle);
    _float          Get_SlopeAngle() const { return m_fSlopeAngle; }

    void            Set_SlopeDirection(SLOPE_DIR eDir);
    SLOPE_DIR       Get_SlopeDirection() const { return m_eSlopeDir; }

    virtual void    Set_FloorType(_uint iType) override;

    //  좌표 계산 헬퍼 함수 
    // 반대쪽 끝점 위치 (경사 각도/방향에 따라 계산)
    _vec3           Get_OppositeEndPosition();
    _vec3           Get_OppositeEndPosition(const _vec3& vDupDir);

protected:
    _float          m_fSlopeAngle;          // 경사 각도 (0 ~ 89도)
    SLOPE_DIR       m_eSlopeDir;            // 경사 방향

public:
    // 기본 생성 (배치용)
    static CEditorSlopeFloor* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

    // 전체 파라미터 (맵 로드용)
    static CEditorSlopeFloor* Create(LPDIRECT3DDEVICE9 pGraphicDev,
                                    _vec3 vPos, _vec3 vRot, _vec3 vScale);

    // 텍스처 + 경사 설정 포함 (복제/로드용)
    static CEditorSlopeFloor* Create(LPDIRECT3DDEVICE9 pGraphicDev,
                                    _vec3 vPos, _vec3 vScale,
                                    _uint iFloorType,
                                    _float fSlopeAngle,
                                    SLOPE_DIR eSlopeDir);

protected:
    virtual void    Free() override;

public:
    // 경사 방향에 따른 회전 계산
    void            Calculate_Rotation();

private:
    static vector<TextureSource> m_vTextureSource;
};

