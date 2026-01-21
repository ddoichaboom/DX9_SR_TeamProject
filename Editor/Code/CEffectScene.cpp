#include "pch.h"
#include "CEffectScene.h"
#include "CDInputMgr.h"
#include "CEditorCamera.h"
#include "CToolBar.h"
#include "CHierarchy.h"
#include "CBlood.h"
#include "CTrail.h"
#include "CEditorFloor.h"
#include "CFlare.h"
#include "CExplosion.h"
#include "CBeamFlare.h"
#include "CBodyEmit.h"
#include "CHitUI.h"


CEffectScene::CEffectScene(LPDIRECT3DDEVICE9 pGraphicDev)
	: CEditorScene(pGraphicDev), m_pCurParticle(nullptr)
{
	m_pGraphicDev->AddRef();
}

CEffectScene::~CEffectScene()
{
}

HRESULT CEffectScene::Ready_Scene()
{
    m_pCamera = CEditorCamera::Create(m_pGraphicDev);
    if (nullptr == m_pCamera)
    {
        MSG_BOX("EditorCamera Create Failed");
        return E_FAIL;
    }

    m_pCamera->Set_Position(_vec3(0.f, 25.f, -30.f));
    m_pCamera->Set_LookAt(_vec3(0.f, -1.f, 1.f));

    _vec3 pos = { 0,0,0 };
    m_mapParticle[EF_BLOOD] = CBlood::Create(m_pGraphicDev, &pos, 1);
    m_mapParticle[EF_TRAIL] = CTrail::Create(m_pGraphicDev);
    m_mapParticle[EF_FLARE] = CFlare::Create(m_pGraphicDev);
    m_mapParticle[EF_EXP] = CExplosion::Create(m_pGraphicDev);
    m_mapParticle[EF_BEAM_FLARE] = CBeamFlare::Create(m_pGraphicDev);
    m_mapParticle[EF_BODY] = CBodyEmit::Create(m_pGraphicDev);
    m_mapParticle[EF_HITUI] = CHitUI::Create(m_pGraphicDev);

    m_pCurParticle = m_mapParticle[EF_HITUI];

    m_pFloor = CEditorFloor::Create(m_pGraphicDev, { 0,0,0 });
    return S_OK;
}

_int CEffectScene::Update_Scene(const _float& fTimeDelta)
{
    if (m_pCamera)
        m_pCamera->Update_GameObject(fTimeDelta);

    if (m_pCurParticle) m_pCurParticle->Update_GameObject(fTimeDelta);
    if (m_pFloor) m_pFloor->Update_GameObject(fTimeDelta);
    return 0;
}

void CEffectScene::LateUpdate_Scene(const _float& fTimeDelta)
{
}

void CEffectScene::Render_Scene()
{
    if (nullptr == m_pCamera)
        return;
    _matrix matView, matProj;
    m_pCamera->Get_ViewMatrix(&matView);
    m_pCamera->Get_ProjMatrix(&matProj);

    m_pGraphicDev->SetTransform(D3DTS_VIEW, &matView);
    m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);


}

void CEffectScene::SetEmitter(EFFECT_TYPE _type)
{
    if (m_mapParticle.find(_type) == m_mapParticle.end()) return;
    m_pCurParticle = m_mapParticle[_type];
    m_pCurParticle->Reset();
}

CParticleEmitter* CEffectScene::GetEmitter()
{
    return m_pCurParticle;
}

CEffectScene* CEffectScene::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CEffectScene* pInstance = new CEffectScene(pGraphicDev);

    if (FAILED(pInstance->Ready_Scene()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CEffectScene Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CEffectScene::Free()
{
    for (auto iter = m_mapParticle.begin(); iter != m_mapParticle.end(); iter++)
    {
        Safe_Release((*iter).second);
    }
    Safe_Release(m_pFloor);
    Safe_Release(m_pCamera);
    Safe_Release(m_pGraphicDev);

    CScene::Free();
}
