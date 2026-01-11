#include "pch.h"
#include "CHeightMapTerrain.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

CHeightMapTerrain::CHeightMapTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CHeightMapTerrain::CHeightMapTerrain(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

CHeightMapTerrain::~CHeightMapTerrain()
{
}

HRESULT CHeightMapTerrain::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;


	m_pTransformCom->Set_Pos(-25.f, -12.5f, -15.f);

	return S_OK;
}

_int CHeightMapTerrain::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_PRIORITY, this);

	return iExit;
}

void CHeightMapTerrain::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CHeightMapTerrain::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pGraphicDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	_matrix mat;
	D3DXMatrixIdentity(&mat);
	m_pGraphicDev->SetTransform(D3DTS_TEXTURE0, &mat);
	//m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	if (FAILED(Ready_Material()))
		return;


	m_pTextureCom->Set_Texture(0);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);

	m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

}

HRESULT CHeightMapTerrain::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	// TerrainTex
	pComponent = m_pBufferCom = dynamic_cast<Engine::CTerrainTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_TerrainTex"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	// Texture
	pComponent = m_pTextureCom = dynamic_cast<Engine::CTerrainTexture*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_TerrainTexture"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });


	return S_OK;
}

HRESULT CHeightMapTerrain::Ready_Material()
{
	D3DMATERIAL9			tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tMtrl.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);

	tMtrl.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
	tMtrl.Power = 0.f;

	m_pGraphicDev->SetMaterial(&tMtrl);

	return S_OK;
}


CHeightMapTerrain* CHeightMapTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CHeightMapTerrain* pHeightMapTerrain = new CHeightMapTerrain(pGraphicDev);

	if (FAILED(pHeightMapTerrain->Ready_GameObject()))
	{
		Safe_Release(pHeightMapTerrain);
		MSG_BOX("pHeightMapTerrain Create Failed");
		return nullptr;
	}

	return pHeightMapTerrain;
}

void CHeightMapTerrain::Free()
{
	Safe_Release(m_pBufferCom);

	CGameObject::Free();
}
