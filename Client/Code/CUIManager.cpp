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

IMPLEMENT_SINGLETON(CUIManager)

CUIManager::CUIManager()
	: m_eNowState(UI_DEFAULT), m_pEffectUI(nullptr), m_bRenderEffectUI(false), m_pDashUI(nullptr),m_bDash(false)
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

	Safe_Release(m_pEffectUI);	
	Safe_Release(m_pDashUI);
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

	CEventMgr::GetInstance()->Subscribe(EVENT_STAGE_END, this);
	CEventMgr::GetInstance()->Subscribe(EVENT_DOOR_IN, this);
	CEventMgr::GetInstance()->Subscribe(EVENT_NEXT_STAGE, this);
	CEventMgr::GetInstance()->Subscribe(EVENT_DRINK, this);

	return S_OK;
}

void CUIManager::Update_GameObject(const _float& fTimeDelta)
{
	if (m_mapUI.count(m_eNowState) == 0)
		return;

	//_int result;
	//list<CBaseUI*> listMove;
	//for(auto* pUI : m_mapUI[m_eNowState])
	//{
	//	result = pUI->Update_GameObject(fTimeDelta);

	//	if (result < 0)
	//	{
	//		listMove.push_back(pUI);
	//	}
	//};

	//for (auto* pUI : listMove)
	//{
	//	auto it = find(m_mapUI[m_eNowState].begin(), m_mapUI[m_eNowState].end(), pUI);

	//	if (it != m_mapUI[m_eNowState].end())
	//	{

	//		m_mapUI[m_eNowState].erase(it);			
	//		m_mapUI[UI_DEACTIVATE].push_back(pUI);
	//	}
	//}

	//for (auto iter = m_mapUI[m_eNowState].begin(); iter != m_mapUI[m_eNowState].end(); )
	//{
	//	_int result = (*iter)->Update_GameObject(fTimeDelta);
	//	if (result < 0)
	//	{
	//		m_mapUI[UI_DEACTIVATE].push_back(*iter);
	//		iter = m_mapUI[m_eNowState].erase(iter);
	//	}
	//	else iter++;
	//}

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


	if (m_bRenderEffectUI)
	{
		m_pEffectUI->Update_GameObject(fTimeDelta);
	}

	if (m_bDash)
	{
		_int iResult = m_pDashUI->Update_GameObject(fTimeDelta);
		if (iResult == RET_DEAD)
			m_bDash = false;
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
	
	if (m_bRenderEffectUI)
	{
		m_pEffectUI->LateUpdate_GameObject(fTimeDelta);
	}

	if (m_bDash)
	{
		m_pDashUI->LateUpdate_GameObject(fTimeDelta);
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

	// SHOP BG
	pUI = CShopBG::Create(pGraphicDev);

	if (nullptr == pUI)
		return E_FAIL;

	vUI.push_back(pUI);

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

void CUIManager::Set_OnEffectUI(_bool bDrink)
{		
	wstring wText = bDrink ? L"생 명 소 다" : L"즉 결 처 형";	
	m_pEffectUI->Init();
	m_pEffectUI->Set_Text(wText);	

	m_bRenderEffectUI = true;
}

void CUIManager::Set_OnDashUI(_bool bDash)
{
	if (bDash)
	{
		m_pDashUI->Activate();
		m_bDash = true;
	}
	else
	{
		m_bDash = false;
	}
	

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
		Set_OnEffectUI(true);
		break;
	}		
	case Engine::EVENT_DOOR_IN:
		
		break;
	case Engine::EVENT_DOOR_OUT:
		break;
	case Engine::EVENT_STAGE_END:
		Change_UIState(UI_STAGE_CLEAR);
		break;		
	case Engine::EVENT_NEXT_STAGE:
		Change_UIState(UI_DEFAULT);
		break;
	case Engine::EVENT_END:
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
