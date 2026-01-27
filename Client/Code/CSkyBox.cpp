#include "pch.h"
#include "CSkyBox.h"
#include "CProtoMgr.h"
#include "CManagement.h"
#include "CRenderer.h"

vector<TextureSource> CSkyBox::m_vTextureSource =
{
	{0, L"../Bin/Resource/Texture/Terrain/SkyBox/SKY_BOX.dds", false, 0, 0, 0, {1.f, 1.f}}
};

CSkyBox::CSkyBox(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pTransformCom(nullptr)
	, m_pBufferCom(nullptr)
	, m_pTextureCom(nullptr)
{
}

CSkyBox::CSkyBox(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CSkyBox::~CSkyBox()
{
}

HRESULT CSkyBox::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	if (m_pTextureCom)
		m_pTextureCom->Change_Texture(0);

	// Far Plane 보다 작으면 됨 ( Far Plane > SkyBox Scale * 2)
	m_pTransformCom->Set_Scale(_vec3(300.f, 300.f, 300.f));

	return S_OK;
}

_int CSkyBox::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_PRIORITY, this);

	return iExit;
}

void CSkyBox::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);

	_matrix		matCamWorld;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matCamWorld);

	D3DXMatrixInverse(&matCamWorld, 0, &matCamWorld);


	m_pTransformCom->Set_Pos(matCamWorld._41, matCamWorld._42 + 3.f, matCamWorld._43);
}

void CSkyBox::Render_GameObject()
{
	DWORD dOldCullMode, dOldTTF;
	m_pGraphicDev->GetRenderState(D3DRS_CULLMODE, &dOldCullMode);
	m_pGraphicDev->GetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, &dOldTTF);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3);

	if (m_pTextureCom)
		m_pTextureCom->Render_Texture();	

	m_pBufferCom->Render_Buffer();

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, dOldTTF);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, dOldCullMode);
	m_pGraphicDev->SetTexture(0, nullptr);
}

HRESULT CSkyBox::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// CubeTex
	pComponent = m_pBufferCom = static_cast<Engine::CCubeTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CubeTex"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom = static_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	// Texture
	pComponent = m_pTextureCom = static_cast<Engine::CCubeTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SkyTexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });


	return S_OK;
}



CSkyBox* CSkyBox::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSkyBox* pPlayer = new CSkyBox(pGraphicDev);

	if (FAILED(pPlayer->Ready_GameObject()))
	{
		Safe_Release(pPlayer);
		MSG_BOX("pPlayer Create Failed");
		return nullptr;
	}

	return pPlayer;
}

void CSkyBox::Free()
{
	//Safe_Release(m_pBufferCom);

	CGameObject::Free();
}
