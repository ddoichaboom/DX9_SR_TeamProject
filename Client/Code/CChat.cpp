#include "pch.h"
#include "CChat.h"
#include "CProtoMgr.h"
#include "CRenderer.h"

vector<TextureSource> CChat::m_vTextureSource =
{
	{ CHAT_IDLE,  L"../Bin/Resource/Texture/UI/UI_CHAT.dds" },
	
};

vector<AnimationSource>  CChat::m_vAnimSource =
{
	{ CHAT_IDLE,1,3,3, true, 0.12f},
	
};

CChat::CChat(LPDIRECT3DDEVICE9 pGraphicDev)
	: CBaseUI(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pAnimationCom(nullptr), m_pStateCom(nullptr)
{
	m_iOrder = 6;
}

CChat::CChat(const CChat& rhs)
	: CBaseUI(rhs)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pAnimationCom(nullptr), m_pStateCom(nullptr)
{
	m_iOrder = 6;
}

CChat::~CChat()
{
}

CChat* CChat::Create(PDIRECT3DDEVICE9 pGraphicDev)
{
	CChat* pChat = new CChat(pGraphicDev);

	if (FAILED(pChat->Ready_GameObject()))
	{
		Safe_Release(pChat);
		MSG_BOX("Chat Create Failed");
		return nullptr;
	}

	return pChat;
}

void CChat::CreateStateData()
{
	auto Mgr = CDataMgr<CChat>::GetInstance();
	if (Mgr->IsStateEmpty() == false) return;

	CState<CChat>* State = new CState<CChat>(&CChat::Begin_Idle, &CChat::Idle, nullptr);
	Mgr->AddState(CHAT_IDLE, State);
}

HRESULT CChat::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_ChatAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });
	return S_OK;
}

void CChat::Free()
{
	CDataMgr<CChat>::DestroyInstance();
	CBaseUI::Free();
}

HRESULT CChat::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CreateStateData();

	return S_OK;
}

_int CChat::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CBaseUI::Update_GameObject(fTimeDelta);
	return iExit;
}

void CChat::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CBaseUI::LateUpdate_GameObject(fTimeDelta);
	m_pAnimationCom->Update_State(m_pStateCom->GetCurrentStateID());
}

void CChat::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pAnimationCom->Render_Animation();
	m_pBufferCom->Render_Buffer();
}

void CChat::Rotate(ROTATION eType, const _float& fAngle)
{
	m_pTransformCom->Rotation(eType, fAngle);
}

void CChat::SetPos(_vec3 _pos)
{
	m_pTransformCom->Set_Pos(_pos.x - WINCX * 0.5f, -_pos.y + WINCY * 0.5f, 0.f);
}

void CChat::SetScale(_float fCX, _float fCY)
{
	m_pTransformCom->Set_Scale(fCX * 0.5f, fCY * 0.5f, 1.f);
}

void CChat::ChangeState(_uint nextStateID)
{
	//현재 상태에 맞는 애니메이션으로 자동 전환
	m_pAnimationCom->Update_State(nextStateID);
	m_pStateCom->ChangeState<CChat>(nextStateID);
}

void CChat::Begin_Idle()
{
	m_fSizeX = 256.f;
	m_fSizeY = 256.f;
	m_fX = WINCX - 128.f;
	m_fY = WINCY - 143.f;

	SetScale(m_fSizeX, m_fSizeY);
	SetPos({ m_fX, m_fY, 0.f });
}

void CChat::Idle()
{
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA_UI, this);
}
