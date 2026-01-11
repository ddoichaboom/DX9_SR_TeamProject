#include "pch.h"
#include "CFloor.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "Engine_Enum.h"

//마지막 벡터 값은 외곽에서부터 제거될 픽셀 값이라고 생각하기 (이 비율만큼 uv를 땡겨서 랜더링함)
vector<TextureSource> CFloor::m_vTextureSource =
{
    { STATIC_FLOOR, L"../Bin/Resource/Texture/Terrain/Floor/STATIC_FLOOR/FLOORS.dds", true, 0, 7, 7, {2.f, 2.f} },

    { DYNAMIC_FLOOR_LAVA, L"../Bin/Resource/Texture/Terrain/Floor/DYNAMIC_FLOOR/LAVA.dds"}
};

vector<AnimationSource> CFloor::m_vAnimSource =
{
    { DYNAMIC_FLOOR_LAVA, 0, 4, 4, true, 0.75f}
};

CFloor::CFloor(LPDIRECT3DDEVICE9 pGraphicDev)
    : CTerrain(pGraphicDev)
{
    m_eTerrainType = TERRAIN_FLOOR;
}

CFloor::CFloor(const CFloor& rhs)
    : CTerrain(rhs)
{
    m_eTerrainType = TERRAIN_FLOOR;
}

CFloor::~CFloor()
{
}

HRESULT CFloor::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

_int CFloor::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);
    return iExit;
}

void CFloor::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CFloor::Render_GameObject()
{
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    if (FAILED(Ready_Material(D3DXCOLOR(0.6f, 0.4f, 0.2f, 1.f))))
        return;

    if (m_bIsAnimated && m_pAnimationCom)
        m_pAnimationCom->Render_Animation();        // Dynamic_Floor 애니메이션 Render
    else
        m_pTextureCom->Render_Texture();

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

    //TODO : 모든 타일에 텍스쳐 삽입이 완료되면 지우기 
    //그 전까지는 모든 타일의 Render_GameObject 끝에 아래 코드 추가 
    //아래 코드가 없으면 여기서 SetTexture에 들어간 텍스쳐가 텍스쳐가 없는 벽 등에 영향을 미침
    m_pGraphicDev->SetTexture(0, nullptr);
}

HRESULT CFloor::Add_Component()
{
    Engine::CComponent* pComponent = nullptr;

    // RcTex
    pComponent = m_pBufferCom = dynamic_cast<Engine::CRcTex*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Transform", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_FloorTexture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture", pComponent });

    // Animation
    pComponent = m_pAnimationCom = dynamic_cast<Engine::CAnimation*>
        (Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_FloorAnimation"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });

    return S_OK;
}

void CFloor::Set_FloorType(_uint eFloorType)
{
    if (eFloorType >= (int)DF_START)
    {
        m_bIsAnimated = true;
        if (m_pAnimationCom)
            m_pAnimationCom->Change_Animation(eFloorType);
    }
    else
    {
        m_bIsAnimated = false;
        if (m_pTextureCom)
        {
            m_pTextureCom->Change_Texture(eFloorType);
            m_pTextureCom->Set_Frame(_vec2(m_iTextureIdx, 0));
        }
    }
}

CFloor* CFloor::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CFloor* pInstance = new CFloor(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CFloor Create Failed");
        return nullptr;
    }

    return pInstance;
}

CFloor* CFloor::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CFloor* pInstance = new CFloor(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CFloor Create Failed");
        return nullptr;
    }


    // Transform 설정 
    pInstance->SetPos(vPos);
    pInstance->SetAngle(_vec3( - 90.f, 0.f, 0.f));

    //pInstance->m_pTransformCom->Update_Component(0.f);

    return pInstance;
}

CFloor* CFloor::Create(LPDIRECT3DDEVICE9 pGraphicDev,
                        _vec3 vPos, _vec3 vRot, _vec3 vScale)
{
    CFloor* pInstance = new CFloor(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CFloor Create Failed");
        return nullptr;
    }

    // Transform 설정 
    pInstance->SetPos(vPos);
    pInstance->SetAngle(vRot);
    pInstance->SetScale(vScale);

    ///pInstance->m_pTransformCom->Update_Component(0.f);


    return pInstance;
}

void CFloor::Free()
{
    CTerrain::Free();
}