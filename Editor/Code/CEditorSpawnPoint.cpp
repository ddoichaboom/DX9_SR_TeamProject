#include "pch.h"
#include "CEditorSpawnPoint.h"
#include "CRenderer.h"

#include "CProtoMgr.h"
#include "CTransform.h"
#include "CCubeTex.h"

CEditorSpawnPoint::CEditorSpawnPoint(LPDIRECT3DDEVICE9 pGraphicDev)
    : CEditorObject(pGraphicDev)
    , m_eSpawnType(SPAWN_PLAYER)
    , m_strMonsterKey("")
{
}

CEditorSpawnPoint::~CEditorSpawnPoint()
{
}

HRESULT CEditorSpawnPoint::Ready_GameObject()
{
    FAILED_CHECK_RETURN(CEditorObject::Ready_GameObject(), E_FAIL);
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    // 기본 이름
    m_wstrName = L"SpawnPoint";

    return S_OK;
}

_int CEditorSpawnPoint::Update_GameObject(const _float& fTimeDelta)
{
    CEditorObject::Update_GameObject(fTimeDelta);

    // Renderer에 추가
    Engine::CRenderer::GetInstance()->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

    return 0;
}

void CEditorSpawnPoint::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CEditorObject::LateUpdate_GameObject(fTimeDelta);
}

void CEditorSpawnPoint::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    DWORD dOldAlphaTest;
    m_pGraphicDev->GetRenderState(D3DRS_ALPHATESTENABLE, &dOldAlphaTest);

    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    // 텍스처 설정 (기본 색상)
    if (nullptr == m_pTextureCom)
    {
        m_pGraphicDev->SetTexture(0, nullptr);
    }
    else
    {
        //m_pTextureCom->Set_Texture(0);
    }

    m_pGraphicDev->SetTexture(0, nullptr);

    // 스폰 타입에 따라 색상 구분
    DWORD dwColor = D3DCOLOR_ARGB(255, 255, 255, 255);  // 기본 흰색

    if (m_bSelected)
    {
        // 선택됨: 노란색 (모든 타입 공통)
        dwColor = D3DCOLOR_ARGB(255, 255, 255, 0);
    }
    else
    {
        // 스폰 타입별 색상
        if (m_eSpawnType == SPAWN_PLAYER)
        {
            // 플레이어: 초록색
            dwColor = D3DCOLOR_ARGB(255, 0, 255, 0);
        }
        else if (m_eSpawnType == SPAWN_MONSTER)
        {
            // 몬스터: 빨강색
            dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
        }
        else if (m_eSpawnType == SPAWN_BOSSMONSTER)
        {
            // 보스 몬스터 : 파란색 
            dwColor = D3DCOLOR_ARGB(255, 0, 0, 255);
        }
    }

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwColor);

    m_pBufferCom->Render_Buffer();

    // 렌더 상태 복원
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, dOldAlphaTest);
}

HRESULT CEditorSpawnPoint::Add_Component()
{
    Engine::CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[Engine::ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Buffer ( CubeTex )
    pComponent = m_pBufferCom = dynamic_cast<Engine::CVIBuffer*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CubeTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[Engine::ID_STATIC].insert({ L"Com_Buffer", pComponent });

    return S_OK;
}

CEditorSpawnPoint* CEditorSpawnPoint::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, SPAWN_TYPE eType)
{
    CEditorSpawnPoint* pInstance = new CEditorSpawnPoint(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorSpawnPoint Create Failed");
        return nullptr;
    }

    // 위치 설정
    pInstance->Set_Position(vPos);

    pInstance->Set_Scale(_vec3(5.f, 5.f, 5.f));

    // 스폰 타입 설정
    pInstance->Set_SpawnType(eType);

    // 이름 자동 설정
    if (eType == SPAWN_PLAYER)
        pInstance->Set_Name(L"PlayerSpawn");
    else if (eType == SPAWN_MONSTER)
        pInstance->Set_Name(L"MonsterSpawn");
    else if (eType == SPAWN_BOSSMONSTER)
        pInstance->Set_Name(L"BossMonsterSpawn");

    return pInstance;
}

CEditorSpawnPoint* CEditorSpawnPoint::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot, _vec3 vScale,
                            SPAWN_TYPE eType, const string& strMonsterKey)
{
    CEditorSpawnPoint* pInstance = new CEditorSpawnPoint(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorSpawnPoint Create Failed");
        return nullptr;
    }

    // Transform 전체 지정
    pInstance->Set_Scale(vScale);
    pInstance->Set_Rotation(vRot);
    pInstance->Set_Position(vPos);

    // 스폰 정보 설정
    pInstance->Set_SpawnType(eType);
    pInstance->Set_MonsterKey(strMonsterKey);

    if (eType == SPAWN_PLAYER)
        pInstance->Set_Name(L"PlayerSpawn");
    else if (eType == SPAWN_MONSTER)
        pInstance->Set_Name(L"MonsterSpawn");
    else if (eType == SPAWN_BOSSMONSTER)
        pInstance->Set_Name(L"BossMonsterSpawn");

    return pInstance;
}


void CEditorSpawnPoint::Free()
{
    CEditorObject::Free();
}