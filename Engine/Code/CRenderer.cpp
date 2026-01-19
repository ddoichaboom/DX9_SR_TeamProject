#include "CRenderer.h"
#include "CRenderStateGuard.h"
#include "CDInputMgr.h"

IMPLEMENT_SINGLETON(CRenderer)

CRenderer::CRenderer()
{
}

CRenderer::~CRenderer()
{
	Free();
}

void CRenderer::Add_RenderGroup(RENDERID eType, CGameObject* pGameObject)
{
	if (RENDER_END <= eType || nullptr == pGameObject)
		return;

	m_RenderGroup[eType].push_back(pGameObject);
	pGameObject->AddRef();
}

void CRenderer::Render_GameObject(LPDIRECT3DDEVICE9& pGraphicDev)
{
	pGraphicDev->SetTexture(0, nullptr);
	Render_Priority(pGraphicDev);
	Render_NonAlpha(pGraphicDev);
	Render_NonAlpha_WRAP(pGraphicDev);
	Render_NonAlpha_Quality(pGraphicDev);
	Render_Alpha(pGraphicDev);
	Render_Alpha_UI(pGraphicDev);
	Render_UI(pGraphicDev);

	if (CDInputMgr::GetInstance()->GetDebugState()) Render_DEBUG(pGraphicDev);

	Clear_RenderGroup();
}

void CRenderer::Clear_RenderGroup()
{
	for (size_t i = 0; i < RENDER_END; ++i)
	{
		for_each(m_RenderGroup[i].begin(), m_RenderGroup[i].end(), CDeleteObj());
		m_RenderGroup[i].clear();
	}
}

void CRenderer::Render_Priority(LPDIRECT3DDEVICE9& pGraphicDev)
{
	for (auto& pObj : m_RenderGroup[RENDER_PRIORITY])
		pObj->Render_GameObject();
}

void CRenderer::Render_NonAlpha(LPDIRECT3DDEVICE9& pGraphicDev)
{
	//알파테스트 
	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 200);

	for (auto& pObj : m_RenderGroup[RENDER_NONALPHA])
		pObj->Render_GameObject();
}

//WRAP용. UV가 1을 넘었을때 기존 이미지가 반복되어 나타남 
void CRenderer::Render_NonAlpha_WRAP(LPDIRECT3DDEVICE9& pGraphicDev)
{
	pGraphicDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	pGraphicDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	for (auto& pObj : m_RenderGroup[RENDER_NONALPHA_WRAP])
		pObj->Render_GameObject();

	pGraphicDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	pGraphicDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
}

//밉맵의 더 높은 해상도를 꺼내쓰게 함 
void CRenderer::Render_NonAlpha_Quality(LPDIRECT3DDEVICE9& pGraphicDev)
{
	float bias = -1.f;
	//좀 멀리 있어도 기본보다 더 높은 해상도의 밉맵을 사용하도록 
	pGraphicDev->SetSamplerState(0, D3DSAMP_MIPMAPLODBIAS, *((DWORD*)&bias));

	for (auto& pObj : m_RenderGroup[RENDER_NONALPHA_QUALITY])
		pObj->Render_GameObject();

	//여기서 알파테스트 끔 
	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	float defaultBias = 0.0f;
	pGraphicDev->SetSamplerState(0, D3DSAMP_MIPMAPLODBIAS, *((DWORD*)&defaultBias));

}

void CRenderer::Render_Alpha(LPDIRECT3DDEVICE9& pGraphicDev)
{
	CRenderStateGuard cGuard(pGraphicDev);
	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	
	pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_RenderGroup[RENDER_ALPHA].sort([](CGameObject* pDst, CGameObject* pSrc)->bool
		{
			return pDst->Get_ViewZ() > pSrc->Get_ViewZ();
		});

	for (auto& pObj : m_RenderGroup[RENDER_ALPHA])
		pObj->Render_GameObject();

}

void CRenderer::Render_Alpha_UI(LPDIRECT3DDEVICE9& pGraphicDev)
{
	CRenderStateGuard cGuard(pGraphicDev);
	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	_matrix mat, View;
	D3DXMatrixIdentity(&View);
	pGraphicDev->SetTransform(D3DTS_VIEW, &View);

	D3DXMatrixOrthoLH(&mat, WINCX, WINCY, 0.f, 1.f);
	pGraphicDev->SetTransform(D3DTS_PROJECTION, &mat);

	//UI는 Z값이 동일하므로 넣은 순으로 랜더
	for (auto& pObj : m_RenderGroup[RENDER_ALPHA_UI])
		pObj->Render_GameObject();

}

void CRenderer::Render_UI(LPDIRECT3DDEVICE9& pGraphicDev)
{
	CRenderStateGuard cGuard(pGraphicDev);

	//알파테스트 
	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 200);

	_matrix mat, View;
	D3DXMatrixIdentity(&View);
	pGraphicDev->SetTransform(D3DTS_VIEW, &View);

	D3DXMatrixOrthoLH(&mat, WINCX, WINCY, 0.f, 1.f);
	pGraphicDev->SetTransform(D3DTS_PROJECTION, &mat);
	
	for (auto& pObj : m_RenderGroup[RENDER_UI])
		pObj->Render_GameObject();

	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}

void CRenderer::Render_DEBUG(LPDIRECT3DDEVICE9& pGraphicDev)
{
	pGraphicDev->SetTexture(0, nullptr);

	//디퓨즈 색을 쓰기 
	pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);

	pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	for (auto& pObj : m_RenderGroup[RENDER_DEBUG])
		pObj->Render_GameObject();

	pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	//텍스쳐 색을 쓰기
	pGraphicDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	pGraphicDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

}


void CRenderer::Free()
{
	Clear_RenderGroup();
}
