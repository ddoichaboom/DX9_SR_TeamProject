#include "pch.h"
#include "CPhonePlayer.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

vector<TextureSource> CPhonePlayer::m_vTextureSource =
{
	{ PHONE_IDLE,  L"../Bin/Resource/Texture/UI/PHONE_Player.dds" },

};

vector<AnimationSource>  CPhonePlayer::m_vAnimSource =
{
	{ PHONE_IDLE,2,3,3, true, 0.4f},

};

CPhonePlayer::CPhonePlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CBaseUI(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pAnimationCom(nullptr), m_pStateCom(nullptr)
{
	m_iOrder = 1;
}

CPhonePlayer::CPhonePlayer(const CPhonePlayer& rhs)
	: CBaseUI(rhs)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pAnimationCom(nullptr), m_pStateCom(nullptr)
{
	m_iOrder = 1;
}

CPhonePlayer::~CPhonePlayer()
{
}

CPhonePlayer* CPhonePlayer::Create(PDIRECT3DDEVICE9 pGraphicDev)
{
	CPhonePlayer* pPlayer = new CPhonePlayer(pGraphicDev);

	if (FAILED(pPlayer->Ready_GameObject()))
	{
		Safe_Release(pPlayer);
		MSG_BOX("Chat Create Failed");
		return nullptr;
	}

	return pPlayer;
}


void CPhonePlayer::CreateStateData()
{
	auto Mgr = CDataMgr<CPhonePlayer>::GetInstance();
	if (Mgr->IsStateEmpty() == false) return;

	CState<CPhonePlayer>* State = new CState<CPhonePlayer>(&CPhonePlayer::Begin_Idle, &CPhonePlayer::Idle, nullptr);
	Mgr->AddState(PHONE_IDLE, State);
}

HRESULT CPhonePlayer::Add_Component()
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

	//Animation
	pComponent = m_pAnimationCom = dynamic_cast<Engine::CAnimation*>
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_PhonePlayerAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });
	return S_OK;
}

void CPhonePlayer::Free()
{
	CDataMgr<CPhonePlayer>::DestroyInstance();
	CBaseUI::Free();
}

HRESULT CPhonePlayer::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CreateStateData();

	
	m_fX = 310.f;
	m_fY = WINCY - 240.f;
	m_fSizeX = 70.f;
	m_fSizeY = 70.f;

	//Rotate(ROT_Z, 15.f);
	SetScale(m_fSizeX, m_fSizeY);
	SetPos({ m_fX, m_fY, 0.f });

	ChangeState(PHONE_IDLE);

	return S_OK;
}

_int CPhonePlayer::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CBaseUI::Update_GameObject(fTimeDelta);
	return iExit;
}

void CPhonePlayer::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CBaseUI::LateUpdate_GameObject(fTimeDelta);
	m_pAnimationCom->Update_State(m_pStateCom->GetCurrentStateID());
}

void CPhonePlayer::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pAnimationCom->Render_Animation();
	m_pBufferCom->Render_Buffer();
}

void CPhonePlayer::Rotate(ROTATION eType, const _float& fAngle)
{
	m_pTransformCom->Rotation(eType, fAngle);
}

void CPhonePlayer::SetPos(_vec3 _pos)
{
	m_pTransformCom->Set_Pos(_pos.x - WINCX * 0.5f, -_pos.y + WINCY * 0.5f, 0.f);
}

void CPhonePlayer::SetScale(_float fCX, _float fCY)
{
	m_pTransformCom->Set_Scale(fCX * 0.5f, fCY * 0.5f, 1.f);
}

void CPhonePlayer::ChangeState(_uint nextStateID)
{
	//현재 상태에 맞는 애니메이션으로 자동 전환
	m_pAnimationCom->Update_State(nextStateID);
	m_pStateCom->ChangeState<CPhonePlayer>(nextStateID);
}

void CPhonePlayer::Begin_Idle()
{

}

void CPhonePlayer::Idle()
{
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
}
