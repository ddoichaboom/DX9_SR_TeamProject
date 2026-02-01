#include "pch.h"
#include "CUIManager.h"
#include "CProtoMgr.h"
#include "CPoolMgr.h"

#include "CPhoneBG.h"
#include "CShopBG.h"
#include "CShopItem.h"
#include "CSelectBG.h"
#include "CStageBG.h"
#include "CChat.h"
#include "CMascot.h"
#include "CHeart.h"
#include "CHeartBeat.h"
#include "CNoise.h"
#include "CPhonePlayer.h"
#include "CHPUI.h"
#include "CPlusUI.h"
#include "CTextBG.h"
#include "CTextUI.h"
#include "CEffectUI.h"
#include "CCursor.h"
#include "CDashUI.h"
#include "CTakeDown.h"
#include "CEventMgr.h"
#include "CSlotUI.h"
#include "CHudUI.h"
#include "CInfoUI.h"
#include "CTargetUI.h"

IMPLEMENT_SINGLETON(CUIManager)

CUIManager::CUIManager()
	: m_eNowState(UI_DEFAULT)
	, m_pEffectUI(nullptr), m_bRenderEffectUI(false)
	, m_pDashUI(nullptr),m_bDash(false)
	, m_pSlotUI(nullptr), m_bSlot(false)
	, m_pShopUI(nullptr), m_bShop(false)
	, m_iTargetIndex(-1), m_iTargetCount(0), m_bSnipermap(false)
{	
	
}

CUIManager::~CUIManager()
{
	Free();
}

void CUIManager::Free()
{	
	for (auto& pair : m_mapUI)
	{		
		for_each(pair.second.begin(), pair.second.end(), [](auto* pUI) { Safe_Release(pUI);});

		pair.second.clear();
	}
	
	m_mapUI.clear();

	for_each(m_vTargetUI.begin(), m_vTargetUI.end(), CDeleteObj());
	m_vTargetUI.clear();

	Safe_Release(m_pEffectUI);	
	Safe_Release(m_pDashUI);
	Safe_Release(m_pSlotUI);
	Safe_Release(m_pShopUI);
}

HRESULT CUIManager::Ready_GameObject(LPDIRECT3DDEVICE9 pGraphicDev)
{
	if (FAILED(Add_ProtoType(pGraphicDev)))
		return E_FAIL;


	if (FAILED(Add_UI(pGraphicDev)))
		return E_FAIL;

	m_pEffectUI = CEffectUI::Create(pGraphicDev);
	if (m_pEffectUI == nullptr)
		return E_FAIL;

	m_pDashUI = CDashUI::Create(pGraphicDev);
	if (m_pDashUI == nullptr)
		return E_FAIL;

	m_pSlotUI = CSlotUI::Create(pGraphicDev);
	if (m_pSlotUI == nullptr)
		return E_FAIL;

	m_pShopUI = CShopBG::Create(pGraphicDev);
	if (nullptr == m_pShopUI)
		return E_FAIL;	

	CEventMgr::GetInstance()->Subscribe(EVENT_STAGE_END, this);
	CEventMgr::GetInstance()->Subscribe(EVENT_ROOM_CHANGE, this);
	CEventMgr::GetInstance()->Subscribe(EVENT_NEXT_STAGE, this);
	CEventMgr::GetInstance()->Subscribe(EVENT_DRINK, this);
	CEventMgr::GetInstance()->Subscribe(EVENT_TAKEDOWN, this);
	CEventMgr::GetInstance()->Subscribe(EVENT_VIEW_EVENT_END, this);

	return S_OK;
}

void CUIManager::Update_GameObject(const _float& fTimeDelta)
{
	if (m_mapUI.count(m_eNowState) == 0)
		return;

	for (auto iter = m_mapUI[m_eNowState].begin(); iter != m_mapUI[m_eNowState].end(); )
	{
		_int result = (*iter)->Update_GameObject(fTimeDelta);

		if (result ==  RET_DEAD)
		{
			//Take Down UI가 끝나면 Return Dead 후 디폴트로 돌아감 
			if (m_eNowState == UI_TAKEDOWN)
			{
				CEventMgr::GetInstance()->Broadcast(EVENT_TAKEDOWN_END, NULL);
				Change_UIState(UI_DEFAULT);
				return;
			}

			IBasePool* pool = (*iter)->GetPool();
			if (pool == nullptr) Safe_Release((*iter));
			else (*iter)->ReturnToPool();

			iter = m_mapUI[m_eNowState].erase(iter);
		}
		else iter++;
	}

	if (m_bSnipermap == false)
	{
		if (m_eNowState == UI_DEFAULT)
		{
			if (m_bDash && m_pDashUI)
			{
				_int iResult = m_pDashUI->Update_GameObject(fTimeDelta);
				if (iResult == RET_DEAD)
					m_bDash = false;
			}

			if (m_bSlot && m_pSlotUI)
			{
				m_pSlotUI->Update_GameObject(fTimeDelta);
			}
		}

		if (m_bRenderEffectUI && m_pEffectUI)
		{
			m_pEffectUI->Update_GameObject(fTimeDelta);
		}

		if (m_bShop && m_pShopUI)
		{
			m_pShopUI->Update_GameObject(fTimeDelta);
		}
	}
	else
	{
		for (_int i = 0; i < m_iTargetCount; ++i)
		{
			m_vTargetUI[i]->Update_GameObject(fTimeDelta);
		}
	}
}

void CUIManager::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_mapUI.count(m_eNowState) == 0)
		return;

	for (auto* pUI : m_mapUI[m_eNowState])
	{
		pUI->LateUpdate_GameObject(fTimeDelta);
	};
	

	if (m_bSnipermap == false)
	{
		if (m_eNowState == UI_DEFAULT)
		{


			if (m_bDash && m_pDashUI)
			{
				m_pDashUI->LateUpdate_GameObject(fTimeDelta);
			}

			if (m_bSlot && m_pSlotUI)
			{
				m_pSlotUI->LateUpdate_GameObject(fTimeDelta);
			}
		}

		if (m_bRenderEffectUI && m_pEffectUI)
		{
			m_pEffectUI->LateUpdate_GameObject(fTimeDelta);
		}


		if (m_bShop && m_pShopUI)
		{
			m_pShopUI->LateUpdate_GameObject(fTimeDelta);
		}
	}
	else
	{
		for (_int i = 0; i < m_iTargetCount; ++i)
		{
			m_vTargetUI[i]->LateUpdate_GameObject(fTimeDelta);
		}
	}
	
}

HRESULT CUIManager::Add_ProtoType(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTexture* pCom_Texture = nullptr;

	// Floor Proto 
	pCom_Texture = Engine::CTexture::Create(pGraphicDev, CPhoneBG::GetTextureSource());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_PhoneBGTexture", pCom_Texture)))
		return E_FAIL;

	pCom_Texture = Engine::CTexture::Create(pGraphicDev, CShopItem::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_ShopItemTexture", pCom_Texture)))
		return E_FAIL;

	pCom_Texture = Engine::CTexture::Create(pGraphicDev, CSelectBG::GetTextureSource());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_SelectBGTexture", pCom_Texture)))
		return E_FAIL;

	pCom_Texture = Engine::CTexture::Create(pGraphicDev, CStageBG::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_StageBGTexture", pCom_Texture)))
		return E_FAIL;

	pCom_Texture = Engine::CTexture::Create(pGraphicDev, CShopBG::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_ShopBGTexture", pCom_Texture)))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_ShopBGAnimation",
		Engine::CAnimation::Create(pGraphicDev, pCom_Texture, CShopBG::GetAnimSources()))))
		return E_FAIL;

	pCom_Texture = Engine::CTexture::Create(pGraphicDev, CChat::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_ChatTexture", pCom_Texture)))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_ChatAnimation",
		Engine::CAnimation::Create(pGraphicDev, pCom_Texture, CChat::GetAnimSources()))))
		return E_FAIL;

	pCom_Texture = Engine::CTexture::Create(pGraphicDev, CMascot::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_MascotTexture", pCom_Texture)))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_MascotAnimation",
		Engine::CAnimation::Create(pGraphicDev, pCom_Texture, CMascot::GetAnimSources()))))
		return E_FAIL;

	pCom_Texture = Engine::CTexture::Create(pGraphicDev, CHeart::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_HeartTexture", pCom_Texture)))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_HeartAnimation",
		Engine::CAnimation::Create(pGraphicDev, pCom_Texture, CHeart::GetAnimSources()))))
		return E_FAIL;

	pCom_Texture = Engine::CTexture::Create(pGraphicDev, CHeartBeat::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_HeartBeatTexture", pCom_Texture)))
		return E_FAIL;

	pCom_Texture = Engine::CTexture::Create(pGraphicDev, CNoise::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_NoiseTexture", pCom_Texture)))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_NoiseAnimation",
		Engine::CAnimation::Create(pGraphicDev, pCom_Texture, CNoise::GetAnimSources()))))
		return E_FAIL;

	pCom_Texture = Engine::CTexture::Create(pGraphicDev, CPhonePlayer::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_PhonePlayerTexture", pCom_Texture)))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_PhonePlayerAnimation",
		Engine::CAnimation::Create(pGraphicDev, pCom_Texture, CPhonePlayer::GetAnimSources()))))
		return E_FAIL;

	pCom_Texture = Engine::CTexture::Create(pGraphicDev, CHPUI::GetTextureSource());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_HPUITexture", pCom_Texture)))
		return E_FAIL;

	pCom_Texture = Engine::CTexture::Create(pGraphicDev, CPlusUI::GetTextureSource());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_PlusTexture", pCom_Texture)))
		return E_FAIL;

	pCom_Texture = Engine::CTexture::Create(pGraphicDev, CTextBG::GetTextureSource());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TextBGTexture", pCom_Texture)))
		return E_FAIL;

	//TakeDown 
	pCom_Texture = Engine::CTexture::Create(pGraphicDev, CTakeDown::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TakeDownTexture", pCom_Texture)))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_TakeDownAnimation",
		Engine::CAnimation::Create(pGraphicDev, pCom_Texture, CTakeDown::GetAnimSources()))))
		return E_FAIL;


	pCom_Texture = Engine::CTexture::Create(pGraphicDev, CEffectUI::GetTextureSource());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_EffectUITexture", pCom_Texture)))
		return E_FAIL;

	pCom_Texture = Engine::CTexture::Create(pGraphicDev, CCursor::GetTextureSource());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_CursorTexture", pCom_Texture)))
		return E_FAIL;

	pCom_Texture = Engine::CTexture::Create(pGraphicDev, CDashUI::GetTextureSources());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_DashTexture", pCom_Texture)))
		return E_FAIL;

	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_DashAnimation",
		Engine::CAnimation::Create(pGraphicDev, pCom_Texture, CDashUI::GetAnimSources()))))
		return E_FAIL;

	pCom_Texture = Engine::CTexture::Create(pGraphicDev, CSlotUI::GetTextureSource());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_SlotUITexture", pCom_Texture)))
		return E_FAIL;

	pCom_Texture = Engine::CTexture::Create(pGraphicDev, CHudUI::GetTextureSource());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_HudUITexture", pCom_Texture)))
		return E_FAIL;

	pCom_Texture = Engine::CTexture::Create(pGraphicDev, CInfoUI::GetTextureSource());
	if (FAILED(CProtoMgr::GetInstance()->Ready_Prototype(L"Proto_InfoUITexture", pCom_Texture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIManager::Add_UI(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBaseUI* pUI = nullptr;
	UI_STATE eState = UI_DEFAULT;
	list<CBaseUI*> vUI;

	m_mapUI.insert({ eState, vUI });

#pragma region UI_STAGE_CLEAR
	// UI_STAGE_CLEAR
	
	eState = UI_STAGE_CLEAR;

	// STAGE BG
	pUI = CStageBG::Create(pGraphicDev);

	if (nullptr == pUI)
		return E_FAIL;

	vUI.push_back(pUI);

	// CChat BG
	pUI = CChat::Create(pGraphicDev);

	if (nullptr == pUI)
		return E_FAIL;

	vUI.push_back(pUI);


	// Mascot
	pUI = CMascot::Create(pGraphicDev);

	if (nullptr == pUI)
		return E_FAIL;

	vUI.push_back(pUI);

	// Heart
	pUI = CHeart::Create(pGraphicDev, 320.f, WINCY - 90.f);

	if (nullptr == pUI)
		return E_FAIL;

	vUI.push_back(pUI);

	// Heart
	pUI = CHeart::Create(pGraphicDev, 800.f, WINCY - 90.f);

	if (nullptr == pUI)
		return E_FAIL;

	vUI.push_back(pUI);

	//Heart_Beat
	pUI = CHeartBeat::Create(pGraphicDev, 560.f, WINCY - 90.f, 0);

	if (nullptr == pUI)
		return E_FAIL;

	vUI.push_back(pUI);

	//Heart_Beat
	pUI = CHeartBeat::Create(pGraphicDev, 400.f, WINCY - 90.f, 1);

	if (nullptr == pUI)
		return E_FAIL;

	vUI.push_back(pUI);


	pUI = CInfoUI::Create(pGraphicDev);

	if (nullptr == pUI)
		return E_FAIL;

	vUI.push_back(pUI);

	m_mapUI.insert({ eState, vUI });

	Sort_UI(eState);
#pragma endregion

	//Take Down
	pUI = CTakeDown::Create(pGraphicDev);
	m_mapUI[UI_TAKEDOWN].push_back(pUI);
	

	return S_OK;
}

void CUIManager::Sort_UI(UI_STATE eState)
{
	auto& plist = m_mapUI[eState];
	plist.sort([](auto* pA, auto* pB)
		{
			return pA->Get_Order() < pB->Get_Order();
		});
}

void CUIManager::Set_OnEffectUI(EFFECT_STATE eState)
{

	wstring wText;

	switch (eState)
	{
	case CUIManager::ES_DRINK:
		wText = L"생명 소다";
		m_pEffectUI->Init(false);
		m_pEffectUI->Set_Text(wText);
		break;
	case CUIManager::ES_TAKEDOWN:
		wText = L"즉결 처형";
		m_pEffectUI->Init(false);
		m_pEffectUI->Set_Text(wText);
		break;
	case CUIManager::ES_CLEAR:
		wText = L"클리어";
		m_pEffectUI->Init(D3DXCOLOR(0.f, 0.f, 0.f, 1.f));
		m_pEffectUI->Set_Text(wText);
		break;
	default:
		break;
	}

	m_bRenderEffectUI = true;
	
}

void CUIManager::Set_OnDashUI(_bool bDash)
{
	if (bDash && m_pDashUI)
	{
		m_pDashUI->Activate();
		m_bDash = true;
	}
	else
	{
		m_bDash = false;
	}
	

}

void CUIManager::Set_OnSlotUI(_bool bSlot)
{
	if (bSlot&& m_pSlotUI)
	{
		m_pSlotUI->Activate();
		m_bSlot = true;
	}
	else
	{
		m_bSlot = false;
	}
}

void CUIManager::Set_OnShopUI(_bool bShop)
{
	if (bShop&& m_pShopUI)
	{
		m_pShopUI->Activate();
		m_bShop = true;
	}
	else
	{
		m_bShop = false;
	}
}

void CUIManager::Set_VecTargetUI(LPDIRECT3DDEVICE9 pGraphicDev)
{
	m_vTargetUI.resize(10, nullptr);

	_float fX = 420.f;
	_float fY = WINCY - 100.f;
	_float fOfssetX = 90.f;
	_float fOfssetY = 100.f;
	CTargetUI* pTarget = nullptr;
	for (_int i = 0; i < 10; ++i)
	{
		_float fFixX = fX + (fOfssetX * (i % 5));
		_float fFixY = fY - (fOfssetY * (i / 5));
		pTarget = CTargetUI::Create(pGraphicDev, fFixX, fFixY);
		m_vTargetUI[i] = pTarget;
	}
	m_bSnipermap = true;
	m_iTargetCount = 0;
}

void CUIManager::Add_TargetUI()
{	
	m_iTargetIndex++;
	if (m_iTargetIndex >= 10)
		return;

	m_iTargetCount++;

	m_vTargetUI[m_iTargetIndex]->Activate();
}

void CUIManager::Clear_UIGroup()
{
	for (auto& pair : m_mapUI)
	{
		for_each(pair.second.begin(), pair.second.end(), [](auto* pUI) { pUI->SetDead(); });		
	}
	Safe_Release(m_pEffectUI);
	//Safe_Release(m_pDashUI);
	Safe_Release(m_pSlotUI);
	Safe_Release(m_pShopUI);
}

void CUIManager::Clear_SniperUI()
{
	for_each(m_vTargetUI.begin(), m_vTargetUI.end(), CDeleteObj());
	m_vTargetUI.clear();
	m_bSnipermap = false;
}

void CUIManager::Create_TextUI(LPDIRECT3DDEVICE9 pGraphicDev, COLLIDER_TAG eTag, _int iTimes)
{
	_vec3 vPos = { 150.f,200.f, 0.f };
	wstring deadSign;
	switch (eTag)
	{
	case Engine::TAG_NONE:
		deadSign = L"처치";
		break;
	case Engine::TAG_KICK:
		deadSign = L"발차기";
		break;
	case Engine::TAG_SLIDE:
		deadSign = L"슬라이딩";
		break;
	case Engine::TAG_LAVA:
		deadSign = L"용암구이";
		break;
	case Engine::TAG_ACID:
		deadSign = L"산성무침";
		break;
	case Engine::TAG_ELECTRIC:
		deadSign = L"찌릿찌릿";
		break;
	case Engine::TAG_KATANA:
		deadSign = L"스겅스겅";
		break;
	case Engine::TAG_HEAD:
		deadSign = L"헤드샷";
		break;
	case Engine::TAG_FAN:
		deadSign = L"갈갈이";
		break;
	default:
		break;
	}
	wstring timeText = to_wstring(iTimes) + L" sec";

	CTextBG* pUI = CPoolMgr::GetInstance()->Get_Object<CTextBG>();
	if (!pUI)
		return;

	pUI->Set_Text(deadSign,timeText);
	pUI->Set_StartPos(vPos);

	UI_STATE eTargetState = UI_DEFAULT;
	m_mapUI[eTargetState].push_back(pUI);
}

void CUIManager::Create_TextEffect(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _int iTimes)
{
	CTextUI* pUI = CPoolMgr::GetInstance()->Get_Object<CTextUI>();
	if (!pUI) 
		return;

	wstring timeText = to_wstring(iTimes) + L" sec";
	pUI->Set_Text(timeText);
	pUI->Set_StartPos(vPos);

	UI_STATE eTargetState = UI_DEFAULT;
	m_mapUI[eTargetState].push_back(pUI);
}

void CUIManager::OnEvent(EVENT_TYPE _type, EventData* _pData)
{

	switch (_type)
	{
	case Engine::EVENT_DRINK:
	{
		Set_OnEffectUI(ES_DRINK);
		break;
	}		
	case Engine::EVENT_ROOM_CHANGE:
		
		break;
	case Engine::EVENT_DOOR_OPEN:
		break;
	case Engine::EVENT_STAGE_END:
		Change_UIState(UI_STAGE_CLEAR);
		break;		
	case Engine::EVENT_NEXT_STAGE:
		Set_OnDashUI(false);
		Set_OnShopUI(false);
		//TODO : (처치) 폰트 UI 지우기 

		break;
	case Engine::EVENT_VIEW_EVENT_END:
		Set_OnShopUI(false);
		Change_UIState(UI_DEFAULT);
		break;
	case Engine::EVENT_TAKEDOWN:
		Set_OnEffectUI(ES_TAKEDOWN);
		break;
	default:
		break;
	}

}

void CUIManager::Change_UIState(UI_STATE eState)
{
	if (m_eNowState == eState)
		return;

	if (m_mapUI[eState].size() > 0)
	{
		for (auto* pUI : m_mapUI[m_eNowState])
		{
			pUI->Set_Off();
		};
	}

	m_eNowState = eState;

	if (m_mapUI[eState].size() == 0)
		return;

	for (auto* pUI : m_mapUI[m_eNowState])
	{
		pUI->Set_On();
	};
}
