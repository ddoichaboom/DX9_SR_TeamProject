#include "pch.h"
#include "CUIManager.h"
#include "CProtoMgr.h"

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


IMPLEMENT_SINGLETON(CUIManager)

CUIManager::CUIManager()
	: m_eNowState(UI_DEFAULT)
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
}

HRESULT CUIManager::Ready_GameObject(LPDIRECT3DDEVICE9 pGraphicDev)
{
	if (FAILED(Add_ProtoType(pGraphicDev)))
		return E_FAIL;


	if (FAILED(Add_UI(pGraphicDev)))
		return E_FAIL;

	CEventMgr::GetInstance()->Subscribe(EVENT_STAGE_END, this);
	CEventMgr::GetInstance()->Subscribe(EVENT_DOOR_IN, this);
	CEventMgr::GetInstance()->Subscribe(EVENT_NEXT_STAGE, this);
	//CEventMgr::GetInstance()->Subscribe(EVENT_MONSTER_DEAD, this);

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

	for (auto iter = m_mapUI[m_eNowState].begin(); iter != m_mapUI[m_eNowState].end(); )
	{
		_int result = (*iter)->Update_GameObject(fTimeDelta);
		if (result < 0)
		{
			m_mapUI[UI_DEACTIVATE].push_back(*iter);
			iter = m_mapUI[m_eNowState].erase(iter);
		}
		else iter++;
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

void CUIManager::Create_TextUI(LPDIRECT3DDEVICE9 pGraphicDev, COLLIDER_TAG eTag, _int iTimes)
{
	CTextBG* pUI = nullptr;
	UI_STATE eTargetState = UI_DEFAULT;
	UI_STATE eDeactiveState = UI_DEACTIVATE;

	_vec3 vPos = { 180.f,200.f, 0.f };
	wstring deadSign;
	switch (eTag)
	{
	case Engine::TAG_NONE:
		deadSign = L"Ã³Ä¡";
		break;
	case Engine::TAG_KICK:
		deadSign = L"¹ßÂ÷±â";
		break;
	case Engine::TAG_SLIDE:
		deadSign = L"½½¶óÀÌµù";
		break;
	case Engine::TAG_LAVA:
		deadSign = L"¿ë¾Ï±¸ÀÌ";
		break;
	case Engine::TAG_ACID:
		deadSign = L"»ê¼º¹«Ä§";
		break;
	case Engine::TAG_ELECTRIC:
		deadSign = L"Âî¸´Âî¸´";
		break;
	case Engine::TAG_KATANA:
		deadSign = L"½º°Ï½º°Ï";
		break;
	case Engine::TAG_HEAD:
		deadSign = L"Çìµå¼¦";
		break;
	default:
		break;
	}
	wstring timeText = to_wstring(iTimes) + L" sec";

	if (!m_mapUI[eDeactiveState].empty())
	{
		pUI = static_cast<CTextBG*>(m_mapUI[eDeactiveState].back());
		m_mapUI[eDeactiveState].pop_back();
		static_cast<CTextBG*>(pUI)->Init();
	}
	else
	{
		pUI = CTextBG::Create(pGraphicDev, vPos);
	}
	
	if (pUI == nullptr)
		return;

	pUI->Set_Text(deadSign, timeText);

	m_mapUI[eTargetState].push_back(pUI);
}

void CUIManager::OnEvent(EVENT_TYPE _type, EventData* _pData)
{

	switch (_type)
	{
	case Engine::EVENT_MONSTER_DEAD:
	{
		
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
