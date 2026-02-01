#include "pch.h"
#include "CMinigun.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CPoolMgr.h"
#include "CSoundMgr.h"


#include "CRoadPlayer.h"
#include "CPannel.h"
#include "CChain.h"


vector<TextureSource> CMinigun::m_vTextureSource =
{
	 { MS_IDLE,			L"../Bin/Resource/Texture/Minigun/Minigun_Idle.dds" },
	 { MS_START,		L"../Bin/Resource/Texture/Minigun/Minigun_Start.dds" },
	 { MS_CYCLE,		L"../Bin/Resource/Texture/Minigun/Minigun_Cycle.dds" },
	 { MS_ATTACK,		L"../Bin/Resource/Texture/Minigun/Minigun_Attack.dds" },
	 { MS_ATTACK_END,	L"../Bin/Resource/Texture/Minigun/Minigun_End.dds" }
};
//Loop 인 애니메이션은 Ratio 세팅 금지(디폴트로 두기) . Ratio먹이면 다음 애니메이션이 안나옴 
vector<AnimationSource> CMinigun::m_vAnimSource =
{
	{ MS_IDLE,0,1,1,   true, 0.08f},
	{ MS_START,1,1,1,  false, 0.08f, 1.f},
	{ MS_CYCLE,1,1,1,  true, 0.04f},
	{ MS_ATTACK,1,1,1, true, 0.025f},
	{ MS_ATTACK_END,1,3,3, false, 0.05f, 1.f}
};

CMinigun::CMinigun(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pStateCom(nullptr), m_pAnimationCom(nullptr)
	, m_pParent(nullptr), m_pPannel(nullptr), m_pChain(nullptr)
	, m_fTime(0.f), m_bKeyPressing(false), m_bStageEnd(false)
{

}

CMinigun::CMinigun(const CMinigun& rhs)
	: CGameObject(rhs)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pStateCom(nullptr), m_pAnimationCom(nullptr)
	, m_pParent(nullptr), m_pPannel(nullptr), m_pChain(nullptr)
	, m_fTime(0.f), m_bKeyPressing(false), m_bStageEnd(false)
{
}

CMinigun::~CMinigun()
{
}

CMinigun* CMinigun::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMinigun* pMinigun = new CMinigun(pGraphicDev);

	if (FAILED(pMinigun->Ready_GameObject()))
	{
		Safe_Release(pMinigun);
		MSG_BOX("Minigun Create Failed");
		return nullptr;
	}

	return pMinigun;
}

void CMinigun::CreateStateData()
{
	auto Mgr = CDataMgr<CMinigun>::GetInstance();
	if (Mgr->IsStateEmpty() == false) return;

	CState<CMinigun>* State = new CState<CMinigun>(&CMinigun::Begin_Idle, &CMinigun::Idle, &CMinigun::End_Idle);
	Mgr->AddState(MS_IDLE, State);

	State = new CState<CMinigun>(&CMinigun::Begin_Start, &CMinigun::Start, &CMinigun::End_Start);
	Mgr->AddState(MS_START, State);

	State = new CState<CMinigun>(&CMinigun::Begin_Cycle, &CMinigun::Cycle, &CMinigun::End_Cycle);
	Mgr->AddState(MS_CYCLE, State);

	State = new CState<CMinigun>(&CMinigun::Begin_Attack, &CMinigun::Attack, &CMinigun::End_Attack);
	Mgr->AddState(MS_ATTACK, State);

	State = new CState<CMinigun>(&CMinigun::Begin_Attack_End, &CMinigun::Attack_End, &CMinigun::End_Attack_End);
	Mgr->AddState(MS_ATTACK_END, State);
}

void CMinigun::OnEvent(EVENT_TYPE _type, EventData* _pData)
{
	if (_type == EVENT_ENDING)
	{
		CSoundMgr::GetInstance()->StopGroupSound(SOUND_PLAYER_BGM);
		m_bStageEnd = true;
		m_bKeyPressing = false;		
	}
}

HRESULT CMinigun::Ready_GameObject()
{
	if (FAILED(Add_Component())) return E_FAIL;
	CreateStateData();
	
	m_vScale = { 450.f, 450.f, 1.f };
	m_vPosition = { WINCX * 0.5f, WINCY - 200.f, 0.f };


	SetPos(m_vPosition);
	SetScale(m_vScale);

	ChangeState(MS_IDLE);


	m_pPannel = CPannel::Create(m_pGraphicDev);
	if (m_pPannel == nullptr) return E_FAIL;
	
	
	m_pChain = CChain::Create(m_pGraphicDev);
	if (m_pChain == nullptr) return E_FAIL;

	CEventMgr::GetInstance()->Subscribe(EVENT_ENDING, this);
    return S_OK;
}

_int CMinigun::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bDead) return RET_DEAD;
	m_fTime += fTimeDelta;
	_int iExit = CGameObject::Update_GameObject(fTimeDelta);	
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);	
	m_pChain->Update_GameObject(fTimeDelta);
	m_pPannel->Update_GameObject(fTimeDelta);

	if(m_bStageEnd == false)
		Key_Input(fTimeDelta);

	return iExit;
}

void CMinigun::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pAnimationCom->Update_State(m_pStateCom->GetCurrentStateID());
	m_pChain->LateUpdate_GameObject(fTimeDelta);
	m_pPannel->LateUpdate_GameObject(fTimeDelta);
}

void CMinigun::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pAnimationCom->Render_Animation();
	m_pBufferCom->Render_Buffer();

	m_pChain->Render_GameObject();
	m_pPannel->Render_GameObject();
}

void CMinigun::Activate()
{
	CGameObject::Activate();
}

void CMinigun::Deactivate()
{
	CGameObject::Deactivate();
}

void CMinigun::ChangeState(_uint nextStateID)
{
	m_fTime = 0.f;
	m_pAnimationCom->Update_State(nextStateID);
	m_pStateCom->ChangeState<CMinigun>(nextStateID);
}

HRESULT CMinigun::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_MinigunAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });

    return S_OK;
}

void CMinigun::Free()
{
	Safe_Release(m_pChain);
	Safe_Release(m_pPannel);
	CDataMgr<CMinigun>::DestroyInstance();
	CGameObject::Free();
}

void CMinigun::Key_Input(const _float& fTimeDelta)
{
	if (CDInputMgr::GetInstance()->Mouse_Pressing(DIM_LB))
	{
		m_bKeyPressing = true;
	}
	else
	{
		m_bKeyPressing = false;
	}
}

void CMinigun::Begin_Idle()
{
	
}

void CMinigun::Idle()
{
	if (m_bKeyPressing)
	{
		ChangeState(MS_START);
		return;
	}
}

void CMinigun::End_Idle()
{
}

void CMinigun::Begin_Start()
{
	CSoundMgr::GetInstance()->PlayPlayerSound(m_szLoopStart.c_str(), 0.8f);
}

void CMinigun::Start()
{
	if (m_bKeyPressing == false)
	{
		CSoundMgr::GetInstance()->PlayPlayerSound(m_szFastEnd.c_str(), 0.8f);
		ChangeState(MS_ATTACK_END);

		return;
	}

	if (m_pAnimationCom->CanEnd())
	{
		ChangeState(MS_CYCLE);
		return;
	}
}

void CMinigun::End_Start()
{
}

void CMinigun::Begin_Cycle()
{
	CSoundMgr::GetInstance()->PlayPlayerBGMSound(m_szMinigunCycle.c_str(),0.8f);
}

void CMinigun::Cycle()
{
	if (m_bKeyPressing == false)
	{
		CSoundMgr::GetInstance()->StopGroupSound(SOUND_PLAYER_BGM);
		CSoundMgr::GetInstance()->PlayPlayerSound(m_szFastEnd.c_str(), 0.8f);
		ChangeState(MS_ATTACK_END);		
		return;
	}

	if (m_fTime > 0.5f)
	{
		ChangeState(MS_ATTACK);
		return;
	}
}

void CMinigun::End_Cycle()
{
}

void CMinigun::Begin_Attack()
{
	m_pChain->ChangeState(1);
	CSoundMgr::GetInstance()->StopGroupSound(SOUND_PLAYER_BGM);
//	CSoundMgr::GetInstance()->PlayPlayerBGMSound(m_szMinigunLoop.c_str(), 1.8f);
	CSoundMgr::GetInstance()->PlayPlayerBGMSound(m_szMinigunLoop.c_str(), 1.f);
}

void CMinigun::Attack()
{
	if (m_bKeyPressing == false)
	{
		CSoundMgr::GetInstance()->StopGroupSound(SOUND_PLAYER_BGM);
		ChangeState(MS_ATTACK_END);
		CSoundMgr::GetInstance()->PlayPlayerSound(m_szLoopEnd.c_str(), 0.8f);
		return;
	}

	if (m_fTime > 0.05f)
	{	
		m_fTime = 0.f;
		// 총알 생성
		m_pParent->Shoot();
		
		return;
	}
}

void CMinigun::End_Attack()
{

}

void CMinigun::Begin_Attack_End()
{
	m_pChain->ChangeState(0);
}

void CMinigun::Attack_End()
{
	if (m_pAnimationCom->CanEnd())
	{
		ChangeState(MS_IDLE);
		return;
	}
}

void CMinigun::End_Attack_End()
{
}

void CMinigun::Rotate(ROTATION eType, const _float& fAngle)
{
	m_pTransformCom->Rotation(eType, fAngle);
}

void CMinigun::SetPos(_vec3 _pos)
{
	m_pTransformCom->Set_Pos(_pos.x - WINCX * 0.5f, -_pos.y + WINCY * 0.5f, 0.f);
}

void CMinigun::SetScale(_vec3 _scale)
{
	m_pTransformCom->Set_Scale(_scale.x * 0.5f, _scale.y * 0.5f, 1.f);
}

void CMinigun::Set_Parent(CRoadPlayer* pPlayer)
{	
	m_pParent = pPlayer;
}
