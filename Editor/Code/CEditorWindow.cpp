#include "pch.h"
#include "CEditorWindow.h"
#include "CRenderer.h"
#include "CProtoMgr.h"

vector<TextureSource> CEditorWindow::m_vTextureSource =
{
	{ 0, L"../Bin/Resource/Texture/Object/WINDOW.dds"}
};

CEditorWindow::CEditorWindow(LPDIRECT3DDEVICE9 pGraphicDev)
	: CEditorObject(pGraphicDev)
	, m_iTextureIdx(0)
{
}

CEditorWindow::~CEditorWindow()
{
}

HRESULT CEditorWindow::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

    if (!m_pTextureCom)
        return E_FAIL;

    m_pTextureCom->Change_Texture(0);

    if (!m_pTransformCom)
        return E_FAIL;

    m_pTransformCom->Set_Scale(_vec3(16.f, 16.f, 1.f));

    m_wstrName = L"Window";

	return S_OK;
}

_int CEditorWindow::Update_GameObject(const _float& fTimeDelta)
{
    CEditorObject::Update_GameObject(fTimeDelta);

    // 렌더러에 등록 (알파 렌더링)
    Engine::CRenderer::GetInstance()->Add_RenderGroup(Engine::RENDER_ALPHA, this);

    return 0;
}

void CEditorWindow::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CEditorObject::LateUpdate_GameObject(fTimeDelta);
}

void CEditorWindow::Render_GameObject()
{
    DWORD dwOldColorOp, dwOldColorArg1, dwOldColorArg2, dwOldTextureFactor, dOldTTFF;

    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwOldColorOp);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG1, &dwOldColorArg1);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_COLORARG2, &dwOldColorArg2);
    m_pGraphicDev->GetRenderState(D3DRS_TEXTUREFACTOR, &dwOldTextureFactor);

    m_pGraphicDev->GetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, &dOldTTFF);

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

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

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, dOldTTFF);
}


HRESULT CEditorWindow::Add_Component()
{
    CComponent* pComponent = nullptr;

    // 1. RcTex 버퍼 (2D 사각형)
    pComponent = m_pBufferCom = static_cast<Engine::CRcTex*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));
    NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
    m_mapComponent[Engine::ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // 2. Transform
    pComponent = m_pTransformCom = static_cast<Engine::CTransform*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));
    NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);
    m_mapComponent[Engine::ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // 3. Texture (Window_Idle)
    pComponent = m_pTextureCom = static_cast<Engine::CTexture*>(
        Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_WindowTexture"));
    NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
    m_mapComponent[Engine::ID_DYNAMIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

CEditorWindow* CEditorWindow::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CEditorWindow* pInstance = new CEditorWindow(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorWindow Create Failed");
        return nullptr;
    }

    pInstance->Set_Position(vPos);

    pInstance->Set_Scale(_vec3(16.f, 16.f, 1.f));

    return pInstance;
}

CEditorWindow* CEditorWindow::Create(LPDIRECT3DDEVICE9 pGraphicDev,
						_vec3 vPos, _vec3 vRot, _vec3 vScale)
{
    CEditorWindow* pInstance = new CEditorWindow(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEditorWindow Create Failed");
        return nullptr;
    }

    pInstance->Set_Position(vPos);
    pInstance->Set_Rotation(vRot);
    pInstance->Set_Scale(vScale);

    return pInstance;
}

void CEditorWindow::Free()
{
	CEditorObject::Free();
}
