#include "pch.h"
#include "CUIManager.h"

#include "CShopBG.h"
#include "CStageBG.h"
#include "CChat.h"
#include "CMascot.h"
#include "CHeart.h"
#include "CHeartBeat.h"
#include "CNoise.h"

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
	for_each(m_mapUI.begin(), m_mapUI.end(),
		[](auto& data)
		{
			auto& vUI = data.second;
			for (auto* pUI : vUI)
			{
				Safe_Release(pUI);
			}
			vUI.clear();
		});
	m_mapUI.clear();
}

HRESULT CUIManager::Ready_GameObject(LPDIRECT3DDEVICE9 pGraphicDev)
{
	if (FAILED(Add_UI(pGraphicDev)))
		return E_FAIL;

	CEventMgr::GetInstance()->Subscribe(EVENT_STAGE_END, this);
	CEventMgr::GetInstance()->Subscribe(EVENT_DOOR_IN, this);
	CEventMgr::GetInstance()->Subscribe(EVENT_NEXT_STAGE, this);

	return S_OK;
}

void CUIManager::Update_GameObject(const _float& fTimeDelta)
{
	if (m_mapUI.count(m_eNowState) < 0)
		return;

	for(auto* pUI : m_mapUI[m_eNowState])
	{
		pUI->Update_GameObject(fTimeDelta);
	};
}

void CUIManager::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_mapUI.count(m_eNowState) <= 0)
		return;

	for (auto* pUI : m_mapUI[m_eNowState])
	{
		pUI->LateUpdate_GameObject(fTimeDelta);
	};
	
}

HRESULT CUIManager::Add_UI(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBaseUI* pUI = nullptr;
	UI_STATE eState = UI_DEFAULT;

	// UI_STAGE_CLEAR
	vector<CBaseUI*> vUI;
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

	return S_OK;
}

void CUIManager::Sort_UI(UI_STATE eState)
{
	auto& vec = m_mapUI[eState];

	sort(vec.begin(), vec.end(), [](auto* pA, auto* pB)
		{
			return pA->Get_Order() < pB->Get_Order();
		});
}

void CUIManager::OnEvent(EVENT_TYPE _type, EventData* _pData)
{

	switch (_type)
	{
	case Engine::EVENT_MONSTER_DEAD:
		break;
	case Engine::EVENT_DOOR_IN:
		Change_UIState(UI_STAGE_CLEAR);
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
