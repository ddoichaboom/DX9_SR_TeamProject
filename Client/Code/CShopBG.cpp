#include "pch.h"
#include "CShopBG.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CDInputMgr.h"

#include "CShopItem.h"

vector<TextureSource> CShopBG::m_vTextureSource =
{
	{ SHOP,  L"../Bin/Resource/Texture/UI/Shop_Loading_Begin.dds" },
	{ NOISE,  L"../Bin/Resource/Texture/UI/Shop_Loading_Noise.dds"},
	{ ONPAGE,  L"../Bin/Resource/Texture/UI/Shop_BackGround.dds"}
};

vector<AnimationSource>  CShopBG::m_vAnimSource =
{
	{ SHOP,1,2,2, true, 0.12f},
	{ NOISE,1,2,2, true, 0.12f, 1.f},
	{ ONPAGE,1,0,0, true, 0.12f}
};

CShopBG::CShopBG(LPDIRECT3DDEVICE9 pGraphicDev)
	: CBaseUI(pGraphicDev)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pAnimationCom(nullptr), m_pStateCom(nullptr)
	, m_fTime(0.f), m_fDelayTime(0.f), m_bDelay(false), m_bStateStop(false)
	, m_bRender(false)
{	
	ZeroMemory(m_pItem, sizeof(m_pItem));
}

CShopBG::CShopBG(const CShopBG& rhs)
	: CBaseUI(rhs)
	, m_pBufferCom(nullptr), m_pTransformCom(nullptr), m_pTextureCom(nullptr)
	, m_pAnimationCom(nullptr), m_pStateCom(nullptr)
	, m_fTime(0.f), m_fDelayTime(0.f), m_bDelay(false), m_bStateStop(false)
	, m_bRender(false)
{
	ZeroMemory(m_pItem, sizeof(m_pItem));
}

CShopBG::~CShopBG()
{
}

CShopBG* CShopBG::Create(PDIRECT3DDEVICE9 pGraphicDev)
{
	CShopBG* pShopBG = new CShopBG(pGraphicDev);

	if (FAILED(pShopBG->Ready_GameObject()))
	{
		Safe_Release(pShopBG);
		MSG_BOX("Shop BackGround Create Failed");
		return nullptr;
	}

	return pShopBG;
}

void CShopBG::CreateStateData()
{
	auto Mgr = CDataMgr<CShopBG>::GetInstance();
	if (Mgr->IsStateEmpty() == false) return;

	CState<CShopBG>* State = new CState<CShopBG>(&CShopBG::Begin_Idle, &CShopBG::Idle, nullptr);
	Mgr->AddState(SHOP, State);

	State = new CState<CShopBG>(&CShopBG::Begin_Noise, &CShopBG::Noise, nullptr);
	Mgr->AddState(NOISE, State);

	State = new CState<CShopBG>(&CShopBG::Begin_OnPage, &CShopBG::OnPage, nullptr);
	Mgr->AddState(ONPAGE, State);
}

HRESULT CShopBG::Add_Component()
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
		(Engine::CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_ShopBGAnimation"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animation", pComponent });
	return S_OK;
}

HRESULT CShopBG::Add_ShopItem()
{
	CShopItem* pItem = nullptr;

	_float padding = 125.f;
	_float startX = 300.f;
	_float startY = 300.f;

	for (_int i = 0; i < 3; ++i)
	{
		pItem = CShopItem::Create(m_pGraphicDev, startX + (padding * i), startY, i);
		m_pItem[i] = pItem;
	}

	return S_OK;
}

void CShopBG::Free()
{
	for (_int i = 0; i < 3; ++i)
		Safe_Release(m_pItem[i]);
	
	CDataMgr<CShopBG>::DestroyInstance();
	CBaseUI::Free();
}

HRESULT CShopBG::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	if (FAILED(Add_ShopItem()))
		return E_FAIL;

	CreateStateData();

    return S_OK;
}

_int CShopBG::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = CBaseUI::Update_GameObject(fTimeDelta);
	m_fTime += fTimeDelta;

	if (m_bDelay)
		m_fDelayTime += fTimeDelta;

	if (m_bRender)
	{
		for (_int i = 0; i < 3; ++i)
			m_pItem[i]->Update_GameObject(fTimeDelta);
	}

	
    return _int();
}

void CShopBG::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CBaseUI::LateUpdate_GameObject(fTimeDelta);
	m_pAnimationCom->Update_State(m_pStateCom->GetCurrentStateID());
	if (m_bRender)
	{
		for (_int i = 0; i < 3; ++i)
			m_pItem[i]->LateUpdate_GameObject(fTimeDelta);
	}
}

void CShopBG::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
	m_pAnimationCom->Render_Animation();
	m_pBufferCom->Render_Buffer();
}

void CShopBG::Rotate(ROTATION eType, const _float& fAngle)
{
	m_pTransformCom->Rotation(eType, fAngle);
}

void CShopBG::SetPos(_vec3 _pos)
{
	m_pTransformCom->Set_Pos(_pos.x - WINCX * 0.5f, -_pos.y + WINCY * 0.5f, 0.f);
}

void CShopBG::SetScale(_float fCX, _float fCY)
{
	m_pTransformCom->Set_Scale(fCX * 0.5f, fCY * 0.5f, 1.f);
}

void CShopBG::ChangeState(_uint nextStateID)
{
	m_fTime = 0.f;
	m_fDelayTime = 0.f;
	//현재 상태에 맞는 애니메이션으로 자동 전환
	m_pAnimationCom->Update_State(nextStateID);
	m_pStateCom->ChangeState<CShopBG>(nextStateID);
}

void CShopBG::Begin_Idle()
{
	m_bRender = false;
	m_fSizeX = 340.f;
	m_fSizeY = 195.f;
	m_fX = 404.f;
	m_fY = WINCY -5.f;
	m_vStartPos = { m_fX, m_fY, 0.f };
	m_vEndPos = { m_fX, m_fY - 150.f , 0.f };

	m_vStartScale = { m_fSizeX * 0.5f, m_fSizeY * 0.5f, 1.f };
	m_vEndScale = { m_fSizeX * 0.7f, m_fSizeY * 0.7f, 1.f };
	m_bDelay = true;
	m_fDelayTime = 0.f;
	SetPos(m_vStartPos);
	m_pTransformCom->Set_Scale(m_vStartScale);
}

void CShopBG::Idle()
{
	_vec3 vPos;
	_vec3 vScale;
	_float fTime;
	
	if (m_bDelay)
	{
		if (m_fDelayTime < 1.f)
		{
			fTime = m_fTime * 3.f;
			D3DXVec3Lerp(&vPos, &m_vStartPos, &m_vEndPos, fTime);
			m_pTransformCom->Set_Pos(vPos.x - WINCX * 0.5f, -vPos.y + WINCY * 0.5f, 0.f);	
			CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
			return;
		}
		else
		{
			m_bDelay = false;
			m_fTime = 0.f;
		}
	}

	fTime = m_fTime * 3.f;
	D3DXVec3Lerp(&vScale, &m_vStartScale, &m_vEndScale, fTime);

	if (fTime < 1.f)
	{
		m_pTransformCom->Set_Scale(vScale);
	}
	if (fTime > 1.f)
	{
		m_pTransformCom->Set_Scale(vScale);		
		ChangeState(NOISE);
		return;
	}
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
}

void CShopBG::Begin_Noise()
{
	m_bRender = false;
}

void CShopBG::Noise()
{
	if (m_pAnimationCom->CanEnd())
	{
		ChangeState(ONPAGE);
		return;
	}
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
}

void CShopBG::Begin_OnPage()
{
	// 아이템 3개 생성 혹은 초기화
	m_bRender = true;
}

void CShopBG::OnPage()
{
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);
}
