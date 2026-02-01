#include "pch.h"
#include "CEditorDoor.h"
#include "CRenderer.h"
#include "CProtoMgr.h"
#include "CTexture.h"

vector<TextureSource> CEditorDoor::m_vTextureSource =
{
    {DOOR_1, L"../Bin/Resource/Texture/Terrain/Door/DOOR_1.dds", false, 0, 0, 0, {1.f, 1.f}},
    {DOOR_2, L"../Bin/Resource/Texture/Terrain/Door/DOOR_2.dds", false, 0, 0, 0, {1.f, 1.f}},
    {DOOR_3, L"../Bin/Resource/Texture/Terrain/Door/DOOR_3.dds", false, 0, 0, 0, {1.f, 1.f}},
    {DOOR_ELEVATOR, L"../Bin/Resource/Texture/Terrain/Door/DOOR_ELEVATOR.dds", false, 0, 0, 0, {1.f, 1.f}}
};

CEditorDoor::CEditorDoor(LPDIRECT3DDEVICE9 pGraphicDev)
    : CEditorObject(pGraphicDev)
    , m_eDoorType(DOOR_1)
    , m_iDoorID(0)
    , m_pTextureCom(nullptr)
{
    m_eObjType = EDITOR_OBJ_DOOR;
}

CEditorDoor::~CEditorDoor()
{
}

HRESULT CEditorDoor::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);


    if (m_pTextureCom)
    {
        m_pTextureCom->Change_Texture(m_eDoorType);
        m_pTextureCom->Set_Frame(_vec2(0, 0));
    }

    // 기본 이름 설정
    m_wstrName = L"Door";

    return S_OK;
}

_int CEditorDoor::Update_GameObject(const _float& fTimeDelta)
{
    CEditorObject::Update_GameObject(fTimeDelta);

    // Renderer에 추가
    Engine::CRenderer::GetInstance()->Add_RenderGroup(Engine::RENDER_NONALPHA, this);

    return 0;
}

void CEditorDoor::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CEditorObject::LateUpdate_GameObject(fTimeDelta);
}

void CEditorDoor::Render_GameObject()
{
    // 텍스처 스테이트 저장 
    DWORD dwOldColorOp, dwOldColorArg1, dwOldColorArg2, dwOldTextureFactor;

    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwOldColorOp);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG1, &dwOldColorArg1);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG2, &dwOldColorArg2);
    m_pGraphicDev->GetRenderState(D3DRS_TEXTUREFACTOR, &dwOldTextureFactor);

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    if (m_pTextureCom)
    {
        m_pTextureCom->Render_Texture();
    }

    if (m_bSelected)
    {
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
        m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 255, 255, 100));  // 노란색 tint
    }

    m_pBufferCom->Render_Buffer();

    // 텍스처 스테이트 복원
    m_pGraphicDev->SetTexture(0, nullptr);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, dwOldColorOp);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, dwOldColorArg1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG2, dwOldColorArg2);
    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwOldTextureFactor);

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
}

HRESULT   CEditorDoor::Add_Component()
{
    Engine::CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = static_cast<Engine::CTransform*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[Engine::ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Buffer ( RcTex - Floor/Ceiling과 동일 )
    pComponent = m_pBufferCom = static_cast<Engine::CVIBuffer*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[Engine::ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = static_cast<Engine::CTexture*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Door_Texture"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[Engine::ID_DYNAMIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

void CEditorDoor::Set_DoorType(DOOR_TYPE iDoorType)
{
    m_eDoorType = iDoorType;

    if (m_pTextureCom)
    {
        m_pTextureCom->Change_Texture(m_eDoorType);
        m_pTextureCom->Set_Frame(_vec2(0, 0));
    }
}

CEditorDoor* CEditorDoor::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CEditorDoor* pInstance = new CEditorDoor(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorDoor Create Failed");
        return nullptr;
    }

    // 위치 설정
    pInstance->Set_Position(vPos);

    // 기본 크기 설정 (Door는 세로로 긴 형태)
    // 가로 8, 세로 16, 두께 1
    pInstance->Set_Scale(_vec3(16.f, 16.f, 1.f));

    // 기본 회전 (XY 평면 - 정면을 바라보도록)
    pInstance->Set_Rotation(_vec3(0.f, 0.f, 0.f));

    return pInstance;
}

CEditorDoor* CEditorDoor::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vRot, _vec3 vScale)
{
    CEditorDoor* pInstance = new CEditorDoor(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorDoor Create Failed");
        return nullptr;
    }

    // Transform 설정
    pInstance->Set_Position(vPos);
    pInstance->Set_Rotation(vRot);
    pInstance->Set_Scale(vScale);

    return pInstance;
}

CEditorDoor* CEditorDoor::Create(LPDIRECT3DDEVICE9 pGraphicDev,
                                _vec3 vPos, _vec3 vRot, _vec3 vScale, DOOR_TYPE iDoorType, _int iDoorID)
{
    CEditorDoor* pInstance = new CEditorDoor(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorDoor Create Failed");
        return nullptr;
    }

    // Transform 설정
    pInstance->Set_Position(vPos);
    pInstance->Set_Rotation(vRot);
    pInstance->Set_Scale(vScale);

    // Door 속성 설정
    pInstance->Set_DoorType(iDoorType);
    pInstance->Set_DoorID(iDoorID);

    return pInstance;
}

void CEditorDoor::Free()
{
    CEditorObject::Free();
}
