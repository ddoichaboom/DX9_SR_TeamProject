#include "pch.h"
#include "CKatana.h"
#include "CAnimation.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"
#include "CPlayerPart.h"


#include "CPlayer.h"
#include "CTrail.h"
#include "CManagement.h"
#include "CPoolMgr.h"


vector<TextureSource> CKatana::m_vTextureSource =
{
	{ INTRO, L"../Bin/Resource/Texture/Player/Katana_Idle.dds" },
	{ IDLE, L"../Bin/Resource/Texture/Player/Katana_Idle.dds" },
	{GetStateID(ATTACK, COMBO_1),L"../Bin/Resource/Texture/Player/Katana_Idle.dds" },
	{GetStateID(ATTACK, COMBO_2),L"../Bin/Resource/Texture/Player/Katana_Idle.dds" },
	{GetStateID(ATTACK, COMBO_3),L"../Bin/Resource/Texture/Player/Katana_Idle.dds" }
	
};

vector<AnimationSource>  CKatana::m_vAnimSource =
{	
	{ INTRO,1,0,0, true, 0.11f},
	{ IDLE,1,0,0, true, 0.11f},		
	{ GetStateID(ATTACK, COMBO_1),1,0,0, true, 0.11f},
	{ GetStateID(ATTACK, COMBO_2),1,0,0, true, 0.11f},
	{ GetStateID(ATTACK, COMBO_3),1,0,0, true, 0.11f},
};


CKatana::CKatana(LPDIRECT3DDEVICE9 pGraphicDev)
	: CWeapon(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pAnimationCom(nullptr), m_pStateCom(nullptr)
	, m_fAniTime(0.f), m_fDelayTime(0.f), m_bDelay(false)
	, m_vStartPos(), m_vEndPos(), m_vConvertScale()
	, m_eCombo(COMBO_NONE), m_bCanCombo(false), m_bComboBuffered(false), m_fAniSpeed(1.f)
{
	m_iNowBullet = 1;
}

CKatana::CKatana(const CKatana& rhs)
	: CWeapon(rhs)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pAnimationCom(nullptr), m_pStateCom(nullptr)
	, m_fAniTime(0.f), m_fDelayTime(0.f), m_bDelay(false)
	, m_vStartPos(), m_vEndPos(), m_vConvertScale()
	, m_eCombo(COMBO_NONE), m_bCanCombo(false), m_bComboBuffered(false), m_fAniSpeed(1.f)
{
	m_iNowBullet = 1;
}

CKatana::~CKatana()
{
}

HRESULT CKatana::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CreateStateData();
	ChangeState(GetStateID(IDLE, WEAPON_KATANA));
	m_fCoolTime = 0.5f;
	m_fPower = 15.f;
	Begin_Intro();

	return S_OK;
}

_int CKatana::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CGameObject::Update_GameObject(fTimeDelta);

	m_fTime += fTimeDelta;
	m_fAniTime += fTimeDelta;
	if (m_bDelay)
		m_fDelayTime += fTimeDelta;

	return iExit;
}

void CKatana::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
	m_pAnimationCom->Update_State(m_pStateCom->GetCurrentStateID());
}

void CKatana::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pAnimationCom->Render_Animation();
	m_pBufferCom->Render_Buffer();
}

void CKatana::ChangeState(_uint nextStateID)
{
	m_fAniTime = 0.f;
	
	//현재 상태에 맞는 애니메이션으로 자동 전환
	m_pAnimationCom->Update_State(nextStateID);
	m_pStateCom->ChangeState<CKatana>(nextStateID);
}

_bool CKatana::Can_Fire()
{
	return !m_bComboBuffered;
}

void CKatana::Fire()
{
	m_fTime = 0.f;

	if (m_eCombo == COMBO_NONE)
	{
		Start_Combo();
		return;
	}
		

	if (m_bCanCombo)
	{
		Start_Combo();
	}
	else
	{
		m_bComboBuffered = true;
	}	
}

void CKatana::Reload()
{
	m_fTime = 0.f;
	m_bShootAble = true;
}

CKatana* CKatana::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CKatana* pWeapon = new CKatana(pGraphicDev);

	if (FAILED(pWeapon->Ready_GameObject()))
	{
		Safe_Release(pWeapon);
		MSG_BOX("Katana Create Failed");
		return nullptr;
	}

	return pWeapon;
}

HRESULT CKatana::Add_Component()
{
	Engine::CComponent* pComponent = nullptr;

	//VIBuffer
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

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	//StateComponent
	pComponent = m_pStateCom = dynamic_cast<Engine::CStateComponent*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_StateComponent"));

	m_pStateCom->SetOnwer(this);
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_StateComponent", pComponent });

	//AnimationComponent
	pComponent = m_pAnimationCom = dynamic_cast<Engine::CAnimation*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_KatanaAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });
	return S_OK;
}

void CKatana::Free()
{
	CWeapon::Free();
}

void CKatana::CreateStateData()
{
	auto Mgr = CDataMgr<CKatana>::GetInstance();
	if (Mgr->IsStateEmpty() == false) return;

	CState<CKatana>* State = new CState<CKatana>(&CKatana::Begin_Idle, &CKatana::Idle, nullptr);
	Mgr->AddState(IDLE, State);

	State = new CState<CKatana>(&CKatana::Begin_Intro, &CKatana::Intro, &CKatana::End_Intro);
	Mgr->AddState(INTRO, State);

	State = new CState<CKatana>(&CKatana::Begin_Attack1, &CKatana::Attack, &CKatana::End_Attack);
	Mgr->AddState(GetStateID(ATTACK, COMBO_1), State);

	State = new CState<CKatana>(&CKatana::Begin_Attack2, &CKatana::Attack, &CKatana::End_Attack);
	Mgr->AddState(GetStateID(ATTACK, COMBO_2), State);

	State = new CState<CKatana>(&CKatana::Begin_Attack3, &CKatana::Attack, &CKatana::End_Attack);
	Mgr->AddState(GetStateID(ATTACK, COMBO_3), State);
}

void CKatana::Begin_Idle()
{
	m_vStartPos = { 1050.f, WINCY - 365.f, 0.f };
	m_vConvertScale = { 1024.f, 128.f, 1.f };
	m_pTransformCom->Set_Angle(0, 0, -90);
	m_pTransformCom->Set_Scale(m_vConvertScale * 0.5f);
	m_pTransformCom->Set_Pos(m_vStartPos.x - WINCX * 0.5f, -m_vStartPos.y + WINCY * 0.5f, 0.f);
}

void CKatana::Idle()
{
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
}

void CKatana::Begin_Intro()
{
	m_vStartPos =	{ 200.f, WINCY - 275.f, 0.f };
	m_vEndPos =		{ 1300.f, WINCY - 275.f, 0.f };
	
	m_vConvertScale = { 1600.f, 256.f, 1.f };
	m_pTransformCom->Set_Angle(0, 0, 0);
	m_pTransformCom->Set_Scale(m_vConvertScale * 0.5f);
	m_pTransformCom->Set_Pos(m_vStartPos.x - WINCX * 0.5f, -m_vStartPos.y + WINCY * 0.5f, 0.f);

	m_fAniTime = 0.f;
	m_fDelayTime = 0.f;	
	m_fAniSpeed = 1.5f;
	m_bDelay = true;
}

void CKatana::Intro()
{
	_vec3 vPos;
	_float fTime;

	if (m_fDelayTime < 1.f)
	{
		CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
		return;
	}
	else if (m_bDelay)
	{
		m_fAniTime = 0.f;
		m_bDelay = false;
	}
									
	fTime = m_fAniTime * m_fAniSpeed;
	D3DXVec3Lerp(&vPos, &m_vStartPos, &m_vEndPos, fTime);

	m_pTransformCom->Set_Pos(vPos.x - WINCX * 0.5f, -vPos.y + WINCY * 0.5f, 0.f);
	if (fTime > 1.f)
	{
		m_pTransformCom->Set_Pos(m_vEndPos.x - WINCX * 0.5f, -m_vEndPos.y + WINCY * 0.5f, 0.f);		
		ChangeState(IDLE);
	}
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
}

void CKatana::End_Intro()
{
	m_vStartPos = { 400.f, WINCY - 200.f, 0.f };
	m_vEndPos = { -700.f, WINCY - 200.f, 0.f };
	m_fDelayTime = 0.f;
	m_bDelay = false;
}

void CKatana::Begin_Attack1()
{
	m_bComboBuffered = false;
	m_bCanCombo = false;

	m_vStartPos = { 800.f, WINCY - 275.f, 0.f };
	m_vEndPos = { 3000.f, WINCY - 275.f, 0.f };

	m_vConvertScale = { 3200.f, 200.f, 1.f };
	m_pTransformCom->Set_Angle(0.f, 0.f, 0.f);
	m_pTransformCom->Set_Scale(m_vConvertScale * 0.5f);
	

	m_fAniTime = 0.f;
	m_fDelayTime = 0.f;
	m_pTransformCom->Set_Pos(m_vStartPos.x - WINCX * 0.5f, -m_vStartPos.y + WINCY * 0.5f, 0.f);
	m_fAniSpeed = 6.f;

	if (!m_pTrail)
	{
		m_pTrail = CPoolMgr::GetInstance()->Get_Object<CTrail>();
		CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer")->Add_GameObject(m_pTrail);
	}
	m_pTrail->SetSize({ 80.f,3000.f });
}

void CKatana::Begin_Attack2()
{
	m_bComboBuffered = false;
	m_bCanCombo = false;

	m_vStartPos = { 0.f, 0.f, 0.f };
	m_vEndPos = { 3000.f, 3000.f, 0.f };

	m_vConvertScale = { 3200.f, 200.f, 1.f };
	m_pTransformCom->Set_Angle(0, 0, -45.f);
	m_pTransformCom->Set_Scale(m_vConvertScale * 0.5f);
	

	m_fAniTime = 0.f;
	m_fDelayTime = 0.f;
	m_pTransformCom->Set_Pos(m_vStartPos.x - WINCX * 0.5f, -m_vStartPos.y + WINCY * 0.5f, 0.f);
	m_fAniSpeed = 6.f;

	if (!m_pTrail)
	{
		m_pTrail = CPoolMgr::GetInstance()->Get_Object<CTrail>();
		CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer")->Add_GameObject(m_pTrail);
	}
	m_pTrail->SetSize({ 80.f,4000.f });


}

void CKatana::Begin_Attack3()
{
	m_bComboBuffered = false;
	m_bCanCombo = false;

	m_vStartPos = { WINCX - 400.f, 400.f, 0.f };
	m_vEndPos = { WINCX - 3000.f, 1800.f, 0.f };

	m_vConvertScale = { 3200.f, 200.f, 1.f };
	m_pTransformCom->Set_Angle(0, 0, -150.f);
	m_pTransformCom->Set_Scale(m_vConvertScale * 0.5f);
	

	m_fAniTime = 0.f;
	m_fDelayTime = 0.f;
	m_pTransformCom->Set_Pos(m_vStartPos.x - WINCX * 0.5f, -m_vStartPos.y + WINCY * 0.5f, 0.f);
	m_fAniSpeed = 6.f;

	if (!m_pTrail)
	{
		m_pTrail = CPoolMgr::GetInstance()->Get_Object<CTrail>();
		CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer")->Add_GameObject(m_pTrail);
	}

	m_pTrail->SetSize({ 80.f,3000.f });
}

void CKatana::Attack()
{
	_vec3 vPos;
	
	_float fTime = m_fAniTime * m_fAniSpeed;

	if (fTime > 0.7f && fTime < 1.0f)
	{
		m_bCanCombo = true;
	}

		
	if (fTime >= 1.f)
	{
		if (m_bComboBuffered)
		{
			m_bComboBuffered = false;
			m_pParentPart->Get_Player()->Katana_Func();
			m_pParentPart->Get_Player()->Change_State(IDLE);			
			Start_Combo();
			return;
		}
		else
		{
			m_pTrail->SetDead();
			m_pTrail = nullptr;
			//m_eCombo = COMBO_NONE;
			ChangeState(IDLE);
			m_pParentPart->Get_Player()->Katana_Func();
			m_pParentPart->Get_Player()->Change_State(IDLE);
		}
		return;
	}

	D3DXVec3Lerp(&vPos, &m_vStartPos, &m_vEndPos, fTime);
	m_pTransformCom->Set_Pos(vPos.x - WINCX * 0.5f, -vPos.y + WINCY * 0.5f, 0.f);
	
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	_vec3 start = { m_vStartPos.x - WINCX * 0.5f, -m_vStartPos.y + WINCY * 0.5f, 0.f };
	_vec3 end = { vPos.x - WINCX * 0.5f, -vPos.y + WINCY * 0.5f, 0.f };
	if (m_eCombo == COMBO_3)
	{
		start.x -= m_pTrail->GetSize().x;
		end.x -= m_pTrail->GetSize().x;
	}
	m_pTrail->SetTrailPos(start, end);
	m_pTrail->Reset();

}

void CKatana::End_Attack()
{
}

void CKatana::Start_Combo()
{
	if (m_eCombo == COMBO_NONE)
		m_eCombo = COMBO_1;
	else if (m_eCombo == COMBO_1)
		m_eCombo = COMBO_2;
	else if (m_eCombo == COMBO_2)
		m_eCombo = COMBO_3;
	else
		m_eCombo = COMBO_1;


	ChangeState(GetStateID(ATTACK, m_eCombo));
}
