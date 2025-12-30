#pragma once
#include "CEditorObject.h"

// 스폰 타입
enum SPAWN_TYPE
{
    SPAWN_PLAYER,   // 플레이어 스폰 지점
    SPAWN_MONSTER,  // 몬스터 스폰 지점
    SPAWN_END
};

class CEditorSpawnPoint : public CEditorObject
{
private:
    explicit        CEditorSpawnPoint(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual         ~CEditorSpawnPoint();

public:
    virtual HRESULT Ready_GameObject() override;
    virtual _int    Update_GameObject(const _float& fTimeDelta) override;
    virtual void    LateUpdate_GameObject(const _float& fTimeDelta) override;
    virtual void    Render_GameObject() override;

public:
    // 스폰 타입 설정/조회
    void            Set_SpawnType(SPAWN_TYPE eType) { m_eSpawnType = eType; }
    SPAWN_TYPE      Get_SpawnType() const { return m_eSpawnType; }

    // 몬스터 키 설정/조회 (SPAWN_MONSTER일 때만 사용)
    void            Set_MonsterKey(const string& strKey) { m_strMonsterKey = strKey; }
    const string&   Get_MonsterKey() const { return m_strMonsterKey; }

private:
    HRESULT         Add_Component();

private:
    SPAWN_TYPE      m_eSpawnType;       // 스폰 타입
    string          m_strMonsterKey;    // 몬스터 종류

public:
    // 기본 생성 (플레이어 스폰)
    static CEditorSpawnPoint* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, SPAWN_TYPE eType);

    // 전체 파라미터 지정 (맵 로드용)
    static CEditorSpawnPoint* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot, _vec3 vScale,
                                        SPAWN_TYPE eType, const string& strMonsterKey);

private:
    virtual void    Free() override;
};

