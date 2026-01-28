#include "pch.h"
#include "CPannel.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CPoolMgr.h"
#include "CSoundMgr.h"

#include "CBullet.h"


vector<TextureSource> CPannel::m_vTextureSource =
{
	 { MS_IDLE,			L"../Bin/Resource/Texture/Minigun/Minigun_Panel.dds" }
};

vector<AnimationSource> CPannel::m_vAnimSource =
{
	{ MS_IDLE,1,0,0,   true, 0.08f},
};

CPannel::CPannel(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pStateCom(nullptr), m_pAnimationCom(nullptr)
	, m_fTime(0.f)
{

}

CPannel::CPannel(const CPannel& rhs)
	: CGameObject(rhs)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pStateCom(nullptr), m_pAnimationCom(nullptr)
	, m_fTime(0.f)
{
}

CPannel::~CPannel()
{
}

CPannel* CPannel::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPannel* pPannel = new CPannel(pGraphicDev);

	if (FAILED(pPannel->Ready_GameObject()))
	{
		Safe_Release(pPannel);
		MSG_BOX("Minigun Pannel Create Failed");
		return nullptr;
	}

	return pPannel;
}

void CPannel::CreateStateData()
{
	auto Mgr = CDataMgr<CPannel>::GetInstance();
	if (Mgr->IsStateEmpty() == false) return;

	CState<CPannel>* State = new CState<CPannel>(nullptr, &CPannel::Idle, nullptr);
	Mgr->AddState(MS_IDLE, State);

}

HRESULT CPannel::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	CreateStateData();

	m_vScale =	{ 700, 350.f, 1.f };
	m_vPosition = { WINCX * 0.5f, WINCY + 80.f, 0.f };


	SetPos(m_vPosition);
	SetScale(m_vScale);

	ChangeState(MS_IDLE);

	return S_OK;
}

_int CPannel::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bDead) return RET_DEAD;

	_int iExit = CGameObject::Update_GameObject(fTimeDelta);
	


	return iExit;
}

void CPannel::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pAnimationCom->Update_State(m_pStateCom->GetCurrentStateID());
}

void CPannel::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pAnimationCom->Render_Animation();
	m_pBufferCom->Render_Buffer();
}

void CPannel::Activate()
{
	CGameObject::Activate();
}

void CPannel::Deactivate()
{
	CGameObject::Deactivate();
}

void CPannel::ChangeState(_uint nextStateID)
{
	m_fTime = 0.f;
	m_pAnimationCom->Update_State(nextStateID);
	m_pStateCom->ChangeState<CPannel>(nextStateID);
}

HRESULT CPannel::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_PannelAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });

	return S_OK;
}

void CPannel::Free()
{
	CDataMgr<CPannel>::DestroyInstance();
	CGameObject::Free();
}

void CPannel::Idle()
{
}


void CPannel::Rotate(ROTATION eType, const _float& fAngle)
{
	m_pTransformCom->Rotation(eType, fAngle);
}

void CPannel::SetPos(_vec3 _pos)
{
	m_pTransformCom->Set_Pos(_pos.x - WINCX * 0.5f, -_pos.y + WINCY * 0.5f, 0.f);
}

void CPannel::SetScale(_vec3 _scale)
{
	m_pTransformCom->Set_Scale(_scale.x * 0.5f, _scale.y * 0.5f, 1.f);
}
