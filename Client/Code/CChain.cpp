#include "pch.h"
#include "CChain.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CPoolMgr.h"
#include "CSoundMgr.h"

#include "CBullet.h"


vector<TextureSource> CChain::m_vTextureSource =
{
	 { MS_IDLE,		L"../Bin/Resource/Texture/Minigun/Minigun_Chain_Idle.dds" },
	 { MS_ATTACK,	L"../Bin/Resource/Texture/Minigun/Minigun_Chain_Attack.dds" }
};

vector<AnimationSource> CChain::m_vAnimSource =
{
	{ MS_IDLE,1,1,1,true, 0.08f},
	{ MS_ATTACK,1,1,0,true, 0.03f}
};

CChain::CChain(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pStateCom(nullptr), m_pAnimationCom(nullptr)
	, m_fTime(0.f)
{

}

CChain::CChain(const CChain& rhs)
	: CGameObject(rhs)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pStateCom(nullptr), m_pAnimationCom(nullptr)
	, m_fTime(0.f)
{
}

CChain::~CChain()
{
}

CChain* CChain::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CChain* pPannel = new CChain(pGraphicDev);

	if (FAILED(pPannel->Ready_GameObject()))
	{
		Safe_Release(pPannel);
		MSG_BOX("Minigun Pannel Create Failed");
		return nullptr;
	}

	return pPannel;
}

void CChain::CreateStateData()
{
	auto Mgr = CDataMgr<CChain>::GetInstance();
	if (Mgr->IsStateEmpty() == false) return;

	CState<CChain>* State = new CState<CChain>(nullptr, &CChain::Idle, nullptr);
	Mgr->AddState(MS_IDLE, State);

	State = new CState<CChain>(nullptr, &CChain::Attack, nullptr);
	Mgr->AddState(MS_ATTACK, State);

}

HRESULT CChain::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	CreateStateData();

	m_vScale = { 500.f, 250.f, 1.f };
	m_vPosition = { WINCX - 350.f, WINCY, 0.f };


	SetPos(m_vPosition);
	SetScale(m_vScale);

	ChangeState(MS_IDLE);

	return S_OK;
}

_int CChain::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bDead) return RET_DEAD;

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);	


	return iExit;
}

void CChain::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pAnimationCom->Update_State(m_pStateCom->GetCurrentStateID());
}

void CChain::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pAnimationCom->Render_Animation();
	m_pBufferCom->Render_Buffer();
}

void CChain::Activate()
{
	CGameObject::Activate();
}

void CChain::Deactivate()
{
	CGameObject::Deactivate();
}

void CChain::ChangeState(_uint nextStateID)
{
	m_fTime = 0.f;
	m_pAnimationCom->Update_State(nextStateID);
	m_pStateCom->ChangeState<CChain>(nextStateID);
}

HRESULT CChain::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	//VIBuffer
	pComponent = m_pBufferCom = static_cast<Engine::CRcTex*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	// Transform
	pComponent = m_pTransformCom = static_cast<Engine::CTransform*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pStateCom = static_cast<Engine::CStateComponent*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_StateComponent"));

	//Owner 지정해주기!! 
	m_pStateCom->SetOnwer(this);

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_StateComponent", pComponent });

	//Animation
	pComponent = m_pAnimationCom = static_cast<Engine::CAnimation*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_ChainAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });

	return S_OK;
}

void CChain::Free()
{
	CDataMgr<CChain>::DestroyInstance();
	CGameObject::Free();
}

void CChain::Idle()
{
}

void CChain::Attack()
{
}


void CChain::Rotate(ROTATION eType, const _float& fAngle)
{
	m_pTransformCom->Rotation(eType, fAngle);
}

void CChain::SetPos(_vec3 _pos)
{
	m_pTransformCom->Set_Pos(_pos.x - WINCX * 0.5f, -_pos.y + WINCY * 0.5f, 0.f);
}

void CChain::SetScale(_vec3 _scale)
{
	m_pTransformCom->Set_Scale(_scale.x * 0.5f, _scale.y * 0.5f, 1.f);
}
